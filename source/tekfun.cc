// tekfun.cc

#include "tekfun.h"

#define BUF_LEN 100000
extern "C" {
	//////// funzione init : parametro d'ingresso ip_device -> uscita clink
	
	CLINK *init(const char *ip) {
		
		char		cmd[1000];
		char		buf[BUF_LEN];
		int		ret;
		long		bytes_returned;
		CLINK		*clink;
		
		clink = new CLINK;
		
		
		ret = vxi11_open_device(ip,clink);
		
		if (ret != 0) {
			printf("Error: could not open device %s, quitting\n",ip);
		}
		
		
		
		memset(cmd, 0, 1000);		// initialize command string
		memset(buf, 0, BUF_LEN);	// initialize buffer
		
		strcpy(cmd, "ch1:bandwidth full;:ch1:coupling DC;:ch1:impedance fifty;:ch1:invert 0;:ch1:offset 0;:ch1:position 0;:ch1:probe 1;:ch1:volts 0.5;:fpanel:press ch1");
		vxi11_send(clink, cmd);
		
		memset(cmd, 0, 1000);
		strcpy(cmd, "ch2:bandwidth full;:ch2:coupling DC;:ch2:impedance fifty;:ch2:invert 0;:ch2:offset 0;:ch2:position 0;:ch2:probe 1;:ch2:volts 0.5;:fpanel:press ch2");
		vxi11_send(clink, cmd);
		
		memset(cmd, 0, 1000);
		strcpy(cmd, "ch3:bandwidth full;:ch3:coupling DC;:ch3:impedance fifty;:ch3:invert 0;:ch3:offset 0;:ch3:position 0;:ch3:probe 1;:ch3:volts 0.5;:fpanel:press ch3");
		vxi11_send(clink, cmd);
		
		memset(cmd, 0, 1000);
		strcpy(cmd, "ch4:bandwidth full;:ch4:coupling DC;:ch4:impedance fifty;:ch4:invert 0;:ch4:offset 0;:ch4:position 0;:ch4:probe 1;:ch4:volts 0.5;:fpanel:press ch4");
		vxi11_send(clink, cmd);
		
		
		for (int i=0;i<4;i++) {
			
			if (i==0) strcpy(cmd, "CH1?");
			if (i==1) strcpy(cmd, "CH2?");
			if (i==2) strcpy(cmd, "CH3?");
			if (i==3) strcpy(cmd, "CH4?");
			
			vxi11_send(clink, cmd);
			
			bytes_returned = vxi11_receive(clink, buf, BUF_LEN);
			if (bytes_returned > 0) {
				printf("%s\n",buf);
			}
			else {
				printf("*** [ NOTHING RECEIVED ] ***\n");
			}
			
		}
		
		///// init parameter for waveform
		
		memset(cmd, 0, 1000);
		strcpy(cmd, "data:encdg ascii;:horiziontal:resolution low;:data:start 0;:data:stop 500");
		vxi11_send(clink, cmd);
		
		
		
		return clink;
		
		
	}
	
	
	///////////////  funzione run
	
	waveform run(CLINK *clink) {
		
		char		cmd[1000];
		char		buf[BUF_LEN];
		long		bytes_returned;
		
		waveform matricedati;
		
		for (int i=0;i<4;i++) {
			for (int j=0;j<500;j++) {
				matricedati.data[i][j] = 0;
			}
		}
		
		for (int i=0;i<4;i++) {
			
			memset(cmd, 0, 1000);
			
			if (i==0) strcpy(cmd, "data:source ch1;:curve?");
			if (i==1) strcpy(cmd, "data:source ch2;:curve?");
			if (i==2) strcpy(cmd, "data:source ch3;:curve?");
			if (i==3) strcpy(cmd, "data:source ch4;:curve?");
			
			vxi11_send(clink, cmd);
			
			bytes_returned = vxi11_receive(clink, buf, BUF_LEN);
			if (bytes_returned > 0) {
				printf("Waveform %d\n",i);
				parsewfm(buf, matricedati.data[i]);
			}
			else {
				printf("*** [ NOTHING RECEIVED ] ***\n");
			}
			
		}
		
		for (int i=0;i<4;i++) {
			printf("CH%d =\n",i);
			for (int j=0;j<500;j++) {
				printf("%d ", matricedati.data[i][j]);
			}
			printf("\n");
		}
		
		return matricedati;
		
	}
	
	
	
	
	
	///// funzione parsing matrice
	
	void parsewfm(char *buf, int wfmpoint[500]) {
		
		char value[100];
		memset(value, 0, 100);
		
		int valore[10];
		int k = 0;
		int j=0;
		
		for (int i=0; i<strlen(buf); i++) {
			
			if (buf[i] != ',' && buf[i] != '\0' ) {
				value[j] = buf[i];
				j++;
			}
			
			if (buf[i] == ',' || buf[i] == '\0') {
				j = 0;
				wfmpoint[k] = atoi(value);
				//printf("Value n %d = %s / %d\n", nnum,value,valore[nnum]);
				memset(value, 0, 100);
				k++;
				
			}	
		}
		
		
		
		
		
		
	}
}











