#include "heart_beat.h"
#include "routing.h"

extern ROUTING_TABLE routingTable;
uint8_t timerHeartBeatRNG_counter = 0;
uint8_t timerHeartBeatRNG_max = 0;
uint8_t flagStartHeartBeatTimer = 0;

static uint32_t last_hb_time = 0;
static void debug_timer_time()
{
    uint32_t now = clock_time();
    uint32_t delta = (last_hb_time == 0) ? 0 : now - last_hb_time;
    printf("DEBUG: Heartbeat time: %u ms\n", delta);
    last_hb_time = now;
}

void sys_start_heartbeat(void)
{
    /*
     * Function that will be called at the beggining of the 
     * state state_START_HEART_BEAT  
    */

    timerHeartBeatRNG_max = 100 + random_rand() % 150;
    timerHeartBeatRNG_counter = 0;
    flagStartHeartBeatTimer = 1;
    printf("START_HEART_BEAT: next heartbeat window=%i ticks\n", timerHeartBeatRNG_max);
}

void handle_heart_beat_send(void)
{
    if(flagStartHeartBeatTimer == 0)
    {
        /*timer arrived at 0, send heart beat*/
        printf("HEARTBEAT SENT...test...passed %i ms :)\n", timerHeartBeatRNG_max);
        /*reset timer flag*/
        flagStartHeartBeatTimer = 1;
        debug_timer_time();
    }
}