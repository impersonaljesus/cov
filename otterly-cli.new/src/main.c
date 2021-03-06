 /*
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

/* This software uses the gnuplot_i library written by N.Devillard */


#include "main.h"
#include <time.h>

/**** Stuff for the command line options ****/
const char *argp_program_version = "Otterly TCD1304 CLI 0.2";
const char *argp_program_bug_address = "<eros@chem.ku.dk>";

/* Program documentation. */
static char doc[] =
  "TCD1304 UART command line interface\
\vThis program is a command line interface for data acquisition from a Toshiba TCD1304DG driven by an STM32F401RE nucleo board.\n\n\
The SH- and ICG-period are 32-bit integers. SH defines the integration time of the CCD in relation to the fM-frequency in Hz, e.g:\n\
t_int = SH / fM = 2500 / 2000000 Hz = 1.25 ms\n\n\
ICG must be an integer multiple of SH, and ICG > 14775 and t_int > 10 us\n\n\
Average must be an integer between 1-15. \n\n\
Example: Otterly-CCD-CLI -s 2500 -i 100000 -n 4 -t ttyACM0 -o CCDraw.dat\n\n\
For full documentation visit <http://tcd1304.wordpress.com>";

/* The program takes only options, no arguments. */
static char args_doc[] = "";

/* Struct for arguments to pass from CLI to main() */
struct arguments
{
	int sh_period;
	int icg_period;
	int avg_exps;
	int plot;
	char *input_tty;
	char *output_file;
};

/* Options the program can recognize */
static struct argp_option options[] =
{
	{"SHperiod", 's', "Integer", 0, "The SH-period. Defines integration time.",0},
	{"ICGperiod", 'i', "Integer", 0, "The ICG-period.",0},
	{"Average", 'n', "Integer", 0, "Number of integrations to average (default is 1).",1},
	{"outputfile", 'o', "FILE", 0, "Output to FILE (default is output.dat).",2},
	{"ttyport", 't', "TTY", 0, "The TTY to which STM32F401 is connected (default is ttyACM0).",1},
	{"Plot", 'p', 0, 0, "Plot returned data with gnuplot-x11",0},
	{0}
};

/* Forward declarations */
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/* The parser */
static struct argp argp = { options, parse_opt, args_doc, doc,  0,0,0 };

volatile uint8_t txBuffer[TxDataSize];
volatile uint8_t rxBuffer[2*CCDSize];
double data[CCDSize];






