#ifndef MSG_H
#define MSG_H

#include <stdio.h>

#define ___NORMAL___ "\033[m"
#define ___FG_RED___ "\033[31m"
#define ___FG_GRN___ "\033[32m"
#define ___FG_YLW___ "\033[33m"
#define ___FG_BLE___ "\033[34m"

#define LOG_CLIENT_FILE "talkme.log"
#define LOG_SERVER_FILE "server.log"

#define MSG(...) printf(__VA_ARGS__)

#define MSG_ERR(...); { MSG(" [%sERR%s] %s", \
                           ___FG_RED___, ___NORMAL___, ___FG_RED___ ) ; \
                       MSG(__VA_ARGS__) ;  \
                       MSG("%s", ___NORMAL___); }

#define MSG_DBG(...); { MSG(" [%sDBG%s] %s", \
                          ___FG_BLE___, ___NORMAL___, ___FG_BLE___ ) ; \
                       MSG(__VA_ARGS__) ;  \
                       MSG("%s", ___NORMAL___); }

#define MSG_INF(...); { MSG(" [%sINF%s] %s", \
                          ___FG_YLW___, ___NORMAL___, ___FG_YLW___ ) ; \
                       MSG(__VA_ARGS__) ; \
                       MSG("%s", ___NORMAL___); }

#define MSG_SUC(...); { MSG(" [%sSUC%s] %s", \
                          ___FG_GRN___, ___NORMAL___, ___FG_GRN___ ) ; \
                       MSG(__VA_ARGS__) ; \
                       MSG("%s", ___NORMAL___); }

#define LOG_MSG(...) { FILE *f = fopen(LOG_FILE, "a"); \
                       fprintf(f, __VA_ARGS__) \
                       fclose(f); }

#define LOG_MSG_ERR(...); { LOG_MSG(" [ERR]") ; \
                            LOG_MSG(__VA_ARGS__) ; }

#define LOG_MSG_DBG(...); { LOG_MSG(" [DBG]");\
                            LOG_MSG(__VA_ARGS__) ; }

#define LOG_MSG_INF(...); { LOG_MSG(" [INF]") ; \
                            LOG_MSG(__VA_ARGS__) ; }

#define LOG_MSG_SUC(...); { LOG_MSG(" [SUC]") ; \
                            LOG_MSG(__VA_ARGS__) ; }

#endif /* msg.h */
