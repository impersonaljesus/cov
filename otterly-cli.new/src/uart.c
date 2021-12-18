/*-
 * Copyright (c) 2017 Esben Rossel
 * All rights reserved.
 *
 * Author: Esben Rossel <esbenrossel@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "main.h"

extern volatile uint8_t txBuffer[TxDataSize];
extern volatile uint8_t rxBuffer[2*CCDSize];


int open_tty(char *input_tty)
{
	int fd = -1;

	/* Correct path for input_tty */
	char *devf = "/dev/";
	char new_itty[100] = {0};
	snprintf(new_itty, sizeof(new_itty), "%s%s", devf, input_tty);

	/* open tty */
	fd = open(new_itty, O_RDWR | O_NOCTTY | O_SYNC);

	/* Does input_tty exist? */	
	if (fd == -1)
	{
		printf("Error opening %s: %s\n", new_itty, strerror(errno));
		exit(0);
	}

	/* Is input_tty a terminal? */
	if (!isatty(fd))
	{
		printf("Error opening %s: %s\n", new_itty, strerror(errno));
		/* if not, then close filedescriptor */
		close(fd);
		exit(0);
	}

	/* flush input/output - delay is needed since linux 2.6.20 */
	usleep(20000);
	tcflush(fd, TCIOFLUSH);

	return fd;
}


int UART_init(int tty_fd)
{
    struct termios tty;

	/* Get input_tty's attributes */
    if (tcgetattr(tty_fd, &tty) < 0)
	{
        printf("Error from tcgetattr: %s\n", strerror(errno));
        exit(0);
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

	/* Set input_tty's */
    if (tcsetattr(tty_fd, TCSAFLUSH, &tty) != 0)
	{
        printf("Error from tcsetattr: %s\n", strerror(errno));
        exit(0);
    }
    return 0;
}

int gio_setup(int tty_fd, GMainLoop *loop)
{
	GIOChannel *gio_tty;
	guint ret;
	/* flush */
	

	/* Open a gio-channel on input_tty */
	gio_tty = g_io_channel_unix_new (tty_fd);
	if (!gio_tty)
        g_error ("Error creating new GIOChannel!\n");

	/* setup the channel for non-canonical (ie binary) mode */	
	g_io_channel_set_encoding (gio_tty,NULL, NULL);
	//g_io_channel_set_buffered (gio_tty, FALSE);
	g_io_channel_set_buffer_size (gio_tty, (gsize) 8192);

	/* setup a watch in gtk_main() */
	//g_io_add_watch (gio_tty, G_IO_IN | G_IO_HUP | G_IO_ERR, (GIOFunc)rx_UART, cfgdata);

	ret = g_io_add_watch (gio_tty, G_IO_IN | G_IO_HUP | G_IO_ERR, (GIOFunc) rx_UART, loop);
	if (!ret)
        g_error ("Error creating watch!\n");
	return 0;
}


gboolean rx_UART (GIOChannel *gio_tty, GIOCondition condition, gpointer data)
{
	g_main_loop_quit((GMainLoop *)data);

	GIOStatus ret = 0;
	GError *err = NULL;
	gsize count = 2*CCDSize;
	gsize rlen = 0;

	if (condition & G_IO_HUP)
		g_error ("TTY hung up!\n");

//some things to do here:
	ret = g_io_channel_read_chars (gio_tty, (gchar*)rxBuffer, count, &rlen, &err);
	if (ret == G_IO_STATUS_ERROR)
		g_error ("Error reading: %s\n", err->message);
	
	printf ("Read %i bytes: \n", (int) rlen);

	/* Remove watch after completion */
	return FALSE;
}


void UART_txdata_setup(int sh_period, int icg_period, int avg_exps)
{
	/*	Setup the txBuffer */
	/* key */
	txBuffer[0] = 69;
	txBuffer[1] = 82;

	/* Break the 32-bit int sh into 4x 8-bit integers */
	txBuffer[2] = sh_period >> 24;
	txBuffer[3] = sh_period >> 16;
	txBuffer[4] = sh_period >> 8;
	txBuffer[5] = sh_period;

	/* Break the 32-bit int icg into 4x 8-bit integers */
	txBuffer[6] = icg_period >> 24;
	txBuffer[7] = icg_period >> 16;
	txBuffer[8] = icg_period >> 8;
	txBuffer[9] = icg_period;

	/* 10 is the continuous flag (which cannot be used with the current CLI, use the pyCCDGUI for this), 11 is the number of integrations to average */
	txBuffer[10] = 0;
	txBuffer[11] = avg_exps;
}

int UART_tx(int tty_fd)
{
	int wlen = 0;
	int ret = 0;
	
	/* Send settings to Nucleo */
	wlen = write(tty_fd, (uint8_t*)txBuffer, TxDataSize);
	if (wlen==-1)
		printf("TTY write error: %s\n", strerror(errno));

	/* Wait for all activity on tty_fd to cease */
	ret = tcdrain(tty_fd);
	if (ret==-1)
		printf("TTY drain error: %s\n", strerror(errno));

	printf("%i bytes written\n",wlen); 
	return wlen;
}
