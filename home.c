//Modulo che ci permette di ottenere il periodo intercorso
//tra la prima posizione dell'home con la prossima

#include <linux/module.h>

#include <asm/io.h>

#include <asm/rtai.h>

#include <rtai_shm.h>

#include <rtai_sched.h>

#include "parameters.h"


static RT_TASK my_task;

static struct enc_str *enc;

static void period(int t)

{

    int trovato = 0;

    int inizio = 0;
    int fine = 0;
    int val;

    while (1) {

        enc->home_slit = val;

        if (val==1){ 
		inizio=rt_get_time(); //prendo il tempo iniziale
		trovato=1; //variabile booleana che indica che è stato trovato home
	}

        while (!trovato){
	if (val==1){
		fine=rt_get_time(); //prendo il tempo finale
		trovato=0; //resetto la var. booleana
		}
	periodo=fine-inizio;
	}

        rt_task_wait_period();

    }

}


int init_module(void)

{

    RTIME tick_period;

    start_rt_timer(nano2count(TICK_PERIOD));
    rt_task_init(&enc_task, (void *)enc, 1, STACK_SIZE, 11, 1, 0);
    rt_task_init(&speed_task, (void *)speed, 1, STACK_SIZE, 10, 1, 0);

    enc_data = rtai_kmalloc(SHMNAM, sizeof(struct enc_str));

    tick_period = nano2count(TICK_PERIOD);

    rt_task_make_periodic(&enc_task, rt_get_time() + tick_period, tick_period);
    rt_task_make_periodic(&speed_task, rt_get_time() + tick_period, 320*tick_period);

    return 0;

}

void cleanup_module(void)

{

    rt_task_delete(&enc_task);
    rt_task_delete(&speed_task);

    rtai_kfree(SHMNAM);

    return;

}
