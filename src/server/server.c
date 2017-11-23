#include "server.h"


char* server_form_refresh_answer(char *filePath, int *counter){
	int i;
	struct stClient *user = malloc(sizeof(struct stClient));
	// 60 - maximal size of each unit
	char *answer = malloc(((*counter) * 60));

	sprintf(answer, "HEAD:");
	for (i = MIN_UID; i <= *counter; i++)
    {
		user = db_worker_get_client_by_id(filePath, &i);
		if (user->id >= MIN_UID && user->id < MAX_UID)
        {
			sprintf(answer, "%s%d:%s:%s:", answer, user->id, user->name, user->status);
		}
	}

	MSG_DBG(" [Server] | Formed answer : \n %s\n", answer);

	return answer;
}

static OP_STATUS server_forward_message(const int sFD, struct packet *recvMsg, struct packet *sendMsg)
{
	int i, dest_sFD;
	struct stClient *tmpCl = malloc(sizeof(struct stClient));

	if (strcmp(recvMsg->name, "All") == 0) {
		dest_sFD = 0;
	} else {
		dest_sFD = (db_worker_get_client_by_name(DATABASE, recvMsg->name))->socketId;
	}

	sendMsg->type = MESSAGE;

	tmpCl = db_worker_get_client_by_socket_fd(DATABASE, &sFD);
	strcpy(sendMsg->name, tmpCl->name);
	strcpy(sendMsg->msg, recvMsg->msg);
	if (dest_sFD == 0) {
		// broadcast type of packet
		// omitting server socket
		for (i = serverSockFD + 1; i <= fdmax; i++)
		{
			if (sFD != i
				&& send(i, sendMsg,
						sizeof(struct packet), 0) <= 0)
			{
				MSG_ERR(" [Server] | Can`t send to [%d/%d].\n",
						i, fdmax);
				return OP_SENDING_ABORT;
			}
		}
	}
	else if (send(dest_sFD, sendMsg,
					sizeof(struct packet), 0) < 0)
	{
		MSG_ERR(" [Server] | Cannot send to [s:%d].\n",
				dest_sFD);
		return OP_SENDING_ABORT;
	}
	return OP_OK;
}

static void server_update_clients(struct packet *recvMsg, struct packet *sendMsg)
{
	int i;
	char *answer;

	answer = server_form_refresh_answer(DATABASE, &idCounter);
	sendMsg->type = REFRESH;
	strcpy(sendMsg->name, recvMsg->msg);
	strcpy(sendMsg->msg, answer);

	for (i = serverSockFD + 1; i <= fdmax; i++)
	{
		if (send(i, sendMsg, sizeof(struct packet), 0) < 0)
		{
			MSG_ERR("[Server] | Cannot send data to %d.\n", i);
		}
	}
}


void* server_receive_packet(int sFD, fd_set *master, int serverSockFD, int fdmax)
{
	//int i;
	int bytesReceived;
    //int dest_sFD;
	//struct stClient *tmpCl = malloc(sizeof(struct stClient));
    struct packet *recvMsg = malloc(sizeof(struct packet));
    struct packet *sendMsg = malloc(sizeof(struct packet));

    bytesReceived = recv(sFD, recvMsg, sizeof(struct packet), 0);

    if (bytesReceived <= 0) {
        if (bytesReceived == 0) {
			MSG_INF(" [Server] | Socket #%d hung up.\n", sFD);
			db_worker_remove_client_by_socket_fd(DATABASE, &sFD);
		} else {
            MSG_ERR(" [Server] | Error when receiving data.\n");
		}

        close(sFD);
        FD_CLR(sFD, master);
    } else {
		/*
		MSG_DBG(" [Server] | Data received [T: %d; N: %s; M: %s]\n",
				 recvMsg->type, recvMsg->name, recvMsg->msg);
		*/
        switch(recvMsg->type)
        {
            case LOGIN: db_worker_modify_name_by_socket_fd(DATABASE, &sFD,
													 		recvMsg->name);
						server_update_clients(recvMsg, sendMsg);

                    	break;

			case REFRESH:
						server_update_clients(recvMsg, sendMsg);
						break;

			case MESSAGE:
						server_forward_message(sFD, recvMsg, sendMsg);
                        break;

			case STATUS :
						db_worker_modify_status_by_socket_fd(DATABASE, &sFD,
													  		 recvMsg->msg);
						server_update_clients(recvMsg, sendMsg);
                        break;

			case LOGOUT :
						db_worker_remove_client_by_socket_fd(DATABASE, &sFD);
						server_update_clients(recvMsg, sendMsg);
                        break;

            default: MSG_ERR(" [Server] | Error when checking packet type.\n");
        }
    }
	free(recvMsg);
	free(sendMsg);

	return NULL;
}


