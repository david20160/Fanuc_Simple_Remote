#include <stdio.h>
#include <stdlib.h>
#include <iup.h>
#include "rs232.h"

#if __WIN32
	#define SEPARATOR '\\'
#else
	#define SEPARATOR '/'
#endif

Ihandle *tmr;
Ihandle *btnFile, *btnStop, *lblFile, *txtPort, *lblPort, *lblStatus, *txtBuffer, *txtTime, *window;

int nr_port;

FILE *f;
/* Tested with DRY RUN to 4000 FPM */
/* If want to make BLOCK->BLOCK run or M01, the MAX packet size must be 10 */
/* If not, work with more (i.e.: 192, 254, etc.) */
unsigned int package = 10; //192;
unsigned int timeDelay = 5;  /* 10 = ok // 5 = ok */
int stop = 1;

unsigned char buffer[1024];

int btnFile_CB (Ihandle *h) {

	Ihandle *dlg = IupFileDlg ();
	IupPopup (dlg, IUP_CENTER, IUP_CENTER);
	
	if (IupGetInt (dlg, "STATUS") != -1) {

		char *file = IupGetAttribute (dlg, "VALUE");
		
		char *p = strrchr (file, SEPARATOR) + 1; /* Show selectec file name */
		IupSetAttribute (lblFile, "TITLE", p);
		IupSetAttribute (lblFile, "EXPAND", "HORIZONTAL");

		IupSetAttribute (h, "ACTIVE", "NO");
		IupSetAttribute (btnStop, "ACTIVE", "YES");

		f = fopen (file, "r");
	
		IupSetAttribute (tmr, "RUN", "YES");
	
	}

	return IUP_DEFAULT;
}

int btnStop_CB (Ihandle *h) {

	/* Stop send timer */
	IupSetAttribute (tmr, "RUN", "NO");
	
	/* Close file to send */
	if (f != NULL) {
		fclose (f);
	}

	/* Enable new file select and disable transmission stop */
	IupSetAttribute (btnFile, "ACTIVE", "YES");
	IupSetAttribute (h, "ACTIVE", "NO");

	return IUP_DEFAULT;
}

int txtPort_CB (Ihandle *h) {
	
	int i = RS232_GetPortnr (IupGetAttribute (h, "VALUE"));
	if (i != -1) { /* Valid port */
		RS232_CloseComport (nr_port); /* Close previous port */
		nr_port = i;
		i = RS232_OpenComport (nr_port, 9600, "7E2", 1); /* Open the new port */
		if (i != 1) { 
			IupSetAttribute (lblPort, "TITLE", "OK");
		} else { /* Error opening the port */
			IupSetAttribute (lblPort, "TITLE", "Err");
		}
	} else { /* Not a valid port */
		IupSetAttribute (lblPort, "TITLE", "--");
	}
	
	return IUP_DEFAULT;
}

int txtTime_CB (Ihandle *h) {
	
	timeDelay = IupGetInt (h, "VALUE");
	
	return IUP_DEFAULT;
}

int txtBuffer_CB (Ihandle *h) {

	package = IupGetInt (h, "VALUE");
	
	return IUP_DEFAULT;
}


int checkRX (Ihandle *h) {
	
	int leido = RS232_PollComport(5, buffer, 1024);
	
	if (leido > 0) {
		if (buffer[leido-1] == 0x13) { /* 0X13 DC3 XOFF */
			IupSetAttributes (lblStatus, "TITLE=\" ||\",FGCOLOR=\"255 0 0\"");
			stop = 1;
		} else if (buffer[leido-1] == 0x11) { /* 0x11 DC1 XON */
			IupSetAttributes (lblStatus, "TITLE=\" ->\",FGCOLOR=\"0 255 0\"");
			stop = 0;
		}
	} else {
		if (!stop) { 
			int r = fread (buffer, 1, package, f);
			if (r) {
				printf ("->: %.*s\n", r, buffer);
				RS232_SendBuf(5, buffer, r);
			} else {
				/* Sending finished */
				IupSetAttribute (tmr, "RUN", "NO");
				
				/* Close the file */
				fclose (f);
				
				/* Enable new file selection and disable stop sending */
				IupSetAttribute (btnFile, "ACTIVE", "YES");
				IupSetAttribute (h, "ACTIVE", "NO");

			}
		}
	}
	
	fflush (stdout);
	
	return IUP_DEFAULT;
}

int main (void) {

	IupOpen (NULL, NULL);
/* Port row */	
	txtPort = IupText ("VALUECHANGED_CB");
	IupSetAttributes (txtPort, "");
	IupSetCallback (txtPort, "VALUECHANGED_CB", (Icallback) txtPort_CB);
	IupSetAttribute (txtPort, "VALUE", "COM1");
	
	lblPort = IupLabel ("--");
	IupSetAttribute (lblPort, "SIZE", "10");
	
	Ihandle *hb0 = IupHbox (IupLabel("Port"), txtPort, lblPort, NULL);
	
/* Delay between line sending */
	txtTime = IupText ("VALUECHANGED_CB");
	IupSetAttribute (txtTime, "TIP", "Delay between line sending to check the machine signal.\nMay be less but not 0. Units: ms.");
	IupSetInt (txtTime, "VALUE", timeDelay);
	IupSetCallback (txtTime, "VALUECHANGED_CB", (Icallback) txtTime_CB);
	
	Ihandle *hb1 = IupHbox (IupLabel("Check delay"), txtTime, NULL);

/* Size of data pack */	
	txtBuffer = IupText ("VALUECHANGED_CB");
	IupSetAttribute (txtBuffer, "TIP", "If not exist (M01 / BLOCK STOP) may be more.\ni.e.: 256");
	IupSetInt (txtBuffer, "VALUE", package);
	IupSetCallback (txtBuffer, "VALUECHANGED_CB", (Icallback) txtBuffer_CB);
	
	Ihandle *hb2 = IupHbox (IupLabel("Pack size"), txtBuffer, NULL);
	
/* Button to send */
	btnFile = IupButton ("File", "ACTION");
	IupSetCallback (btnFile, "ACTION", (Icallback) btnFile_CB);
	
	lblStatus = IupLabel (" ##");
	IupSetAttribute (lblStatus, "FONT", "Courier New, 10");
	IupSetAttribute (lblStatus, "FGCOLOR", "255 128 0");
	
	lblFile = IupLabel ("-- NONE --");
	
	Ihandle *vb2 = IupVbox (IupHbox (btnFile, lblStatus, NULL), lblFile, NULL);

/* Button to stop the sending */
	btnStop = IupButton ("stop", "ACTION");
	IupSetAttribute (btnStop, "EXPAND", "HORIZONTAL");
	IupSetAttribute (btnStop, "ACTIVE", "NO");
	IupSetCallback (btnStop, "ACTION", (Icallback) btnStop_CB);

	
/* Main program window */
	window = IupDialog (IupVbox (hb0, hb1, hb2, vb2, btnStop, NULL));
	IupSetAttributes (window, "SIZE=150x100,TITLE=\"Send to FANUC\"");
	
	IupShowXY (window, IUP_CENTER, IUP_CENTER);
	

	tmr = IupTimer ();
	IupSetInt (tmr, "TIME", timeDelay);
	IupSetCallback (tmr, "ACTION_CB", (Icallback) checkRX);
	
	txtPort_CB (txtPort); /* Iniciar con el puerto por defecto */

	
	IupMainLoop ();

	RS232_CloseComport(5);
	
	IupClose ();
	
	return 0;
}