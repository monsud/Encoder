//Contatore delle onde quadre prodotte dall'encoder
//sul fronte di salita

#include <linux/module.h>

#include <asm/io.h>

#include <asm/rtai.h>

#include <rtai_shm.h>

#include <rtai_sched.h>

#include "parameters.h"
 

static RT_TASK my_task;

static struct enc_str *enc;

 

static void counter(int t)

{

    unsigned int count = 0;

    int val = 0;

    while (1) {

        enc->slit = count;

        if ((count%50)==0){ //faccio il controllo ogni 5 ms del periodo
		val = val + 1;
		val = val % 2;
	}

        enc->value = val; //metto il valore in shm

        count++; //passo alla prossima onda

        rt_task_wait_period();

    }

}

int init_module(void)

{

    RTIME tick_period;

    rt_task_init(&my_task, (void *)counter, 1, STACK_SIZE, TASK_PRIORITY, 1, 0);

    enc = rtai_kmalloc(SHMNAM, sizeof(struct enc_str));

    tick_period = start_rt_timer(nano2count(TICK_PERIOD));

    rt_task_make_periodic(&my_task, rt_get_time() + tick_period, tick_period);

    return 0;

}

void cleanup_module(void)

{

    stop_rt_timer();

    rt_task_delete(&my_task);

    rtai_kfree(SHMNAM);

    return;

}
