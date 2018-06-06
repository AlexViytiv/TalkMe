#include "talkme.h"

void * talkme_scan_lan_ip_segment(void *args)
{
	return NULL;
}

/* function to check who is a server */
char* talkme_get_server_addr()
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	memset(server_addr.sin_zero, ' ', sizeof server_addr.sin_zero);
    int i;
    char out[INET_ADDRSTRLEN];
    struct ifaddrs *iflist, *iface;

    if (getifaddrs(&iflist) < 0) {
        perror("getifaddrs");
        return "error";
	}

   	/* getting our IP */
    for (iface = iflist; iface; iface = iface->ifa_next) {
        int af = iface->ifa_addr->sa_family;
        const void *addr;
        char addrp[INET_ADDRSTRLEN];

        switch (af) {
            case AF_INET:
                addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
                break;
            default:
                addr = NULL;
        }

        if (addr) {
            if (inet_ntop(af, addr, addrp, sizeof addrp) == NULL) {
                perror("inet_ntop");
                continue;
            }
			if ((strcmp(iface->ifa_name, "enp3s0")   == 0)
			   || (strcmp(iface->ifa_name, "eth0")   == 0)
			   || (strcmp(iface->ifa_name, "lo")     == 0)
			   || (strcmp(iface->ifa_name, "wlp2s0") == 0)) {
                strcpy(out,addrp);
            }
        }
	}

	ip_found_flag = false;
 	/* getting LAN IP with /24 mask */
    char *fo, *so, *to, *newaddr = malloc(INET_ADDRSTRLEN);
    fo = strtok(out, ".");
    so = strtok(NULL,".");
    to = strtok(NULL, ".");
    int sockfd, ret;

	/* looking for server */
    for (i = 1; i < 255; i++) {
		MSG_DBG(" Checking %s.%s.%s.%d ...\n", fo, so, to, i);
		sprintf(newaddr, "%s.%s.%s.%d", fo, so, to, i);
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0) == -1)) {
			continue;
		}
		server_addr.sin_addr.s_addr = inet_addr(newaddr);
		ret = connect(sockfd, (struct sockaddr *)&server_addr,
					  sizeof(server_addr));
		if (ret < 0) {
			close(sockfd);
			continue;
		} else {
			MSG_SUC("%s %s", "Server found on", newaddr);
			return newaddr;  // get a server IP
		}
	}
    return "Can not find ip\n";
}

gboolean talkme_check_ip_struct (char *ip)
{
	int t = 0, s = 0, f = 0; // first, second and third octet parts
	int i = 0; // iteration variable
	int dots = 0;   // number of dots
	int octets = 0; // number of int groups
	int fullsz = 0; // len of full ip string
	int digits = 0; // number of digits in octet

	fullsz = strlen(ip);

	for (i = 0; i < fullsz; i++) {
		if (!isdigit(ip[0])) {
			return FALSE;
		}

		// do not allow two dots sequentially
		if (ip[i] == '.') {
			if (ip[i-1] == '.' || dots > 2) {
				return FALSE;
			}
			dots++;
			digits = 0;
			octets++;
			f = s = t = 0;
		}

		// if char is digit
		if (isdigit(ip[i])) {
			if (digits < 3) {
				t = s;
				s = f;
				f = ((int)ip[i]) - 48;   // convert char to digit

				if (digits <= 3 && ((100*t + 10*s + f) > 255)) {
					if (dots < 3) {
						ip[i] = '.';
						dots++;
						digits = 0;
						octets++;
						f = s = t = 0;
					} else {
						return FALSE;
					}
				} else {
					digits++;
				}

			} else if (ip[i-1] != '.' && dots < 3) {
				ip[i] = '.';
				dots++;
				digits = 0;
				octets++;
				f = s = t = 0;
			}
			else {
				return FALSE;
			}
		}
	}

	return TRUE;
}

gboolean talkme_login_event (GtkWidget *btnLogin, gpointer data)
{
    int result = 0;
    char *command = malloc(200);
	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_start_iter(tbfUName, &start);
	gtk_text_buffer_get_end_iter  (tbfUName, &end);
	strcpy(lgData.name,
		   (char*)gtk_text_buffer_get_text(tbfUName, &start, &end, FALSE));

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chbtnLAN))) {
		MSG_DBG(" Auto search started ...\n");
		strcpy(lgData.servIp, talkme_get_server_addr());
	} else {
		MSG_DBG(" Static address ...\n");
		gtk_text_buffer_get_start_iter(tbfSAddr, &start);
		gtk_text_buffer_get_end_iter  (tbfSAddr, &end);
		strcpy(lgData.servIp, (char*)gtk_text_buffer_get_text(tbfSAddr, &start, &end, FALSE));
	}

    sprintf(command, "./%s %s %s &", CLIENT, lgData.name, lgData.servIp);

	result = system(command);
    MSG_DBG(" Result = %d.\n", result);
    //free(command);
    exit(0);
    return TRUE;
}

gboolean talkme_check_name (char *name)
{
	int i;
	int size = strlen(name);
	if (size > NAME_SIZE) {
		return FALSE;
	}

	for (i = 0; i < size; i++) {
		if (name[i] == ' ' || name[i] == ':'
		|| name[i] == '\t' || name[i] == '\n'
		|| name[i] == '\v')
			return FALSE;
	}
	return TRUE;
}

