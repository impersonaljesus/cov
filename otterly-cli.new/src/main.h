#ifndef MAINH
#define MAINH

/**** Libraries ****/

//#include <gtk/gtk.h>
#include <glib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
#include <termios.h>
#include <argp.h>
#include <error.h>
#include <errno.h>

/**** Header files ****/

#include "uart.h"
#include "gnuplot_i.h"


/**** Data definitions ****/
#define CCDSize 3694
#define TxDataSize 12


/**** UART definitions ****/

#define BAUDRATE B115200
//#define BAUDRATE 230400
//#define BAUDRATE 500000
//#define BAUDRATE 460800
//#define BAUDRATE 921600
//#define BAUDRATE 1000000
//#define BAUDRATE 1152000



#endif
