#include <arpa/inet.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "netinet/in.h"
#include "netdb.h"
#include "pthread.h"
#include "stdio.h"
#include "sys/types.h"
#include "sys/socket.h"
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "msg.h"
#include "tm-types.h"


int result;
// Socket File Descriptor
int clientSockFD;
int fdmax;
// Recieve thread
char ServerIP[IP_SIZE];

// =============================================================================
// Declaration of all client variables
// =============================================================================

GtkWidget 		*window;			// window widget

GtkWidget 		*ltMain;			// Layout contain ltHeader,
									// sprtHead,ltUserPanel

GtkWidget 		*ltHeader;			// Layout of header info
GtkWidget 		*lblName;			// Your name : <Name>
GtkWidget 		*lblId;				// Your id : <id>
GtkWidget 		*lblStatus;			// Your status -> [ComboBox]
GtkWidget 		*cmbStatus;			// ComboBox used for user status
GList			*lstStatus;			// Non-graphical list
GtkWidget 		*btnStatus;			// Button for status acception

GtkWidget 		*sprtHead;			// Separator after header
GtkWidget 		*sprtVert;			// Vertical separator for general goals
GtkWidget 		*sprtVert1;			// Vertical separator for general goals

GtkWidget 		*ltUserPanel;		// Layout contain ltUsers & ltMessages

GtkWidget 		*ltUsers;			// Layout for detected users
GtkWidget 		*ltFrmUsers;     	// Layout for frmUsers
GtkWidget       *frmUsers;          // Frame of users
GtkTextBuffer   *tbfUsers;          // Text buffer of users
GtkWidget       *txvUsers;          // Text view of users

GtkWidget 		*sprtVert2;			// Vertical separator for general goals

GtkWidget		*ltMessages;		// Layout for messages box of widgets
									// Contain ltCLean,txvInput,ltOutput,
									// ltSend

GtkWidget 		*ltClean;			// Layout contain btnClean, chbtnAuto
GtkWidget 		*btnClean;			// Button for cleaning input messages
GtkWidget 		*chbtnAuto;			// Checkbutton for autoremoving messages

GtkWidget 		*frmInput;			// Frame to make border of input
GtkTextBuffer	*tbfInput;			// TextBuffer for input
GtkWidget 		*txvInput;			// Label used to show input messages

GtkWidget 		*ltOutput;			// Layout contain frmOutput, ltAttach
GtkWidget 		*frmOutput;			// Frame that contain output area
GtkTextBuffer	*tbfOutput;			// TextBuffer for ouptput
GtkWidget 		*txvOutput;			// Text entry for user messages
GtkWidget 		*ltAttach;			// Layout contain btnAttach, lblLeft
GtkWidget 		*btnAttach;			// Button to attach resources
GtkWidget 		*lblLeft;			// Label shows number of max symbols


GtkWidget 		*ltSend;			// Layout contain lblReceiver,
									// cmbReceiver,btnSend
GtkWidget 		*lblReceiver;		// Receiver -> [ComboBox]
GList     		*lstReceiver;		// List of receiver options
GtkWidget 		*cmbReceiver;		// Combobox shows list of possible
									// receivers
GtkWidget 		*btnSend;			// Button perform send action


// =============================================================================
// Set of stardart functions
// =============================================================================

// Name	 	   : delete_event()
// Description : Function used when user close window.
// Functional  : Function call Gtk2 function to close main window and send
//				 signal to stop the program.
extern gboolean client_delete_event( GtkWidget *widget, GdkEvent *event, gpointer data);

// =============================================================================
// Set of help functions
// =============================================================================

void client_sound();

void client_login();

// Name	 	   : fillSendData()
// Description : used in signal call of btnSend.
// Functional  : form structure sendData & send it.
struct sendData*  client_fill_send_data();

// Name	 	   : refresh_event()
// Description : update needed data.
// Functional  : Parse income answer and move variables to different window
//               widgets.
void client_refresh_event_button(GtkWidget *btnClean, gpointer data);

// Name	 	   : compareToSort()
// Description : Function used to compare two elements in GList.
// Functional  : Function compare two strings.
gint client_compare_to_sort(gconstpointer a, gconstpointer b);

// =============================================================================
// Set of main functions
// =============================================================================

gboolean client_status_event(GtkWidget *cmbStatus, gpointer tbfInput);

void client_connect_request(int *clientSockFD, struct sockaddr_in *server_addr);

// Name	 	   : sortReceivers()
// Description : Function used to sort elements of cmbReceivers.
// Functional  : Function uses Gtk2 methods to sort Glist elements.
void client_sort_receivers(GList *lstReceivers);

// Name	 	   : clean_event()
// Description : Function used to clean incoming messages.
// Functional  : Function called when signal comes from btnClean or btnSend
//			     when autoremove checked. It clean tbfInput.
gboolean client_clean_event(GtkWidget *btnClean, gpointer tbfInput);

// Name	 	   : sendToServer()
// Description : Function used to transfer data to server.
// Functional  : Function called when signal comes from btnSend.
gboolean client_send_to_server(struct packet* msg, GtkTextBuffer *tbfInput);


// Name	 	   : receiveMessage()
// Description : Function used to take data from server.
// Functional  : Function works full time.
//static gpointer receiveMessage(gpointer data);

// Name	 	   : send_event()
// Description : Function used to send messages.
// Functional  : Function called when signal comes from btnSend. It sends
//				 message && when autoremove checked it clean tbfInput.
gboolean client_send_event(GtkWidget *btnSend, gpointer data);

// Name	 	   : limit_event()
// Description : Function used to limit outgoing messages.
// Functional  : Function called when signal comes from tbfOutput. It get size
// 				 of entered text and blocks it when number of symbols becomes
//				 over limit.
gboolean client_limit_event(GtkTextBuffer	*txvOutput, gpointer data);