int main(int argc, char *argv[])
{

	/* added by JF for unique Filenames"*/
    char dataFileNameBuffer[100];
    char graphFileNameBuffer[100];
    struct tm *timenow;

    time_t now = time(NULL);
    timenow = gmtime(&now);

    strftime(dataFileNameBuffer, sizeof(dataFileNameBuffer), "resultsFromTestAt_%Y-%m-%d_%H:%M:%S.dat", timenow);
    strftime(graphFileNameBuffer, sizeof(graphFileNameBuffer), "resultsFromTestAt_%Y-%m-%d_%H:%M:%S.dat", timenow);
    
	/* end this section*/

	GMainLoop *loop = g_main_loop_new (NULL, FALSE);
	struct arguments arguments;

	/* Set argument defaults */
	arguments.sh_period = 20000;
	arguments.icg_period = 1000000;
	arguments.avg_exps = 10;
	arguments.output_file = dataFileNameBuffer; // timestampped filename
	arguments.input_tty = "ttyACM0";
	arguments.plot = 1;

	/* Parse options from command line to arguments-struct */
	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	/* Setup the TxBuffer */
	UART_txdata_setup(arguments.sh_period, arguments.icg_period, arguments.avg_exps);

	/* Open input_tty */
	int tty_fd = open_tty(arguments.input_tty);

	/* Open output-file */
	FILE *fp = fopen(arguments.output_file, "w");
	if (fp == NULL) {
		fprintf(stderr, "Can't open output file %s!\n", arguments.output_file);
		return -1;
	}

	/* Set up tty */
	UART_init(tty_fd);

	/* 	Set up a gio channel to wait for incoming data.	*/
	gio_setup(tty_fd, loop);

	/* Transmit settings */
	UART_tx(tty_fd);

	/* Start the main loop and wait for data on tty */
  	g_main_loop_run (loop);


	/* Save data */
//	uint i,p;
	uint i;
//	for (i = 0; i < sizeof(rxBuffer); i+=2){
//		p = (unsigned int)rxBuffer[i+1] << 8 | rxBuffer[i];
//		fprintf(fp, "%d %d \n", i/2 + 1, p);
//	fprintf(fp, "#Data from the TCD1304 linear CCD\n#column 1 = pixelnumber,  column 2 = pixelvalue\n#Pixel 1-32 and 3679-3694 and are dummy pixels\n");
//	fprintf(fp, "#SH-period: %i    ICG-period: %i    Integration time: %i us\n", arguments.sh_period, arguments.icg_period, arguments.sh_period/2);
	for (i = 0; i < CCDSize; i++){
		data[i] = (unsigned int)rxBuffer[2*i+1] << 8 | rxBuffer[2*i];
		if (i%2 == 0)
			data[i] = data[i];
		fprintf(fp, "%i %i \n", i+1, (int) data[i]);
	}

	/* Close output-file */
	fclose(fp);
	
	system("./datacleaner"); //calls app to clean and invert
	
	FILE *cleanData = fopen("lf/finalresults.txt", "r"); //imports clean data
	double finalData[CCDSize];
	
	for (i = 0; i < CCDSize; i++)
	{
		//for (j=0; j<col; j++)
		//{
			fscanf(cleanData, "%lf", &finalData[i+1]/*[i]*/);
		//}
	}
	fclose(cleanData);
	
	//system("mkdir -p graphs");

	




	/* Wait for all activity on tty_fd to cease */
	if (tcdrain(tty_fd)==-1)
		printf("TTY drain error: %s\n", strerror(errno));
	/* Close input_tty */
	close(tty_fd);

	/* Plot the data? */
	if (arguments.plot == 1)
	{
		/* open gnuplot handle */
		gnuplot_ctrl *gh;
	    gh = gnuplot_init();

		/* setup gnuplot-dumb */
		gnuplot_cmd(gh, "set term dumb");
		//gnuplot_setstyle(gh, "lines");
		gnuplot_set_xlabel(gh, "Wavelength") ;
	    gnuplot_set_ylabel(gh, "Intensity") ;
		gnuplot_plot_x(gh, finalData, CCDSize, "Covad");
	
		/* close gnuplot handle */
		gnuplot_close(gh);
		
		/*second handle for png output*/
		/* open gnuplot handle */
		gnuplot_ctrl *gh2;
	    gh2 = gnuplot_init();

		/* setup gnuplot-PNG */
		gnuplot_cmd(gh2, "set term pngcairo");
		gnuplot_cmd(gh2, "set output strftime(\"graph_%Y-%b-%d%d_%H:%M.png\", time(NULL))");
		gnuplot_setstyle(gh2, "lines");
		gnuplot_set_xlabel(gh2, "Wavelength") ;
	    gnuplot_set_ylabel(gh2, "Intensity") ;
	    gnuplot_cmd(gh2, "set autoscale");
	    gnuplot_cmd(gh2, "set grid");
	    gnuplot_cmd(gh2, "set xtic auto");
	    gnuplot_cmd(gh2, "set ytic auto");
		gnuplot_plot_x(gh2, finalData, CCDSize, "Covad");
	
		/* close gnuplot handle */
		gnuplot_close(gh2);
	}

	return 0;

}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key)
    {
		case 's':
			arguments->sh_period = atoi(arg);
			break;
		case 'i':
			arguments->icg_period = atoi(arg);
			break;
		case 'n':
			arguments->avg_exps = atoi(arg);
			break;
		case 'o':
			arguments->output_file = arg;
			break;
		case 't':
			arguments->input_tty = arg;
			break;
		case 'p':
			arguments->plot = 1;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
