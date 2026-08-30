#include "sys.h"

nodeType hostNodeType;
extern struct process process_heartbeat;
extern uint8_t timerHeartBeatRNG_counter;
extern uint8_t flagStartHeartBeatTimer;
extern uint16_t timerHeartBeatRNG_max;

void sys_init_random_node_type(void)
{
    hostNodeType = (nodeType)(random_rand()%3); /*randomly assign a node type*/
    printf("[INFO]Node %u assigned type: %u\n", node.id, hostNodeType);
}

static void sys_timer01_cb_(void* ptr)
{
    /* timer has expired: wake the heartbeat process */
    if(flagStartHeartBeatTimer)
    {
        process_post(&process_heartbeat, PROCESS_EVENT_CONTINUE, NULL);
    }

    ctimer_reset(&timer01);
}

void sys_heart_beat_handler_tx_start(void)
{
    if(timerHeartBeatRNG_max == 0)
    {
        timerHeartBeatRNG_max = 1000;
    }

    /* timerHeartBeatRNG_max is in milliseconds; ctimer uses clock ticks */
    clock_time_t delay_ticks = (clock_time_t)((CLOCK_SECOND * timerHeartBeatRNG_max + 999) / 1000);

    if(delay_ticks == 0) 
    {
        delay_ticks = 1;
    }

    ctimer_set(&timer01, delay_ticks, sys_timer01_cb_, NULL);
}