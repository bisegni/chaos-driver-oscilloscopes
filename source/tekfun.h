// tekfun.h

#ifndef	__TEKFUN__
#define	__TEKFUN__
#include "vxi11_user.h"

///// definiamo la struttura matrice

extern "C" {

struct waveform
{
     int data[4][500];
};




///// funzioni init/run etc etc

CLINK *init(const char *ip);

waveform run(CLINK *clink);

///// funzioni ausiliarie

void parsewfm(char *buf, int wfmpoint[500]);






}
#endif
