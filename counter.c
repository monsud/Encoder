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
	int prev_slit = 0;
	int trovato = 0;

    	while (1) {

	switch (trovato){
	
	//prev_slit = enc->slit; //prendo l'onda e la metto in una variabile temporale
	
	case 0:
		if (prev_slit==0 && enc->slit==1) //fronte di salita
		trovato=1;

	break;

	case 1:
		enc->count=enc->count+1;
		if (prev_slit==1 && enc->slit==1) //se rimango alto devo uscire
		trovato = 0;
	break;

	}
	
        //rt_task_wait_period();
	prev_slit = enc->slit; //prendo l'onda e la metto in una variabile temporale
	rt_sleep(nano2count(3800000));
	rt_printk("Cont %d:\t Wave: %d\t Trov: %d\n",enc->count,enc->slit,trovato);
	
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
