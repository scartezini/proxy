#include "../include/ui.h"

char** shared_buffer = NULL;
char** shared_buffer_2 = NULL;

void in_thread_gtk(){
	pthread_t tid;

	pthread_create(&tid, NULL, &gtk_setup, NULL);
	return;
}


void init_gtk_lock(){
	if(pthread_mutex_init(&gtk_lock, NULL) &&
       pthread_mutex_init(&buffer_lock, NULL) &&
	   pthread_mutex_init(&w_display_lock, NULL) &&
	   pthread_mutex_init(&gtk_lock_2, NULL) &&
       pthread_mutex_init(&buffer_lock_2, NULL) &&
	   pthread_mutex_init(&w_display_lock_2, NULL)) {

		printf("\n mutex init failed\n");
		exit(-5);
	}
	pthread_mutex_lock(&gtk_lock);
	pthread_mutex_lock(&gtk_lock_2);
}

void loadBufferRecv(char* buffer, int length){
	pthread_mutex_lock(&buffer_lock);
	pthread_mutex_lock(&w_display_lock);

	gtk_text_buffer_set_text(buff_recv,buffer,length);
	usleep(300);

	pthread_mutex_unlock(&w_display_lock);
	shared_buffer = &buffer;

	pthread_mutex_lock(&gtk_lock);
	pthread_mutex_unlock(&buffer_lock);

}


void loadBufferResp(char* buffer, int length){
	pthread_mutex_lock(&buffer_lock_2);
	pthread_mutex_lock(&w_display_lock_2);

	gtk_text_buffer_set_text(buff_resp,buffer,length);
	usleep(300);

	pthread_mutex_unlock(&w_display_lock_2);

	shared_buffer_2 = &buffer;

	pthread_mutex_lock(&gtk_lock_2);
	pthread_mutex_unlock(&buffer_lock_2);

}

static void unlock_recv(GtkWidget *, gpointer){

	GtkTextIter start, end;

	pthread_mutex_lock(&w_display_lock);
	gtk_text_buffer_get_start_iter(buff_recv,&start);
	gtk_text_buffer_get_end_iter(buff_recv,&end);

	gchar *g_buffer;
	g_buffer = gtk_text_buffer_get_slice(buff_recv,&start,&end,1);

	if(g_buffer != NULL){
		FILE *fp;
		fp = fopen(".tmp_recv","wb");
		fprintf(fp,"%s",g_buffer);
		fclose(fp);
	}

	gtk_text_buffer_get_start_iter(buff_recv,&start);
	gtk_text_buffer_get_end_iter(buff_recv,&end);
	gtk_text_buffer_delete(buff_recv,&start,&end);

	pthread_mutex_unlock(&w_display_lock);

	pthread_mutex_unlock(&gtk_lock);
}

static void unlock_resp(GtkWidget *, gpointer){

	GtkTextIter start, end;

	pthread_mutex_lock(&w_display_lock_2);

	gtk_text_buffer_get_start_iter(buff_resp,&start);
	gtk_text_buffer_get_end_iter(buff_resp,&end);

	gchar *g_buffer;
	g_buffer = gtk_text_buffer_get_slice(buff_resp,&start,&end,1);
	if(g_buffer != NULL){
		FILE *fp;
		fp = fopen(".tmp_resp","wb");
		fprintf(fp,"%s",g_buffer);
		fclose(fp);
	}
				

	gtk_text_buffer_get_start_iter(buff_resp,&start);
	gtk_text_buffer_get_end_iter(buff_resp,&end);
	gtk_text_buffer_delete(buff_resp,&start,&end);

	pthread_mutex_unlock(&w_display_lock_2);
	pthread_mutex_unlock(&gtk_lock_2);
}

void *gtk_setup(void *){
	GtkWidget *table;

	GtkWidget *fwd_recv;
	GtkWidget *fwd_resp;

	GtkWidget *hbox;
	GtkWidget *vbox;

	gtk_init(0, NULL);


	//Window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 1366, 768);
	gtk_window_set_title(GTK_WINDOW(window), "Proxy");

	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
	
	//Box
    hbox = gtk_hbox_new(0, 5);
    vbox = gtk_vbox_new(0, 3);
	
	//Scrolled window
	s1win = gtk_scrolled_window_new(NULL,NULL);
	s2win = gtk_scrolled_window_new(NULL,NULL);

	//Buttons
    fwd_recv = gtk_button_new_with_label("Forward receive");
    fwd_resp = gtk_button_new_with_label("Forward response");

	//table
	table = gtk_table_new(1, 2, TRUE);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);

	//Text receive
    t_recv = gtk_text_view_new();
    buff_recv = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_recv));

	//Text response
    t_resp = gtk_text_view_new();
    buff_resp = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_resp));
	
	//Signals
    g_signal_connect(fwd_recv, "clicked", G_CALLBACK(unlock_recv), window);
    g_signal_connect(fwd_resp, "clicked", G_CALLBACK(unlock_resp), window);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

	//Attach
    gtk_box_pack_start(GTK_BOX(hbox), fwd_recv, 0, 1, 0);
    gtk_box_pack_start(GTK_BOX(hbox), fwd_resp, 0, 1, 0);

	gtk_container_add(GTK_CONTAINER(s1win), t_recv);
	gtk_container_add(GTK_CONTAINER(s2win), t_resp);

	gtk_table_attach_defaults(GTK_TABLE(table), s1win, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), s2win, 1, 2, 0, 1);

    gtk_box_pack_start(GTK_BOX(vbox) , hbox, 0, 1, 0);
    gtk_box_pack_start(GTK_BOX(vbox) , table, 1, 1, 0);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);
	gtk_main();

	return NULL;
}

