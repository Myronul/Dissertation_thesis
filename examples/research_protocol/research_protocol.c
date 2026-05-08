#include "contiki.h"
#include "protocol_stack.h"
#include "protocol_messages.h"
#include "net/rime/rime.h"
#include "lib/random.h"
#include <stdio.h>
#include <string.h>

uint8_t SystemState = 0; /*global state for the entire system*/

static clock_time_t generate_random_time(void)
{
    int jitter = 1 + random_rand()%1000;
    clock_time_t t = ((CLOCK_SECOND * jitter) / 1000)+100; /*add 100 ticks to avoid 0 time*/
    printf("wait...%lu\n", t);
    if(t == 0) t = 1;
    
    return t;
}

static void Log_print()
{
    printf("Current state: %i\n", SystemState);
}

PROCESS(process_init_node, "Protocol Init Node");
AUTOSTART_PROCESSES(&process_init_node);

//PROCESS(process_state_data_node, "Protocol Process Data");
//AUTOSTART_PROCESSES(&process_state_data_node);

PROCESS_THREAD(process_init_node, ev, data)
{
    static struct etimer timer;
    static uint8_t flagTime = 0;
    static uint8_t counter = 0;
    static uint8_t counter_t = 0;


    PROCESS_BEGIN();

    init_com_channels();
    random_init(linkaddr_node_addr.u8[0] ^ (clock_time() & 0xFF));

    while(1)
    {
        if(SystemState == state_AUTODISCOVERY)
        {
            Log_print();
            clock_time_t t = generate_random_time();
            etimer_set(&timer, t);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
            message_discovery_searching();

            if(message_search_for_type_UC(msgType_UC_JOIN_EXISTING_NETWORK)!=255)
            {
                /*invite from an existing network have been sent*/
                /*TO DO*/

            }

            if(message_search_for_type_BC(msgType_BC_AUTODISCOVERY_START) == 1)
            {
                /*try if another nodes are searching too for a network*/
                counter_t = 3;
                counter = 0;
                protocol_reset_stack_BC();
                SystemState = state_LISTEN;
            }

        }

        /************************************************************************************************************ */

        if(SystemState == state_LISTEN)
        {   

            Log_print();

            if(message_search_for_type_UC(msgType_UC_JOIN_EXISTING_NETWORK)!=255)
            {
                /*If in the meantime an existing network sent an inviation, accept it and
                abort the new netowrk thing*/
            }

            if(message_process_all_BC(msgType_BC_AUTODISCOVERY_START) == 1)
            {
                printf("All recevied Message processed in LISTEN state, add counter\n");
                counter_t++;
            }

            uint8_t n = message_return_index_nr_nodes();
            clock_time_t t = 20 + (clock_time_t)20*n + generate_random_time();//+generate_random_time();
            etimer_set(&timer, t);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));               
            
            message_discovery_searching();
            counter++;

            if(counter == counter_t)
            {
                counter = 0;
                counter_t = 0;
                log_print_nods_id();
                SystemState = state_ELECT_LIDER_TEMP;
                continue;
                //PROCESS_YIELD();
            }

        }

        if(SystemState == state_ELECT_LIDER_TEMP)
        {
            message_clear_buffer_id(); /*clear the buffer of the ids of the nodes discovered in the previous round*/
            Log_print();
            PROCESS_YIELD();
        }

        PROCESS_PAUSE();

    }

    PROCESS_END();
}



/*TO DO: ADD thread that will clear the stack if remains full for too long*/
/*
PROCESS_THREAD(process_state_data_node, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    init_com_channels();
    random_init(linkaddr_node_addr.u8[0] ^ (clock_time() & 0xFF));

    while(1)
    {
        if(SystemState == state_AUTODISCOVERY)
        {
            
            if(message_search_for_type_BC(msgType_BC_AUTODISCOVERY_START) == 1)
            {
                
            }
        }

        if(SystemState == 1)
        {

        }

    }

    PROCESS_END();
}*/