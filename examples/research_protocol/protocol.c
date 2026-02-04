#include "protocol.h"

NODE node;

/* * Tabela simplificata pentru simulare.
 * Mapam: Tip Nod -> [ID_Nod, Metrica]
 * Pentru simplitate, vom hardcoda logica de alegere in get_best_target_address
 */

void protocol_init_node(void) {
    // Default generic
    node.type = CONSUMER;
    node.unicID = linkaddr_node_addr.u8[0];
    node.metric = 0;
}

/* * Aceasta functie este "creierul" rutarii hardcodate.
 * In functie de ce cautam (Producer, Processer), returneaza ID-ul (adresa Rime)
 * a celui mai bun candidat definit de tine.
 */
linkaddr_t get_best_target_address(NodeType_t target_type) {
    linkaddr_t addr;
    addr.u8[0] = 0; // Default invalid
    addr.u8[1] = 0;

    if (target_type == PRODUCER) {
        // Cautam cel mai bun Producer. 
        // In scenariul tau: Nodul 4 este Producer cu metrica 9.
        // Daca am avea mai multi, aici am face if (metric1 < metric2)...
        addr.u8[0] = 4; 
    }
    else if (target_type == PROCESSER) {
        // Cautam cel mai bun Processer.
        // Avem Processer 1 (metric 4) si Processer 5 (metric 8).
        // Alegem 1 pentru ca are metrica mai mica.
        addr.u8[0] = 1; 
    }
    
    return addr;
}

/* Initializari specifice bazate pe ID-ul din Cooja */

void protocol_init_node_processer_1(void) {
    node.type = PROCESSER;
    node.unicID = 1;
    node.metric = 4;
}

void protocol_init_node_consumer_2(void) {
    node.type = CONSUMER;
    node.unicID = 2;
    node.metric = 4;
}

void protocol_init_node_consumer_3(void) {
    node.type = CONSUMER;
    node.unicID = 3;
    node.metric = 1;
}

void protocol_init_node_producer_4(void) {
    node.type = PRODUCER;
    node.unicID = 4; // Atentie: ID-ul trebuie sa bata cu cel din Cooja
    node.metric = 9;
}

void protocol_init_node_processer_5(void) {
    node.type = PROCESSER;
    node.unicID = 5;
    node.metric = 8;
}

void print_status_node(void) {
    char *role_str;
    switch(node.type) {
        case PROCESSER: role_str = "PROCESSER"; break;
        case PRODUCER: role_str = "PRODUCER"; break;
        case CONSUMER: role_str = "CONSUMER"; break;
        default: role_str = "UNKNOWN";
    }
    printf("[INIT] I am Node %u, Role: %s, Metric: %u\n", 
           node.unicID, role_str, node.metric);
}