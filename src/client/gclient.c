#include <gtk/gtk.h>
#include "client.h"

bool client_apply_refresh_answer(char *answer)
{
	int i = 0;
	bool empty = false;
	char *ptr = NULL;
	char *attr[3] = {0};
	bool attrMgr[3] = {0};
	char *TempId  = malloc(20);
    	char *newUser = malloc(NAME_SIZE + STATUS_SIZE + 50);
	sprintf(newUser, "\b");

    	gdk_threads_enter();
	//MSG_DBG("[Client] | Answer : \'%s\'.\n", answer);

	attr[0] = malloc(10);
	if (attr[0] != NULL) {
		strcpy(attr[0], strtok(answer,":"));
		free(attr[0]);
	} else {
		return false;
	}

	g_list_free(lstReceiver);
	lstReceiver = NULL;
	lstReceiver = g_list_append(lstReceiver, "None");
	lstReceiver = g_list_append(lstReceiver, "All");

	while(!empty) {
		attr[0] = malloc(50);
		attr[1] = malloc(50);
		attr[2] = malloc(50);

		if ((attr[0] == NULL) || (attr[1] == NULL)
			|| (attr[2] == NULL))
		{
			MSG_DBG("[Client] | Attribute wasn`t allocated.\n");
			return false;
		}

		ptr = strtok(NULL, ":");
		if (ptr != NULL) {
			strcpy(attr[0], ptr);
			attrMgr[0] = true;
		} else {
			break;
		}

		ptr = strtok(NULL, ":");
		if (ptr != NULL) {
			strcpy(attr[1], ptr);
			attrMgr[1] = true;
		} else {
			break;
		}

		ptr = strtok(NULL, ":");
		if (ptr != NULL) {
			strcpy(attr[2], ptr);
			attrMgr[2] = true;
		} else {
			break;
		}

        if (strcmp(attr[1], User.name) == 0) {
            User.id = atoi(attr[0]);
            sprintf(TempId, "<b>Your id :</b>\n%d", User.id);
	        gtk_label_set_markup(GTK_LABEL(lblId), TempId);
        }

		lstReceiver = g_list_append(lstReceiver, attr[1]);
		sprintf(newUser, "%s  %s [id:%s] [%s]   \n", newUser,
				attr[1], attr[0], attr[2]);
	}

	gtk_combo_set_popdown_strings(GTK_COMBO(cmbReceiver), lstReceiver);
	gtk_text_buffer_set_text(tbfUsers, newUser, (gint)strlen(newUser));

	for(i = 0; (i < 3) && (attrMgr[i] == true); i++) {
		free(attr[i]);
		attrMgr[i] = false;
	}

	free(TempId);
	free(newUser);

	gdk_threads_leave();

	return true;
}

static gpointer client_receive_packet(gpointer data)
{
    int bytesReceived;
	fd_set master;
	FD_ZERO(&master);
	FD_SET(clientSockFD, &master);

	struct packet *recvMsg = malloc(sizeof(struct packet));
	for (;;) {
		if (select(fdmax+1, &master, NULL, NULL, NULL) == -1) {
			//printf(" [Client] | Error when select().\n");
			exit(4);
		}

		bytesReceived = recv(fdmax, recvMsg, sizeof(struct packet), 0);
		MSG_DBG(" [Client] | Received %d bytes.\n", bytesReceived);

		if (recvMsg->type != 0) {
			MSG_INF(" [Client] | Type : %d. Name : %s. packet : %s.\n",
					 recvMsg->type, recvMsg->name, recvMsg->msg);

			if (bytesReceived > 0) {
				switch (recvMsg->type) {
					case REFRESH:
						MSG_INF(" [Client] | Refresh request - answer.\n");
						client_apply_refresh_answer(recvMsg->msg);
						break;

					case MESSAGE:
					    MSG_INF(" [Client] | packet received.\n");
						if (strcmp((char*)gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cmbStatus)->entry)), "Offline") == 0) {
							break;
						}

						gdk_threads_enter();
						gtk_text_buffer_set_text(tbfInput,
											recvMsg->msg, -1);
						gdk_threads_leave();

						if (strcmp((char*)gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cmbStatus)->entry)), "Online") == 0) {
							client_sound();
						}
						break;

					default:
						MSG_ERR("[Client] | Unknown type of data.");
						gdk_threads_enter();
						gtk_text_buffer_set_text(tbfInput,
								"Unknown type of income data.", -1);
						gdk_threads_leave();
				}
		  	} else {
				MSG_ERR(" [Client] | Error when receive data.\n");
                gdk_threads_enter();
				gtk_text_buffer_set_text(tbfInput,
									"Server closed connection.", -1);
                gdk_threads_leave();
                return (NULL);
	  		}
		} else {
            gdk_threads_enter();
			gtk_text_buffer_set_text(tbfInput,
                         "Connection lost. Server doesn`t response.", -1);
            gdk_threads_leave();
            return (NULL);
        }
 	}
  	return (NULL);
}

