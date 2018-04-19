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

    struct enc_str *enc_data;

    signal(SIGINT, endme);

    enc_data = rtai_malloc (SHMNAM,1);

    while (!end) {

        printf(" Counter : %d Frequency : %d \n", enc_data->count, 600000000/enc_data->time);
	sleep(1);

    }

    rtai_free (SHMNAM, &enc);

    return 0;

}
