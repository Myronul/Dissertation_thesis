#include "heart_beat.h"
#include "routing.h"

extern ROUTING_TABLE routingTable;
uint8_t timerHeartBeatRNG_counter = 0;
uint16_t timerHeartBeatRNG_max = 0;
uint8_t flagStartHeartBeatTimer = 0;

static uint32_t last_hb_time = 0;
static void debug_timer_time()
{
    uint32_t now = clock_time();
    uint32_t delta = (last_hb_time == 0) ? 0 : ((now - last_hb_time) * 1000UL) / CLOCK_SECOND;
    printf("DEBUG: Heartbeat time: %lu ms\n", (unsigned long)delta);
    last_hb_time = now;
}

void sys_start_heartbeat(void)
{
    /*
     * Function that will be called at the beggining of the 
     * state state_START_HEART_BEAT  
    */

    timerHeartBeatRNG_max = 100 + (100 * (random_rand() % 10)); /* ms */
    timerHeartBeatRNG_counter = 0;
    flagStartHeartBeatTimer = 1;
    printf("START_HEART_BEAT: next heartbeat window=%u ms\n", timerHeartBeatRNG_max);
}

void handle_heart_beat_send(void)
{
    if(flagStartHeartBeatTimer)
    {
        /* timer arrived, send heartbeat */
        printf("HEARTBEAT SENT...test...passed %u ms :)\n", timerHeartBeatRNG_max);
        debug_timer_time();
    }
}