//---------------- PARAMETERS.H ----------------------- 

#define PERIODIC 

#define TICK_PERIOD 5000000

#define STACK_SIZE 10000

#define SHMNAM 1234567

struct enc_str

{
    unsigned int slit;
    unsigned int home_slit;
    unsigned int count;
    unsigned int time;
};
