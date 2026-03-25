#include "process.h"
#include"protocol.h"
#include <stdio.h>
#include "contiki.h"

static uint8_t SystemState = 0; /*global state for the entire system*/

PROCESS(process_init_node, "Protocol Init Node");
AUTOSTART_PROCESSES(&process_init_node);


PROCESS_THREAD(process_init_node, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    init_com_channels();
    etimer_set(&timer, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    /*Modify dataTxBroadCast...*/
    send_message_broadcast(&dataTxBroadCast);
    /*process all responses*/
    etimer_set(&timer, CLOCK_SECOND*5);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) || flagUniCastRx);

    if(!flagUniCastRx)
    {
        node.unicID = 0;
    }

    else
    {
        //node.unicID = takeMaxId();
    }

    printf("Node initialized with ID %d\n", node.unicID);

    SystemState = 1;
    PROCESS_END();
}