OP_STATUS draw_gui(int argc, char *argv[])
{
	char *TempName;
	char *TempId;
    char *TempLeft;

// =====================================================================< HEADER
	gtk_init(&argc, &argv);
	//printf(" [Client] | Gtk initialized. \n");
	window = gtk_window_new	   (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title	   (GTK_WINDOW(window), " TalkMe | Messanger ");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 300);

	ltMain = gtk_vbox_new(FALSE, 0);

	sprtHead = gtk_hseparator_new();


	// 21 | number of symbols in string + '\n'
	TempName = malloc(21 + strlen(User.name));
	sprintf(TempName, "<b>Your name :</b>\n%s", User.name);

	// 29 = 19 + 10 | 19 number of symbols | 10 it is number of possible digits
	TempId 	 = malloc(20);
	sprintf(TempId, "<b>Your id :</b>\n%d", User.id);

	ltHeader = gtk_hbox_new(FALSE, 0);

	lblName  = gtk_label_new(NULL);
	lblId	 = gtk_label_new(NULL);
	lblStatus= gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(lblName), TempName);
	gtk_label_set_markup(GTK_LABEL(lblId), TempId);
	gtk_label_set_markup(GTK_LABEL(lblStatus), "<b>Your status : </b>");

	cmbStatus= gtk_combo_new();
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cmbStatus)->entry), false);
	lstStatus= NULL;
	lstStatus= g_list_append(lstStatus, "Offline");
	lstStatus= g_list_append(lstStatus, "Busy");
	lstStatus= g_list_append(lstStatus, "Online");
	gtk_combo_set_popdown_strings(GTK_COMBO(cmbStatus), lstStatus);
    btnStatus = gtk_button_new_with_label(" Accept ");

	sprtVert = gtk_vseparator_new();
	sprtVert1= gtk_vseparator_new();

	gtk_box_pack_start(GTK_BOX(ltHeader), lblName  , TRUE , TRUE , 10);
	gtk_box_pack_start(GTK_BOX(ltHeader), sprtVert , TRUE , TRUE , 10);
	gtk_box_pack_start(GTK_BOX(ltHeader), lblId    , TRUE , TRUE , 10);
	gtk_box_pack_start(GTK_BOX(ltHeader), sprtVert1, TRUE , TRUE , 10);
    gtk_box_pack_end  (GTK_BOX(ltHeader), btnStatus, TRUE , TRUE , 10);
	gtk_box_pack_end  (GTK_BOX(ltHeader), cmbStatus, FALSE, TRUE , 10);
	gtk_box_pack_end  (GTK_BOX(ltHeader), lblStatus, TRUE , TRUE , 10);


// =================================================================< USER PANEL
	ltUserPanel = gtk_hbox_new(FALSE, 0);

