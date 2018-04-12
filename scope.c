//---------------------- SCOPE.C ----------------------------

#include <stdio.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/mman.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <signal.h>

#include "/usr/realtime/include/rtai_shm.h"

#include "parameters.h"

 

static int end;

static void endme(int dummy) { end=1; }

 

int main (void)

{

    struct enc_str *enc;

    signal(SIGINT, endme);

    enc = rtai_malloc (SHMNAM,1);

    while (!end) {

        printf(" Counter : %d Value : %d \n", enc->cont, enc->slit);
	sleep(1);

    }

    rtai_free (SHMNAM, &enc);

    return 0;

}
