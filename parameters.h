//---------------- PARAMETERS.H ----------------------- 

#define TICK_PERIOD 5000000

#define STACK_SIZE 10000

#define SHMNAM 1234567

#define TASK_PRIORITY 1

struct enc_str

{
    unsigned int slit;
    unsigned int home_slit;
    unsigned int count; //valore per il contatore di onde
    unsigned int periodo; //valore per il periodo della home
};
