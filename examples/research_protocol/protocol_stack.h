#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "contiki.h"
#include "net/rime/rime.h" 
#include <stdint.h>
#include <stdio.h>

#define NrMaxBytes 32 /*Define max packet length*/
#define MAX_NDR_NODES 10

typedef enum 
{
    PROCESSER = 0,
    PRODUCER = 1,
    CONSUMER = 2
    
}nodeType;

typedef enum
{
    SEARCHING,
    RECEIVED,
    WAITINGBR,
    IDNEGOTIATION,
    WAITINGFORID

}discoveryState;


typedef struct NODE 
{
    nodeType type;
    uint8_t unicID;
    uint8_t metric;

}NODE;

typedef struct DATA 
{
    uint8_t id;      
    uint8_t roleCode;   
    uint8_t msgType; 
    uint8_t msgLen;
    uint8_t payload[NrMaxBytes];   

}DATA;


extern NODE node;
DATA dataTxBroadCast;
DATA dataTxUniCast;


void init_com_channels(void);
void send_message_unicast(linkaddr_t *dest, DATA *data);
void send_message_broadcast(DATA *data);

uint8_t pop_data_comUc_stack(uint8_t msgType, DATA* dataFound);
uint8_t pop_data_comBc_stack(uint8_t msgType, DATA* dataFound);
void protocol_reset_stack_BC();
void protocol_reset_stack_UC();

void protocol_print_status_node(void);
linkaddr_t protocol_get_min_target_metric(nodeType target_type);



#endif 