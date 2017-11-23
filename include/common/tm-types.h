#ifndef TM_TYPES_H
#define TM_TYPES_H

#define DATABASE "userInfo.db"
#define CLIENT   "talkme_client"
#define PORT     4547

#define MIN_UID     5
#define MAX_UID     1000000
#define IP_SIZE     16
#define MSG_SIZE    512
#define NAME_SIZE   30
#define STATUS_SIZE 10
#define BUFFER_SIZE 2048
#define DEF_NET_INTERFACE "wlp2s0"

#define TIME_ZONE 3

#define BUSY    "Busy"
#define ONLINE  "Online"
#define OFFLINE "Offline"

typedef enum { false, true } bool;

/*  Types of packet.
 */
enum {
    LOGIN,      /* packet signal, that user will log in     */
    REFRESH,    /* packet contain updated information       */
    MESSAGE,    /* packet contain some message              */
    STATUS,     /* packet contain updated user status       */
    LOGOUT      /* packet signal, that user will log out    */
};

/*  User info used when connected users displayed.
 */
enum {
    UID,        /* User ID                                  */
    UNAME,      /* User name                                */
    STCOLOR     /* Color for connected users showed in GUI  */
};

/* Operations result status. */
typedef enum {
    OP_OK,              /* 0 - Everything is ok. */
    OP_UNKNOWN_ERROR,   /* 1 - Unknown type of error */
    OP_SENDING_ABORT    /* 2 - Sending of packet was insuccessfull */
} OP_STATUS;

/* Structure of packets, that are used for client-server
 * communication.
 */
struct packet {
	int  type;
	char name[NAME_SIZE];
	char msg[MSG_SIZE];
};

/* Information about current client user.
 */
struct profile {
	char name[NAME_SIZE];
	int  id;
} User;

/* Structure uses to save information about client. Usually used
 * for reading/writing to database.
 */
 struct stClient {
    int id;                     // user ID
    int socketId;               // socket descriptor
    char name[NAME_SIZE];       // name of user
    char status[STATUS_SIZE];   // status of user
    char ip[IP_SIZE];           // ip address of user
};

#endif /* TM_TYPES_H */
