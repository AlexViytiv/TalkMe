#include "db-worker.h"

bool db_worker_add_client(char *filePath, const struct stClient *user)
{
	MSG_DBG("[Server] | Adding new client into DB.");
	FILE* dataFile = fopen(filePath,"ab");
	if(dataFile != NULL && user != NULL){
		fprintf(dataFile, "%d:%d:%s:%s:%s:\n",
				user->id, user->socketId, user->name, user->status, user->ip);
		fclose(dataFile);
		return true;
	}
	return false;
}


bool db_worker_modify_name_by_socket_fd(char *filePath, const int *sFD, char *newName){
	struct stClient *user = malloc(sizeof(struct stClient));

	char *tfilePath	      = malloc(sizeof(filePath)+2);
	sprintf(tfilePath, "%st", filePath);

	FILE *dataFile        = fopen(filePath, "rb");
	FILE *tdataFile		  = fopen(tfilePath, "ab");
	int n 				  = 1;
	size_t sz 			  = 100;
	bool empty 			  = false;
	char *buffer 		  = (char*)malloc(sz);
	char *buffercp 		  = (char*)malloc(sz);
	char *field[5];
	char rename[200];
	char delete[100];

	if(dataFile != NULL && tdataFile != NULL){
		while(getline(&buffer, &sz, dataFile) > 0)
		{
			strcpy(buffercp, buffer);
			field[0] = (char*)malloc(30);
			field[0] = strtok(buffer, ":");
            field[1] = (char*)malloc(30);
			field[1] = strtok(NULL, ":"); n++;

			if(atoi(field[1]) == *sFD){

				while (!empty && n < 5)
				{
					field[n] = (char*)malloc(30);
					field[n] = strtok(NULL, ":");
					if (field[n] == NULL)
						empty = true;
					else
						n++;
				}
				user->id 		= atoi(field[0]);
				user->socketId 	= atoi(field[1]);
                strcpy(user->name  , newName);
                strcpy(user->status, field[3]);
				strcpy(user->ip    , field[4]);
				n = 1;
				fprintf(tdataFile, "%d:%d:%s:%s:%s:\n",
					user->id, user->socketId, user->name, user->status, user->ip);
			}
			else {
				//printf(" Modify buffer : %s \n", buffercp);
				fprintf(tdataFile, "%s", buffercp);
			}
            n = 1;
		}
		fclose(dataFile);
		fclose(tdataFile);
	}
	else{
		return false;
	}

	sprintf(rename, "cp %s %s", tfilePath, filePath);
	sprintf(delete, "rm %s", tfilePath);
	system(rename);
	system(delete);

	//free(buffer);
	return true;
}


bool db_worker_modify_status_by_socket_fd(char *filePath, const int *sFD, char *nStatus){
	struct stClient *user = malloc(sizeof(struct stClient));

	char *tfilePath	      = malloc(sizeof(filePath)+2);
	sprintf(tfilePath, "%st", filePath);

	FILE *dataFile        = fopen(filePath, "rb");
	FILE *tdataFile		  = fopen(tfilePath, "ab");
	int n 				  = 1;
	size_t sz 			  = 100;
	bool empty 			  = false;
	char *buffer 		  = (char*)malloc(sz);
	char *buffercp 		  = (char*)malloc(sz);
	char *field[5];
	char rename[200];
	char delete[100];

	if(dataFile != NULL && tdataFile != NULL){
		while(getline(&buffer, &sz, dataFile) > 0)
		{
			strcpy(buffercp, buffer);
			field[0] = (char*)malloc(30);
			field[0] = strtok(buffer, ":");
            field[1] = (char*)malloc(30);
			field[1] = strtok(NULL, ":"); n++;

			//printf(" Status | Buffer : %s \n", buffercp);
			if(atoi(field[1]) == *sFD){

				while (!empty && n<5)
				{
					field[n] = (char*)malloc(30);
					field[n] = strtok(NULL, ":");
					if (field[n] == NULL)
						empty = true;
					else
						n++;
				}
				user->id 		= atoi(field[0]);
				user->socketId 	= atoi(field[1]);
                strcpy(user->name    , field[2]);
                strcpy(user->status  , nStatus);
				strcpy(user->ip      , field[4]);

				//printf(" Modify client - %d:%d:%s:%s:%s:\n",
				//	user->id, user->socketId, user->name, user->status, user->ip);
				fprintf(tdataFile, "%d:%d:%s:%s:%s:\n",
					user->id, user->socketId, user->name, user->status, user->ip);
			}
			else {
				//printf(" Modify buffer : %s \n", buffercp);
				fprintf(tdataFile, "%s", buffercp);
			}
            n = 1;
		}
		fclose(dataFile);
		fclose(tdataFile);
	}
	else{
		return false;
	}

	sprintf(rename, "cp %s %s", tfilePath, filePath);
	sprintf(delete, "rm %s", tfilePath);
	system(rename);
	system(delete);

	//free(buffer);
	return true;
}


