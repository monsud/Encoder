//------------------- RT_ENC.C ---------------------- 

#include <linux/module.h>

#include <asm/io.h>

#include <asm/rtai.h>

#include <rtai_shm.h>

#include <rtai_sched.h>

#include "parameters.h"

static RTIME slack_1,slack_2;
static RT_TASK enc_task;
static RT_TASK speed_task;
static RT_TASK count_task;
static RT_TASK home_task;
static RT_TASK td_task;

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

static void counter(long t)

{
	RTIME start_time = rt_get_time();
	RTIME ri = start_time;
	RTIME di = ri + nano2count(100000);
	RTIME now,ti;

	unsigned int ci;
	int prev_slit = 0;

    	while (1) {
		ti = rt_get_time();
		if (prev_slit==0 && enc_data->slit==1) //fronte di salita
		enc_data->count=enc_data->count+1;

		prev_slit = enc_data->slit; //prendo l'onda e la metto in una variabile temporale

		now = rt_get_time();
		ci = count2nano (now-ti);
		slack_1 = count2nano (di-ri-ci);

		ri = next_period();
		di = ri + nano2count(1000000);

		//rt_printk("Cont %d:\t Wave: %d\t Slack: %d\n",enc_data->count,enc_data->slit,slack_1);
		rt_task_wait_period();
    }
}

static void home (int t)

{
	RTIME start_time = rt_get_time();
	RTIME ri = start_time;
	RTIME di = ri + nano2count(100000);
	RTIME prev_time = start_time;
	RTIME now,now_1,ti;

	unsigned int ci;
	unsigned int prev_home=0;
   	

    	while (1) {
		ti = rt_get_time();

		if (prev_home==0 && enc_data->home_slit==1){
			now = rt_get_time();
			enc_data->time = count2nano(now-prev_time);
			prev_time = now;
			//rt_printk("Time: %d:\t Frequency in RPM: %d\ Slack: %d\n",enc_data->time,6000000000/(enc_data->time),slack_2);
		}
	prev_home = enc_data->home_slit;

	now_1 = rt_get_time();
	ci = count2nano (now_1-ti);
	slack_2 = count2nano (di - ri -(now_1-ti));

	ri = next_period();
	di = ri + nano2count(1000000);

	rt_task_wait_period();
	
    }
}

static void td (int t){

	long media=0;
	int n=0,somma=0;

	while (1){
		if (n%100==0){
		somma=(slack_1+slack_2);
		media=somma/2;
		rt_printk("N. Esecution: %d:\t Average: %d\n ",n,media);
		}
		n++;
	rt_task_wait_period();
	}


}

int init_module(void)

{

    RTIME tick_period;

    start_rt_timer(nano2count(TICK_PERIOD));
    rt_task_init(&enc_task, (void *)enc, 1, STACK_SIZE, 11, 1, 0);
    rt_task_init(&speed_task, (void *)speed, 1, STACK_SIZE, 10, 1, 0);
    rt_task_init(&count_task, (void *)counter, 1, STACK_SIZE, 10, 1, 0);
    rt_task_init(&home_task, (void *)home, 1, STACK_SIZE, 10, 1, 0);
    rt_task_init(&td_task, (void *)td, 1, STACK_SIZE, 10, 1, 0);

    enc_data = rtai_kmalloc(SHMNAM, sizeof(struct enc_str));

    rt_set_periodic_mode();

    tick_period = nano2count(TICK_PERIOD);

    rt_task_make_periodic(&enc_task, rt_get_time() + tick_period, tick_period);
    rt_task_make_periodic(&speed_task, rt_get_time() + tick_period, 320*tick_period);
    rt_task_make_periodic(&count_task, rt_get_time() + tick_period, tick_period);
    rt_task_make_periodic(&home_task, rt_get_time() + tick_period, tick_period);  
    rt_task_make_periodic(&td_task, rt_get_time() + tick_period, 2*tick_period);
	
    return 0;

}

void cleanup_module(void)

{
    rt_task_delete(&enc_task);
    rt_task_delete(&speed_task);
    rt_task_delete(&count_task);
    rt_task_delete(&home_task);
    rt_task_delete(&td_task);

    rtai_kfree(SHMNAM);

    return;

}
