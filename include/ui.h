#ifndef UI_H_
#define UI_H_

#include <gtk/gtk.h>
#include "cache.h"
#include "socket.h"

//static GtkWidget *window, *text, *text2, *tabs, *resp;
//static GtkTextBuffer *buffer;


static GtkWidget *window;
static GtkWidget *s1win;
static GtkWidget *s2win;
static GtkWidget *t_recv;
static GtkWidget *t_resp;
static GtkTextBuffer *buff_recv;
static GtkTextBuffer *buff_resp;

static pthread_mutex_t gtk_lock;
static pthread_mutex_t buffer_lock;
static pthread_mutex_t w_display_lock;

static pthread_mutex_t gtk_lock_2;
static pthread_mutex_t buffer_lock_2;
static pthread_mutex_t w_display_lock_2;

void *gtk_setup(void *);
void in_thread_gtk();

void init_gtk_lock();

static void unlock_recv(GtkWidget *, gpointer);
static void unlock_resp(GtkWidget *, gpointer);

void loadBufferRecv(char* buffer, int length);
void loadBufferResp(char* buffer, int length);

#endif //UI_H_