// Users list creation
// =============================================================================
	ltUsers    = gtk_vbox_new(FALSE, 0);

    ltFrmUsers    = gtk_hbox_new(FALSE, 0);
    frmUsers = gtk_frame_new(NULL);

	gtk_frame_set_label           (GTK_FRAME(frmUsers), " Users in network   ");
	gtk_frame_set_label_align     (GTK_FRAME(frmUsers), 0.15, 0.5);
	gtk_frame_set_shadow_type     (GTK_FRAME(frmUsers), GTK_SHADOW_ETCHED_OUT);

	tbfUsers = gtk_text_buffer_new(NULL);
	txvUsers = gtk_text_view_new_with_buffer(tbfUsers);

	gtk_container_set_border_width(GTK_CONTAINER(txvUsers), 5);
	gtk_text_view_set_editable    (GTK_TEXT_VIEW(txvUsers), FALSE);
	gtk_container_add             (GTK_CONTAINER(frmUsers), txvUsers);

    gtk_box_pack_start(GTK_BOX(ltFrmUsers), frmUsers, TRUE , TRUE , 0);
    gtk_box_pack_start(GTK_BOX(ltUsers), ltFrmUsers , TRUE , TRUE , 0);
// =============================================================================
// End users list creation

	sprtVert2 = gtk_vseparator_new();

// Messages zone creation
// =============================================================================
	ltMessages = gtk_vbox_new(FALSE, 0);

	// 'Clean' sub area
	ltClean   = gtk_hbox_new(FALSE, 0);
	btnClean  = gtk_button_new_with_label("Clean");
	chbtnAuto = gtk_check_button_new_with_label("Autoremove (30 sec)");

	gtk_box_pack_start(GTK_BOX(ltClean), btnClean , TRUE , TRUE , 0);
	gtk_box_pack_start(GTK_BOX(ltClean), chbtnAuto, FALSE, FALSE, 10);
	// End of 'Clean' sub area

	// 'Incoming' sub area
	frmInput = gtk_frame_new(NULL);

	gtk_frame_set_label           (GTK_FRAME(frmInput), " Incoming ");
	gtk_frame_set_label_align     (GTK_FRAME(frmInput), 0.95, 0.5);
	gtk_frame_set_shadow_type     (GTK_FRAME(frmInput), GTK_SHADOW_ETCHED_OUT);
	gtk_container_set_border_width(GTK_CONTAINER(frmInput), 6);

	tbfInput = gtk_text_buffer_new(NULL);
	txvInput = gtk_text_view_new_with_buffer(tbfInput);

	gtk_text_view_set_wrap_mode   (GTK_TEXT_VIEW(txvInput), GTK_WRAP_WORD_CHAR);

	gtk_container_set_border_width(GTK_CONTAINER(txvInput), 6);
	gtk_text_view_set_editable    (GTK_TEXT_VIEW(txvInput), FALSE);
	gtk_container_add             (GTK_CONTAINER(frmInput), txvInput);
	// End of 'Incoming' sub area

	// 'Outgoing messages' sub area
	ltOutput = gtk_hbox_new(FALSE, 0);
	frmOutput = gtk_frame_new(NULL);

	gtk_frame_set_label		 (GTK_FRAME(frmOutput), " Write a message ... ");
	gtk_frame_set_label_align(GTK_FRAME(frmOutput), 0.05, 0.5);
	gtk_frame_set_shadow_type(GTK_FRAME(frmOutput), GTK_SHADOW_ETCHED_OUT);

	tbfOutput = gtk_text_buffer_new(NULL);
	txvOutput = gtk_text_view_new_with_buffer(tbfOutput);

	gtk_text_view_set_wrap_mode	  (GTK_TEXT_VIEW(txvOutput),
								   GTK_WRAP_WORD_CHAR);
	gtk_container_set_border_width(GTK_CONTAINER(txvOutput), 6);
	gtk_container_add			  (GTK_CONTAINER(frmOutput), txvOutput);

	ltAttach = gtk_vbox_new(FALSE, 0);
	btnAttach = gtk_button_new_with_label("Attach");

	// 5 | number of digits in left value + '\n'
	lblLeft = gtk_label_new(NULL);
	TempLeft = malloc(50);
	sprintf(TempLeft, "<span foreground=\"gray\">Left : %d </span>", MSG_SIZE);
	gtk_label_set_markup(GTK_LABEL(lblLeft), TempLeft);

	gtk_box_pack_start(GTK_BOX(ltAttach), btnAttach, FALSE, TRUE , 0);
	gtk_box_pack_end  (GTK_BOX(ltAttach), lblLeft  , FALSE, TRUE , 0);
	gtk_box_pack_start(GTK_BOX(ltOutput), frmOutput, TRUE , TRUE , 0);
	gtk_box_pack_end  (GTK_BOX(ltOutput), ltAttach , FALSE, FALSE, 10);
	// End of 'Outgoing messages' sub area

	// 'Send' sub area
	ltSend = gtk_hbox_new(FALSE, 0);
	lblReceiver = gtk_label_new(" Choose receiver : ");
	cmbReceiver = gtk_combo_new();
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cmbReceiver)->entry), false);
	lstReceiver = NULL;
	lstReceiver = g_list_append(lstReceiver, "None");
	lstReceiver = g_list_append(lstReceiver, "All");

	gtk_combo_set_popdown_strings(GTK_COMBO(cmbReceiver), lstReceiver);

	btnSend = gtk_button_new_with_label("   Send   ");

	gtk_box_pack_start(GTK_BOX(ltSend), lblReceiver, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ltSend), cmbReceiver, FALSE, FALSE, 0);
	gtk_box_pack_end  (GTK_BOX(ltSend), btnSend    , FALSE, FALSE, 10);
	// End of 'Send' sub area

	// Packing sub areas into Messages zone
	gtk_box_pack_start(GTK_BOX(ltMessages), ltClean , FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ltMessages), frmInput, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ltMessages), ltOutput, TRUE , TRUE , 10);
	gtk_box_pack_end  (GTK_BOX(ltMessages), ltSend  , FALSE, FALSE, 0);
