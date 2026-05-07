#include "protocol_stack.h"
#include"protocol_messages.h"
#define UNICAST_CHANNEL 129 /*unic radio channel active*/
#define BROADCAST_CHANNEL 146 
#define NR_DATA_BUFFER 8 /*number of messages that can be stored in the buffer for each type of com*/

#define BROADCAST_PUSH 1
#define UNICAST_PUSH 0 

extern uint8_t SystemState;
extern uint8_t NodesID[MAX_NDR_NODES]; 
extern uint8_t indexNodesID;

NODE node; /*defined as extern in the header*/
DATA dataRxBroadCast;
DATA dataRxUniCast;

static DATA dataQeueBc[NR_DATA_BUFFER];
static DATA dataQeueUc[NR_DATA_BUFFER];

static uint8_t bitMapBc = 0x00;
static uint8_t bitMapUc = 0x00;

static uint8_t flagUniCastRx = 0;
static uint8_t flagBroadCastRx = 0;

static struct unicast_conn uc;    /*rime structure that handle the radio unicast com*/
static struct broadcast_conn bc;  /*rime structure that handle the radio broadcast com*/


static void push_data_com_stack(DATA dataRx, DATA* buffer, uint8_t* map, uint8_t flagMap)
{
    
    /*search if broadcast for duplicates*/

    uint8_t index = 0;

    if(flagMap == 1) /*case for broadcast*/
    {
        /*check if it is already in the buffer*/
        while(index < NR_DATA_BUFFER)
        {
            if((buffer[index].id == dataRx.id && buffer[index].msgType == dataRx.msgType) && ((*map) & (1<<index)))
            {
                printf("[INFO]Duplicate already in buffer filtered at ISR for BR from nod: %i\n", dataRx.id);
                return;
            }

            index++;
        }         

        for(index = 0; index < indexNodesID; index++)
        {
            if(NodesID[index] == dataRx.id)
            {
                printf("[INFO]Duplicate already registered filtered at ISR for BR from nod: %i\n", dataRx.id);
                return;
            }
        }

    }
  

    /*filter by type*/
    switch(SystemState)
    {
        case state_AUTODISCOVERY:
        if((dataRx.msgType != msgType_BC_AUTODISCOVERY_START) && (dataRx.msgType != msgType_UC_JOIN_EXISTING_NETWORK))
        {
            printf("[FILTER]Message filtered in state AUTODISCOVERY\n");
            return; /*for this state we will accept only a certain types of msges to not fill fast the buffer*/
        }
        break;

        case state_LISTEN:
        if((dataRx.msgType != msgType_BC_AUTODISCOVERY_START) && (dataRx.msgType != msgType_UC_JOIN_EXISTING_NETWORK))
        {
            printf("[FILTER]Message filtered in state LISTEN\n");
            return; /*for this state we will accept only a certain types of msges to not fill fast the buffer*/
        }
        break;

        case state_ELECT_LIDER_TEMP:
            return;
        break;
        
        default:
        break;  
    }

    /*add to the qeue bitMap implementation and message filter*/

    index = 0;

    while(index < NR_DATA_BUFFER)
    {
        if(!((*map) & (1<<index)))
        {
            buffer[index] = dataRx;
            *map = (*map) | (1<<index);
            printf("[INFO]Message stored in the buffer at index %u\n", index);
            return;
        }

        index++;
    }

    printf("[INFO]Qeue BC is full!\n");

}

void protocol_reset_stack_BC()
{
    bitMapBc = 0;
}

void protocol_reset_stack_UC()
{
    bitMapUc = 0;
}

uint8_t pop_data_comBc_stack(uint8_t msgType, DATA* dataFound)
{
    /*Function to search through all messages received in the buffer
    In base of the message ID it will pop the message fom the current index
    and will reset the bitMap. 
    flagMap = 1 -> broadcast
    flagMap = 0 -> unicast*/

    /*bitMap implementation*/
    uint8_t index = 0;

    while(index < NR_DATA_BUFFER)
    {
        if(((bitMapBc) & (1<<index)) && (dataQeueBc[index].msgType == msgType))
        {
            bitMapBc = (bitMapBc) & ~(1<<index); /*reset index*/
            *dataFound = dataQeueBc[index];
            return 1;
        }

        index++;
    }   

    return 0;

}


uint8_t pop_data_comUc_stack(uint8_t msgType, DATA* dataFound)
{
    /*Function to search through all messages received in the buffer
    In base of the message ID it will pop the message fom the current index
    and will reset the bitMap. 
    flagMap = 1 -> broadcast
    flagMap = 0 -> unicast*/

    /*bitMap implementation*/
    uint8_t index = 0;

    while(index < NR_DATA_BUFFER)
    {
        if(((bitMapUc) & (1<<index)) && (dataQeueUc[index].msgType == msgType))
        {
            bitMapUc = (bitMapUc) & ~(1<<index);
            *dataFound = dataQeueUc[index];
            return 1;
        }

        index++;
    }   

    return 0;

}


static void __unicast_ISR__(struct unicast_conn *c, const linkaddr_t *from) 
{
    /*ISR for the unicast receiving data*/
    flagUniCastRx = 1;
    memcpy(&dataRxUniCast, packetbuf_dataptr(), sizeof(DATA));
    printf("[RX-UC]Node %u received from Node %u\n", 
            node.unicID, from->u8[0]);
    push_data_com_stack(dataRxUniCast, dataQeueUc, &bitMapUc, UNICAST_PUSH);

}


static void __broadcast_ISR__(struct broadcast_conn *c, const linkaddr_t *from) 
{
    /*ISR for the broadcast receiving data*/
    if(packetbuf_datalen() != sizeof(DATA)) 
    {
        printf("[ERR]Received invalid size\n");
        return;
    }

    flagBroadCastRx = 1;
    memcpy(&dataRxBroadCast, packetbuf_dataptr(), sizeof(DATA));
    printf("[RX-BC]Node %u received from Node %u\n", 
            node.unicID, from->u8[0]);
    push_data_com_stack(dataRxBroadCast, dataQeueBc, &bitMapBc, BROADCAST_PUSH);
}

static const struct unicast_callbacks unicastCallback = {__unicast_ISR__};
static const struct broadcast_callbacks broadcastCallback = {__broadcast_ISR__};


void init_com_channels(void)
{
    node.unicID = linkaddr_node_addr.u8[0];
    printf("[INIT]Initializing communication channels\n");
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
    printf("[TX] Sent packet Broadcast\n");
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