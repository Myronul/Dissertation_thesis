#include "protocol.h"

NODE node; /*defined as extern in the header*/
static uint8_t tab[3][5];


static inline protocol_init_topology(void)
{

    /*
    * heap map (priority qeue) for each groupe of
    * nodes. Priority -> nodeID
    */

    /*PROCESSER*/
    tab[0][0] = 1;
    tab[0][1] = 5;
    tab[0][2] = 0;
    tab[0][3] = 0;
    tab[0][4] = 0;
    /*PRODUCER*/
    tab[1][0] = 4;
    tab[1][1] = 0;
    tab[1][2] = 0;
    tab[1][3] = 0;
    tab[1][4] = 0;
    /*CONSUMER*/
    tab[2][0] = 3;
    tab[2][1] = 2;
    tab[2][2] = 0;
    tab[2][3] = 0;
    tab[2][4] = 0;

}

linkaddr_t protocol_get_min_target_metric(nodeType targetType) 
{
    linkaddr_t addr;
    addr.u8[0] = 0; /*default values*/
    addr.u8[1] = 0;

    if (targetType == PRODUCER) 
    {
        addr.u8[0] = (unsigned char)tab[1][0]; 
    }

    if (targetType == PROCESSER) 
    {
        addr.u8[0] = (unsigned char)tab[0][0]; 
    }
    
    return addr;
}

void protocol_init_node(void) 
{
    /*default generic node*/
    protocol_init_topology();
    node.type = CONSUMER;
    node.unicID = linkaddr_node_addr.u8[0];
    node.metric = 0;
}


void protocol_init_node_processer_1(void) 
{
    protocol_init_topology();
    node.type = PROCESSER;
    node.unicID = 1;
    node.metric = 4;
}


void protocol_init_node_consumer_2(void) 
{
    protocol_init_topology();
    node.type = CONSUMER;
    node.unicID = 2;
    node.metric = 4;
}


void protocol_init_node_consumer_3(void) 
{
    protocol_init_topology();
    node.type = CONSUMER;
    node.unicID = 3;
    node.metric = 1;
}


void protocol_init_node_producer_4(void) 
{
    protocol_init_topology();
    node.type = PRODUCER;
    node.unicID = 4; 
    node.metric = 9;
}


void protocol_init_node_processer_5(void) 
{
    protocol_init_topology();
    node.type = PROCESSER;
    node.unicID = 5;
    node.metric = 8;
}


void print_status_node(void) 
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

    printf("[INIT]Info -> Node %u, Role: %s, Metric: %u\n", node.unicID, roleStr, node.metric);

}