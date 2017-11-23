#ifndef DB_WORKER_H
#define DB_WORKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include "tm-types.h"

bool db_worker_add_client(char *filePath, const struct stClient *user);
bool db_worker_modify_name_by_socket_fd(char *filePath, const int *sFD, char *newName);
bool db_worker_modify_status_by_id(char *filePath, const int *id, char *nStatus);
bool db_worker_modify_status_by_socket_fd(char *filePath, const int *sFD, char *nStatus);
bool db_worker_remove_client_by_id(char *filePath, const int *id);
bool db_worker_remove_client_by_socket_fd(char *filePath, const int *sFD);
struct stClient* db_worker_get_client_by_id(char *filePath, const int *id);
struct stClient* db_worker_get_client_by_name(char *filePath, const char *name);
struct stClient* db_worker_get_client_by_socket_fd(char *filePath, const int *sFD);

#endif /* DB_WORKER_H */
