#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "contiki.h"
#include "net/rime/rime.h" 
#include <stdint.h>
#include <stdio.h>

#define N 256 /*Define max packet length*/

typedef enum 
{
    PROCESSER = 0,
    PRODUCER = 1,
    CONSUMER = 2
    
}nodeType;


typedef struct NODE 
{
    nodeType type;
    uint8_t unicID;
    uint8_t metric;

}NODE;

typedef struct DATA 
{
    int8_t id;      
    uint8_t roleCode;   
    uint8_t msgType; 
    uint8_t msgLen;
    uint8_t payload[N];   

}DATA;


extern NODE node; /*current node defined*/
extern DATA dataRxBroadCast;
extern DATA dataRxUniCast;
extern uint8_t flagUniCastRx;
extern uint8_t flagBroadCastRx;
DATA dataTxBroadCast;
DATA dataTxUniCast;


void init_com_channels(void);
void send_message_unicast(linkaddr_t *dest, DATA *data);
void send_message_broadcast(DATA *data);

void protocol_print_status_node(void);
linkaddr_t protocol_get_min_target_metric(nodeType target_type);



#endif 