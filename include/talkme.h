#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "msg.h"
#include "tm-types.h"

GtkWidget   	*window;

GtkWidget   	*ltMain;
GtkWidget   	*lblUName;
GtkWidget       *frmUName;
GtkTextBuffer   *tbfUName;
GtkWidget       *txvUName;
GtkWidget		*chbtnLAN;
GtkWidget   	*lblSAddr;
GtkWidget   	*chbtnAuto;
GtkWidget       *frmSAddr;
GtkTextBuffer   *tbfSAddr;
GtkWidget       *txvSAddr;
GtkWidget   	*btnLogin;

int  nmsign;
int  ipsign;
char LastName[NAME_SIZE];
char LastServIp[IP_SIZE];

struct login_data {
    char name[NAME_SIZE];
    char servIp[IP_SIZE];
} lgData;

bool ip_found_flag;

struct lan_ip_segment {
    char LAN_ADDRESS[INET_ADDRSTRLEN];
    char LAN_MASK[INET_ADDRSTRLEN];
    
}

