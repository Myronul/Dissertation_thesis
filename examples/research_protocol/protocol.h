#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "contiki.h"
#include "net/rime/rime.h" // Necesar pentru linkaddr_t
#include <stdint.h>
#include <stdio.h>

// Definim tipurile de noduri
typedef enum {
    PROCESSER = 0,
    PRODUCER = 1,
    CONSUMER = 2
} NodeType_t;

// Structura nodului local
typedef struct NODE {
    NodeType_t type;
    uint8_t unicID;
    uint8_t metric;
} NODE;

// Structura pachetului care zboara prin retea
typedef struct {
    linkaddr_t original_consumer; // Cine a inceput cererea (pentru retur)
    uint8_t sender_type;          // Cine trimite acum
    uint8_t payload[10];          // Date utile (redus pentru demo)
} protocol_msg_t;

// Variabila globala pentru nodul curent
extern NODE node;

// Functii
void protocol_init_node(void);
void print_status_node(void);

// Functie care gaseste urmatorul hop (hardcodat)
linkaddr_t get_best_target_address(NodeType_t target_type);

// Functii de initializare specifice
void protocol_init_node_processer_1(void);
void protocol_init_node_consumer_2(void);
void protocol_init_node_consumer_3(void);
void protocol_init_node_producer_4(void);
void protocol_init_node_processer_5(void);

#endif /* PROTOCOL_H */