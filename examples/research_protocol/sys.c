#include "sys.h"

nodeType hostNodeType;
extern struct process process_heartbeat;
extern uint8_t timerHeartBeatRNG_counter;
extern uint8_t flagStartHeartBeatTimer;
extern uint8_t timerHeartBeatRNG_max;

void sys_init_random_node_type(void)
{
    hostNodeType = (nodeType)(random_rand()%3); /*randomly assign a node type*/
    printf("[INFO]Node %u assigned type: %u\n", node.id, hostNodeType);
}

static void sys_timer01_ISR_(void* ptr)
{
    /*Set timer ISR callback at 100ms*/
    
    if(flagStartHeartBeatTimer)
    {
        timerHeartBeatRNG_counter = (timerHeartBeatRNG_counter + 1)%timerHeartBeatRNG_max;
        
        if(timerHeartBeatRNG_counter == 0)
        {
            flagStartHeartBeatTimer = 0;
            process_post(&process_heartbeat, PROCESS_EVENT_CONTINUE, NULL);
        }
    }

    ctimer_set(&timer01, 100, sys_timer01_ISR_, NULL);
}

void sys_timer01_start(void)
{
    sys_timer01_ISR_(NULL);
}