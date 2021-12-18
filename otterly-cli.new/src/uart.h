/* UART specific */
int open_tty(char *input_tty);
int UART_init(int fd);
void UART_txdata_setup(int sh_period, int icg_period, int avg_exps);
int UART_tx(int tty_fd);
/* glib functions */
int gio_setup(int tty_fd, GMainLoop *loop);
gboolean rx_UART (GIOChannel *gio_tty, GIOCondition condition, gpointer data);
