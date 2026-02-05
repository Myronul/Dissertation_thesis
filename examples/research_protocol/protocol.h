#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "contiki.h"
#include "net/rime/rime.h" 
#include <stdint.h>
#include <stdio.h>


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


typedef struct 
{
    linkaddr_t consumerID; 
    uint8_t senderType;          
    uint8_t payload[10];      

}DATA;


extern NODE node; /*current node defined*/


void protocol_init_node(void);
void print_status_node(void);
linkaddr_t protocol_get_min_target_metric(nodeType target_type);

/*simulation only function prototypes*/
 
void protocol_init_node_processer_1(void);
void protocol_init_node_consumer_2(void);
void protocol_init_node_consumer_3(void);
void protocol_init_node_producer_4(void);
void protocol_init_node_processer_5(void);



#endif 