gboolean talkme_check_ip (char *ip)
{
	int i;
	int size = 0;

	if (ip == NULL) {
		return FALSE;
	}

	size = strlen(ip);
	for (i = 0; i < size; i++) {
		if (ip[i] != '.' && (ip[i] < '0' || ip[i] > '9'))
			return FALSE;
	}

	return talkme_check_ip_struct(ip);
}

gboolean talkme_entry_name_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	strcpy(LastName, lgData.name);

	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_start_iter(tbfUName, &start);
	gtk_text_buffer_get_end_iter  (tbfUName, &end);
    strcpy(lgData.name, (char*)gtk_text_buffer_get_text(tbfUName, &start, &end, FALSE));

	if (!talkme_check_name(lgData.name)) {
		strcpy(lgData.name, LastName);
	} else {
		return FALSE;
	}

	g_signal_handler_block(tbfUName, nmsign);
	gtk_text_buffer_set_text(tbfUName, lgData.name, -1);
	g_signal_handler_unblock(tbfUName, nmsign);

	return TRUE;
}

gboolean talkme_entry_ip_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	strcpy(LastServIp, lgData.servIp);

	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_start_iter(tbfSAddr, &start);
	gtk_text_buffer_get_end_iter  (tbfSAddr, &end);
    strcpy(lgData.servIp, (char*)gtk_text_buffer_get_text(tbfSAddr, &start, &end, FALSE));

	if (!talkme_check_ip(lgData.servIp)) {
		strcpy(lgData.servIp, LastServIp);
	} else {
		return FALSE;
	}

	g_signal_handler_block(tbfSAddr, ipsign);
	gtk_text_buffer_set_text(tbfSAddr, lgData.servIp, -1);
	g_signal_handler_unblock(tbfSAddr, ipsign);

	return TRUE;
}


gboolean talkme_search_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chbtnLAN))) {
		gtk_text_view_set_editable(GTK_TEXT_VIEW(txvSAddr), FALSE);
	} else {
		gtk_text_view_set_editable(GTK_TEXT_VIEW(txvSAddr), TRUE);
	}
    return TRUE;
}



gboolean talkme_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

OP_STATUS draw_gui(int argc, char *argv[])
{
	strcpy(LastName, " ");
	strcpy(LastServIp, " ");

	ltMain   = gtk_vbox_new(FALSE, 0);

	lblUName = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(lblUName),
							"<span foreground=\"blue\">Your name</span>");

	frmUName = gtk_frame_new(NULL);
	tbfUName = gtk_text_buffer_new(NULL);
	txvUName = gtk_text_view_new_with_buffer(tbfUName);
	gtk_text_view_set_justification(GTK_TEXT_VIEW(txvUName),
									GTK_JUSTIFY_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(txvUName), 2);
	gtk_frame_set_shadow_type     (GTK_FRAME(frmUName), GTK_SHADOW_IN);
	gtk_container_add             (GTK_CONTAINER(frmUName), txvUName);

	chbtnLAN = gtk_check_button_new_with_label(" Search server in LAN ");

	lblSAddr  = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(lblSAddr),
							"<span foreground=\"green\">Server IPv4 address</span>");

	frmSAddr = gtk_frame_new(NULL);
	tbfSAddr = gtk_text_buffer_new(NULL);
	txvSAddr = gtk_text_view_new_with_buffer(tbfSAddr);
	gtk_text_view_set_justification(GTK_TEXT_VIEW(txvSAddr), GTK_JUSTIFY_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(txvSAddr), 2);
	gtk_frame_set_shadow_type     (GTK_FRAME(frmSAddr), GTK_SHADOW_ETCHED_OUT);
	gtk_container_add             (GTK_CONTAINER(frmSAddr), txvSAddr);

	btnLogin = gtk_button_new_with_label(" Login ");

	gtk_box_pack_start(GTK_BOX(ltMain), lblUName, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(ltMain), frmUName, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(ltMain), lblSAddr, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(ltMain), frmSAddr, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(ltMain), chbtnLAN, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(ltMain), btnLogin, TRUE, TRUE, 5);

	gtk_init(&argc, &argv);
	window = gtk_window_new	   (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title	   (GTK_WINDOW(window), " TalkMe | Login ");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);

	gtk_container_add(GTK_CONTAINER(window), ltMain);

	gtk_widget_show_all(window);


	nmsign = g_signal_connect(tbfUName, "changed", G_CALLBACK(talkme_entry_name_event), NULL);
	ipsign = g_signal_connect(tbfSAddr, "changed", G_CALLBACK(talkme_entry_ip_event), NULL);
	g_signal_connect(chbtnLAN, "released"    , G_CALLBACK(talkme_search_event) , NULL);
	g_signal_connect(btnLogin, "released"    , G_CALLBACK(talkme_login_event) , argv);
	g_signal_connect(window  , "delete-event", G_CALLBACK(talkme_delete_event), NULL);

	return OP_OK;
}

int main (int argc, char* argv[])
{
	draw_gui(argc, argv);
    gtk_main();

    return 0;
}
