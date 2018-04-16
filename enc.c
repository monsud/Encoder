//------------------- RT_ENC.C ---------------------- 

#include <linux/module.h>

#include <asm/io.h>

#include <asm/rtai.h>

#include <rtai_shm.h>

#include <rtai_sched.h>

#include "parameters.h"

 

static RT_TASK enc_task;
static RT_TASK speed_task;

static struct enc_str *enc_data;

static unsigned int semi_per = 10;

static void enc(int t) 
{
    enc_data->slit = 0;
    enc_data->home_slit = 0;
    unsigned int count = 0;
    unsigned int slit_count = 0;		
    unsigned int prev_slit = 0;
    while (1) {
        prev_slit = enc_data->slit;
        if (count%semi_per == 0) {
		enc_data->slit++;
		enc_data->slit%=2;
	}

	if (prev_slit==0&&enc_data->slit==1) //fronte di salita
		slit_count=(++slit_count)%8;
	
	if (slit_count==0) enc_data->home_slit=enc_data->slit;
	else enc_data->home_slit=0;

	//rt_printk("%d:\t\t %d %d\n",count,enc_data->slit,enc_data->home_slit);
	count++;
        rt_task_wait_period();

    }
}

static void speed(int t)
{
    unsigned int up = 0;
    while(1) {
	if (semi_per == 10) up = 0;
	else if (semi_per == 1) up = 1;
	
	if (up) semi_per++;
	else semi_per--;
	//rt_printk("Semi period changed to %d\n",semi_per*TICK_PERIOD);
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