// =============================================================================
// End messages zone creation

	// Packing 3 areas into ltUserPanel
	gtk_box_pack_start(GTK_BOX(ltUserPanel), ltUsers   , FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(ltUserPanel), sprtVert2 , FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ltUserPanel), ltMessages, TRUE , TRUE , 10);

    gtk_box_pack_start(GTK_BOX(ltMain), ltHeader, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(ltMain), sprtHead, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(ltMain), ltUserPanel, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(window), ltMain);


	g_signal_connect(tbfOutput,  "changed",      G_CALLBACK(client_limit_event),  NULL);
	g_signal_connect(btnClean, 	 "released",     G_CALLBACK(client_clean_event),  NULL);
	g_signal_connect(btnSend,    "released",     G_CALLBACK(client_send_event),   NULL);
    g_signal_connect(btnStatus,  "released",     G_CALLBACK(client_status_event), NULL);
	g_signal_connect(window,     "delete-event", G_CALLBACK(client_delete_event), NULL);

	return OP_OK;
}

int main(int argc, char* argv[]){


    GThread *thread;
    GError *error = NULL;

    struct sockaddr_in server_addr;
    fd_set master;
    fd_set read_fds;


	if (argc < 3) {
	  MSG_INF("usage: ./gclient <name> <server_ip_address>\n");
	  exit(1);
	}

	if (argc > 1) {
		strcpy(User.name, argv[1]);
	} else {
		MSG_ERR("You should pass your name ");
		exit(1);
	}

	if (argc > 2) {
		strcpy(ServerIP, argv[2]);
	} else {
		MSG_ERR("You should pass your address ");
		exit(1);
	}

    MSG_INF("\n Server : %s\n", ServerIP);

    client_connect_request(&clientSockFD, &server_addr);
    MSG_INF(" [Client] | Client sent connection request. \n");

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0, &master);
    FD_SET(clientSockFD, &master);
    fdmax = clientSockFD;
	client_login();

    gdk_threads_init();

	if (draw_gui(argc, argv) != OP_OK) {
		MSG_ERR(" [Client] | Cannot start GUI session.\n");
	}

    /* Obtain gtk's global lock */
    gdk_threads_enter();

    thread = g_thread_new("Name", client_receive_packet, NULL);

    if(!thread){
        g_print("Error: %s\n", error->message);
        return(-1);
    }

    gtk_widget_show_all(window);

	gtk_main();

    /* Release gtk's global lock */
    gdk_threads_leave();

	return 0;
}