bool db_worker_modify_status_by_id(char *filePath, const int *id, char *nStatus){
	struct stClient *user = malloc(sizeof(struct stClient));

	char *tfilePath	      = malloc(sizeof(filePath)+2);
	sprintf(tfilePath, "%st", filePath);

	FILE *dataFile        = fopen(filePath, "rb");
	FILE *tdataFile		  = fopen(tfilePath, "ab");
	int n 				  = 1;
	size_t sz 			  = 100;
	bool empty 			  = false;
	char *buffer 		  = (char*)malloc(sz);
	char *buffercp 		  = (char*)malloc(sz);
	char *field[5];
	char rename[200];
	char delete[100];

	if(dataFile != NULL && tdataFile != NULL){
		while(getline(&buffer, &sz, dataFile) > 0)
		{
			strcpy(buffercp, buffer);
			field[0] = (char*)malloc(30);
			field[0] = strtok(buffer, ":");
			//printf(" Buffer : %s \n", buffercp);
			if(atoi(field[0]) == *id){

				while (!empty && n<5)
				{
					field[n] = (char*)malloc(30);
					field[n] = strtok(NULL, ":");
					if (field[n] == NULL)
						empty = true;
					else
						n++;
				}

				user->id 		= atoi(field[0]);
				user->socketId 	= atoi(field[1]);
                strcpy(user->name, field[2]);
                strcpy(user->status, nStatus);
				strcpy(user->ip, field[4]);

				n = 1;
				//printf(" Modify client - %d:%d:%s:%s:%s:\n",
				//	user->id, user->socketId, user->name, user->status, user->ip);
				fprintf(tdataFile, "%d:%d:%s:%s:%s:\n",
					user->id, user->socketId, user->name, user->status, user->ip);
			}
			else {
				//printf(" Modify buffer : %s \n", buffercp);
				fprintf(tdataFile, "%s", buffercp);
			}
            n = 1;
		}
		fclose(dataFile);
		fclose(tdataFile);
	}
	else{
		return false;
	}

	sprintf(rename, "cp %s %s", tfilePath, filePath);
	sprintf(delete, "rm %s", tfilePath);
	system(rename);
	system(delete);

	//free(buffer);
	return true;
}


bool db_worker_remove_client_by_id(char *filePath, const int *id){

	char *tfilePath	      = malloc(sizeof(filePath)+2);
	sprintf(tfilePath, "%st", filePath);

	FILE *dataFile        = fopen(filePath, "rb");
	FILE *tdataFile		  = fopen(tfilePath, "ab");
	size_t sz 			  = 100;
	char *buffer 		  = (char*)malloc(sz);
	char *buffercp 		  = (char*)malloc(sz);
	char *field			  = (char*)malloc(10);
	char rename[200];
	char delete[100];

	if (dataFile != NULL && tdataFile != NULL)
    {
		while (getline(&buffer, &sz, dataFile) > 0)
		{
			strcpy(buffercp, buffer);
			field = strtok(buffer, ":");
			//printf(" Buffer : %s \n", buffercp);
			if (atoi(field) != *id)
            {
				fprintf(tdataFile, "%s", buffercp);
			}
		}
		fclose(dataFile);
		fclose(tdataFile);
	}
	else
    {
		return false;
	}

	sprintf(rename, "cp %s %s", tfilePath, filePath);
	sprintf(delete, "rm %s", tfilePath);
	system(rename);
	system(delete);

	 //free(buffer);
	return true;
}


bool db_worker_remove_client_by_socket_fd(char *filePath, const int *sFD)
{

	char *tfilePath	      = malloc(sizeof(filePath)+2);
	sprintf(tfilePath, "%st", filePath);

	FILE *dataFile        = fopen(filePath, "rb");
	FILE *tdataFile		  = fopen(tfilePath, "ab");
	size_t sz 			  = 100;
	char *buffer 		  = (char*)malloc(sz);
	char *buffercp 		  = (char*)malloc(sz);
	char *field			  = (char*)malloc(10);
	char rename[200];
	char delete[100];

	if (dataFile != NULL && tdataFile != NULL)
    {
		while(getline(&buffer, &sz, dataFile) > 0)
		{
			strcpy(buffercp, buffer);
			field = strtok(buffer, ":");
			field = strtok(NULL  , ":");
			//printf(" Buffer : %s \n", buffercp);
			if (atoi(field) != *sFD)
            {
				fprintf(tdataFile, "%s", buffercp);
			}
		}
		fclose(dataFile);
		fclose(tdataFile);
	}
	else
    {
		return false;
	}

	sprintf(rename, "cp %s %s", tfilePath, filePath);
	sprintf(delete, "rm %s", tfilePath);
	system(rename);
	system(delete);

	 //free(buffer);
	return true;
}


