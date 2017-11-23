#include "client.h"

// =============================================================================
// Set of standart functions
// =============================================================================

gboolean client_delete_event( GtkWidget *widget, GdkEvent *event, gpointer data)
{
    struct packet *sendMsg = (struct packet *)(malloc(sizeof(struct packet)));
    sendMsg->type = LOGOUT;
    strcpy(sendMsg->name, User.name);
    strcpy(sendMsg->msg,  "");

    client_send_to_server(sendMsg, tbfInput);
    gtk_main_quit();
    return FALSE;
}

// =============================================================================
// Set of help functions
// =============================================================================

void client_sound(){
    for(int i = 0; i< 3000; i++){
        printf("\a");
        usleep(100);
    }
}

gint client_compare_to_sort(gconstpointer a, gconstpointer b)
{
	return strcmp(a,b);
}

void client_sort_receivers(GList *lstReceiver)
{
	lstReceiver = g_list_sort(lstReceiver, client_compare_to_sort);
}

// =============================================================================
// Set of main functions
// =============================================================================

void client_connect_request(int *clientSockFD, struct sockaddr_in *server_addr)
{
	MSG_DBG("[Client] | Connection request was sent.")
	if ((*clientSockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
    //printf(" [Client] | Client socket created. \n");
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = inet_addr(ServerIP);
	memset(server_addr->sin_zero, ' ', sizeof server_addr->sin_zero);

	if(connect(*clientSockFD, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		MSG_ERR(" Connection to %s refused.\n", ServerIP);
		exit(1);
	}
    //printf(" [Client] | Client connected to server. \n");
}

gboolean client_status_event(GtkWidget *btnStatus, gpointer data)
{
	GtkWidget *entry = gtk_entry_new();
    struct packet *sendMsg = (struct packet *)(malloc(sizeof(struct packet)));
    sendMsg->type = STATUS;
    strcpy(sendMsg->name, User.name);

    entry = GTK_COMBO(cmbStatus)->entry;
	strcpy(sendMsg->msg, (char*)gtk_entry_get_text(GTK_ENTRY(entry)));

    client_send_to_server(sendMsg, tbfInput);

    return TRUE;
}

gboolean client_clean_event(GtkWidget *btnClean, gpointer tbfInput)
{
	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_start_iter(tbfInput, &start);
	gtk_text_buffer_get_end_iter  (tbfInput, &end);
	gtk_text_buffer_delete(tbfInput, &start, &end);

	return TRUE;
}

gboolean client_send_to_server(struct packet* sendMsg, GtkTextBuffer *tbfInput)
{
	result = send(clientSockFD, (void*)sendMsg, sizeof(struct packet), 0);
	if(result < 0){
        //printf(" [Client] | Client cannot send data[%d][Socket : %d].\n", errno, clientSockFD);
		gtk_text_buffer_set_text(tbfInput,
								" Transfer data to server failed.", -1);
	}
    return TRUE;
}


void client_login()
{
	struct packet *loginRequest = (struct packet *)(malloc(sizeof(struct packet)));

    //printf(" [Client] | Login request.\n");
	loginRequest->type = LOGIN;
	strcpy(loginRequest->msg, "#");
	strcpy(loginRequest->name, User.name);

	//printf(" [Client] | Type : %d. Name : %s. packet : %s.\n",
	//			 loginRequest->type, loginRequest->name, loginRequest->msg);
	client_send_to_server(loginRequest, tbfInput);
}

gboolean client_send_event(GtkWidget *btnSend, gpointer data)
{
	int Fseconds = time(NULL);
	//int Seconds  = Fseconds%60;
	int Minutes  = (Fseconds%3600)/60;
	int Hours	 = (Fseconds%86400)/3600 + TIME_ZONE;

	char *msg = malloc(NAME_SIZE + MSG_SIZE + 100);
	GtkTextIter start;
	GtkTextIter end;
	GtkWidget *entry = gtk_entry_new();

	struct packet *sendMsg = (struct packet *)(malloc(sizeof(struct packet)));

	gtk_text_buffer_get_start_iter(tbfOutput, &start);
	gtk_text_buffer_get_end_iter  (tbfOutput, &end);

	if(strcmp(gtk_text_buffer_get_text(tbfOutput, &start, &end, FALSE), "") == 0)
		return FALSE;

	sprintf(msg, " %s (%s%d:%s%d) :\n %s", User.name, Hours > 9?"":"0",
                Hours, Minutes>9?"":"0", Minutes,
				gtk_text_buffer_get_text(tbfOutput, &start, &end, FALSE));
	sendMsg->type = MESSAGE;
	strcpy(sendMsg->msg, msg);

	entry = GTK_COMBO(cmbReceiver)->entry;
	strcpy(sendMsg->name, (char*)gtk_entry_get_text(GTK_ENTRY(entry)));
	//printf(" [Client] | Sends to : %s.\n", sendMsg->name);

	if(strcmp(sendMsg->name, "None") == 0){
		return FALSE;
	}

    //printf(" [Client] | Sends info : %s.\n", sendMsg->name);
	client_send_to_server(sendMsg, tbfInput);
	client_clean_event(btnClean, tbfOutput);
	return TRUE;
}



gboolean client_limit_event(GtkTextBuffer	*tbfOutput, gpointer data)
{
	int msgLen = 0;
	int msgLeft = MSG_SIZE;
	char *Message;
	char *TempLeft;
	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_start_iter(tbfOutput, &start);
	gtk_text_buffer_get_end_iter  (tbfOutput, &end);
	Message = gtk_text_buffer_get_text(tbfOutput, &start, &end, FALSE);
	msgLen = strlen(Message);
	msgLeft = MSG_SIZE - msgLen;

	TempLeft = malloc(50);
	sprintf(TempLeft, "<span foreground=\"gray\">Left : %d </span>", msgLeft);

	gtk_label_set_markup(GTK_LABEL(lblLeft), TempLeft);

	if(gtk_text_iter_forward_chars(&start, MSG_SIZE)){
		gtk_text_buffer_delete(tbfOutput, &start, &end);
	}

	return TRUE;
}

