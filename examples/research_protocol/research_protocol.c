#include "contiki.h"
#include "protocol_stack.h"
#include "net/rime/rime.h"
#include "lib/random.h"
#include <stdio.h>
#include <string.h>

static uint8_t SystemState = 0; /*global state for the entire system*/


static void discovery_send_message_searching()
{
    linkaddr_copy((unsigned char*)&dataTxBroadCast.id, &linkaddr_node_addr); /*test*/
    dataTxBroadCast.msgLen = 3;
    dataTxBroadCast.msgType = 0x01;
    dataTxBroadCast.roleCode = 0x00;
    memcpy(dataTxBroadCast.payload, "Broadcast message", 17);

    send_message_broadcast(&dataTxBroadCast);
}

PROCESS(process_init_node, "Protocol Init Node");
AUTOSTART_PROCESSES(&process_init_node);

PROCESS_THREAD(process_init_node, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    init_com_channels();
    random_init(linkaddr_node_addr.u8[0] ^ (clock_time() & 0xFF));

    while(1)
    {
        if(SystemState == 0)
        {
            int jitter = 1 + random_rand()%1000;
            clock_time_t t = (CLOCK_SECOND * jitter) / 1000;
            printf("wait...%lu\n", t);
            if(t == 0) t = 1;
            etimer_set(&timer, t);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
            discovery_send_message_searching();
        }

        if(SystemState == 1)
        {

        }

    }

    PROCESS_END();
}