struct stClient* db_worker_get_client_by_id(char *filePath, const int *id)
{
	struct stClient *user = malloc(sizeof(struct stClient));
	FILE *dataFile        = fopen(filePath, "rb");
	int n 				  = 1;
	size_t sz 			  = 100;
	bool empty 			  = false;
	char *buffer 		  = (char*)malloc(sz);
	char *field[5];

	if (dataFile != NULL && user != NULL)
    {
		while(getline(&buffer, &sz, dataFile) > 0)
		{
			field[0] = (char*)malloc(30);
			field[0] = strtok(buffer, ":");

			if (atoi(field[0]) == *id)
            {
				while (!empty && n<5)
				{
					field[n] = (char*)malloc(30);
					field[n] = strtok(NULL, ":");
					if (field[n] == NULL)
						empty = true;
					else
						n++;
				}

				user->id 		= atoi(field[0]);
				user->socketId 	= atoi(field[1]);
                strcpy(user->name  , field[2]);
                strcpy(user->status, field[3]);
                strcpy(user->ip    , field[4]);

				n = 1;
				break;
			}

            n = 1;
		}
		fclose(dataFile);
	}

	return user;
}


struct stClient* db_worker_get_client_by_name(char *filePath, const char *name)
{
	struct stClient *user = malloc(sizeof(struct stClient));
	FILE *dataFile        = fopen(filePath, "rb");
	int n 				  = 1;
	size_t sz 			  = 100;
	bool empty 			  = false;
	char *buffer 		  = (char*)malloc(sz);
	char *field[5];

	if (dataFile != NULL && user != NULL)
    {
		while (getline(&buffer, &sz, dataFile) != -1)
		{
			field[0] = (char*)malloc(30);
			field[0] = strtok(buffer, ":");
			field[1] = (char*)malloc(30);
			field[1] = strtok(NULL, ":"); n++;
			field[2] = (char*)malloc(30);
			field[2] = strtok(NULL, ":"); n++;

			if (strcmp(field[2], name) == 0)
            {
				while (!empty && n<5)
				{
					field[n] = (char*)malloc(30);
					field[n] = strtok(NULL, ":");
					if (field[n] == NULL)
						empty = true;
					else
						n++;
				}

				user->id 		= atoi(field[0]);
				user->socketId 	= atoi(field[1]);
                strcpy(user->name, field[2]);
                strcpy(user->status, field[3]);
                strcpy(user->ip, field[4]);

				n = 1;
				break;
			}

            n = 1;
        }
		fclose(dataFile);
	}

	return user;
}


struct stClient* db_worker_get_client_by_socket_fd(char *filePath, const int *sFD)
{
	struct stClient *user = malloc(sizeof(struct stClient));
	FILE *dataFile        = fopen(filePath, "rb");
	int n 				  = 1;
	size_t sz 			  = 100;
	bool empty 			  = false;
	char *buffer 		  = (char*)malloc(sz);
	char *field[5];

	if (dataFile != NULL && user != NULL)
    {
		while (getline(&buffer, &sz, dataFile) != -1)
		{
			//printf(" [Server] | SockBuffer : %s.\n", buffer);
			field[0] = (char*)malloc(30);
			field[0] = strtok(buffer, ":");
			field[1] = (char*)malloc(30);
			field[1] = strtok(NULL, ":"); n++;

			if (atoi(field[1]) == *sFD)
            {
				//printf(" [Server] | Gotcha!\n");
				while (!empty && n<5)
				{
					field[n] = (char*)malloc(30);
					field[n] = strtok(NULL, ":");

					if (field[n] == NULL)
						empty = true;
					else
						n++;
				}

				user->id 		= atoi(field[0]);
				user->socketId 	= atoi(field[1]);
                strcpy(user->name, 	 field[2]);
                strcpy(user->status, field[3]);
                strcpy(user->ip, 	 field[4]);

				n = 1;
				break;
			}

            n = 1;
		}
		fclose(dataFile);
	}

	return user;
}
