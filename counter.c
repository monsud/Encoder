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

    while (1) {

        int wave = enc->slit;
	int trovato=0;
	int count=0;

        switch (trovato){
	   case 0:
	    if (wave==1){
	     trovato=1;
     	     (enc->cont)++;
	    }
	break;
	   case 1:
	     if (wave==0){
		trovato=0;
	     }	
	break;
	}
	//rt_printk("%d:\t\t %d \n",count,enc->value);

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
