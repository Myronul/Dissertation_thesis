#include "protocol.h"
#define UNICAST_CHANNEL 146 /*unic radio channel active*/
#define BROADCAST_CHANNEL 146 

NODE node; /*defined as extern in the header*/
DATA dataRxBroadCast;
DATA dataRxUniCast;
DATA dataQeueBc[255];
static uint8_t headBc = 0;
static uint8_t tailBc = 0;
static uint8_t countBc = 0;
static uint8_t indexBc = 0;
DATA dataQeueUc[255];
static uint8_t indexUc = 0;
uint8_t flagUniCastRx = 0;
uint8_t flagBroadCastRx = 0;

static struct unicast_conn uc;    /*rime structure that handle the radio unicast com*/
static struct broadcast_conn bc;  /*rime structure that handle the radio broadcast com*/
static const struct unicast_callbacks unicastCallback = {__unicast_ISR__};
static const struct broadcast_callbacks broadcastCallback = {__broadcast_ISR__};



static void push_qeue_Bc(DATA dataRx)
{
    if(indexBc < 255)
    {
        dataQeueBc[tailBc] = dataRx;
        tailBc = (tailBc+1)%255;
        indexBc++;
    }
}

static DATA pop_qeue_Bc(void)
{
    DATA first = dataQeueBc[headBc];
    headBc = (headBc+1)%255;
    indexBc--;

    return first;
    
}


static void push_qeue_Uc(DATA dataRx)
{
    dataQeueUc[indexUc++] = dataRx;
}

static DATA pop_qeue_Uc(void)
{
    return dataQeueUc[indexUc--];
}


static void __unicast_ISR__(struct unicast_conn *c, const linkaddr_t *from) 
{
    /*ISR for the unicast receiving data*/
    memcpy(&dataRxUniCast, packetbuf_dataptr(), sizeof(DATA));
    push_qeue_Uc(dataRxUniCast);

}


static void __broadcast_ISR__(struct broadcast_conn *c, const linkaddr_t *from) 
{
    /*ISR for the unicast receiving data*/
    memcpy(&dataRxBroadCast, packetbuf_dataptr(), sizeof(DATA));
    push_qeue_Bc(dataRxUniCast);
}


void init_com_channels(void)
{
    unicast_open(&uc, UNICAST_CHANNEL, &unicastCallback);
    broadcast_open(&bc, BROADCAST_CHANNEL, &broadcastCallback);
}


void send_message_unicast(linkaddr_t *dest, DATA *data) 
{
    packetbuf_copyfrom(data, sizeof(DATA)); /*copy data in the stack radio*/
    unicast_send(&uc, dest);
    printf("[TX] Sent packet to Node %u\n", dest->u8[0]);
}


void send_message_broadcast(DATA *data)
{
    packetbuf_copyfrom(data, sizeof(DATA));
    broadcast_send(&bc);
    printf("[TX] Sent packet Broadcast %u\n");
}


void protocol_print_status_node(void) 
{
    char *roleStr;

    switch(node.type) 
    {
        case PROCESSER: 
        roleStr = "PROCESSER"; 
        break;

        case PRODUCER: 
        roleStr = "PRODUCER";
        break;

        case CONSUMER: 
        roleStr = "CONSUMER"; 
        break;

        default: roleStr = "UNKNOWN";
    }

    printf("[STATUS]Info -> Node %u, Role: %s, Metric: %u\n", node.unicID, roleStr, node.metric);

}



linkaddr_t protocol_get_min_target_metric(nodeType targetType) 
{
    linkaddr_t addr;
    addr.u8[0] = 0; /*default values*/
    addr.u8[1] = 0;
    
    return addr;
}