void server_connection_accept(fd_set *master, int *fdmax, int serverSockFD,
                       struct sockaddr_in *client_addr)
{
    int newServerSockFD;
    char IPaddress[IP_SIZE];
	socklen_t addrlen;

	addrlen = sizeof(struct sockaddr_in);
    newServerSockFD = accept(serverSockFD, (struct sockaddr *)client_addr, &addrlen);

    // if connection cannot be accepted
    if (newServerSockFD == -1)
    {
		MSG_ERR("[Server] | Cannot execute accept().\n");
		exit(1);
    // if connection installed
	}
    else
    {
        // add this socket to main set of sockets
		FD_SET(newServerSockFD, master);
        // update maximal socket id
		if (newServerSockFD > *fdmax)
        {
			*fdmax = newServerSockFD;
		}

        strcpy(IPaddress, inet_ntoa(client_addr->sin_addr));
		MSG_INF("[Server] | New connection from %s:%d \n",
               IPaddress, ntohs(client_addr->sin_port));

        struct stClient user;
        user.id = idCounter++;
        user.socketId = newServerSockFD;
        strcpy(user.name,"Unknown");
        strcpy(user.status, OFFLINE);
        strcpy(user.ip, IPaddress);

        db_worker_add_client(DATABASE, &user);
	}
}


void server_connect_request(int *serverSockFD, struct sockaddr_in *my_addr)
{
	int yes = 1;
    struct ifreq ifr;

	if ((*serverSockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
		MSG_ERR(" [Server] | Cannot create socket.\n");
		exit(1);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, ' ', sizeof my_addr->sin_zero);

	if (setsockopt(*serverSockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
		MSG_ERR(" [Server] | Problem when setsockopt().\n");
		exit(1);
	}

	if (bind(*serverSockFD, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1)
    {
		MSG_ERR(" [Server] | Unable to bind.\n");
		exit(1);
	}

	if (listen(*serverSockFD, 10) == -1)
    {
		MSG_ERR(" [Server] | Problem when listen.\n");
		exit(1);
	}

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, DEF_NET_INTERFACE, IFNAMSIZ-1);

    ioctl(*serverSockFD, SIOCGIFADDR, &ifr);
    MSG_INF(" [Server] | Server address [ip: %s | port: %d].\n",\
           inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),\
           PORT);

    fflush(stdout);
}


int main(int argc, char* argv[])
{
	int i;

    if (access(DATABASE, F_OK) != -1)
    {
        char delete[100];
        sprintf(delete, "rm %s", DATABASE);
	    system(delete);
    }

    // make values of master & read_fds equal to null
    FD_ZERO(&master);
	FD_ZERO(&read_fds);

    // binding main socket to server address
	server_connect_request(&serverSockFD, &serv_addr);
	FD_SET(serverSockFD, &master);

    // Temporary set value of maximal socket id to current socket id
    fdmax = serverSockFD;

    while(true)
    {
		read_fds = master;

        // we scan all sockets to find active
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        {
			MSG_ERR(" [Server] | Cannot execute select().\n");
			exit(1);
		}

        // we goes through every socket
		for (i = 0; i <= fdmax; i++)
        {
            // if such socket exist
			if (FD_ISSET(i, &read_fds))
            {
				if (i != serverSockFD) {
					server_receive_packet(i, &master, serverSockFD, fdmax);
				} else {
					server_connection_accept(&master, &fdmax, serverSockFD, &clnt_addr);
				}
			}
		}
	}

    return 0;
}

