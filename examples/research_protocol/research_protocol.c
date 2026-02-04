#include "contiki.h"
#include "protocol.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>

/* Canalul Rime pentru comunicare - asigura-te ca e unic */
#define UNICAST_CHANNEL 146

PROCESS(protocol_process, "Research Protocol Process");
AUTOSTART_PROCESSES(&protocol_process);

static struct unicast_conn uc;

/* Functie helper pentru a trimite un mesaj */
static void send_message(linkaddr_t *dest, protocol_msg_t *msg) {
    packetbuf_copyfrom(msg, sizeof(protocol_msg_t));
    unicast_send(&uc, dest);
    printf("[TX] Sent packet to Node %u\n", dest->u8[0]);
}

/* Callback cand primim un mesaj */
static void recv_uc(struct unicast_conn *c, const linkaddr_t *from) {
    protocol_msg_t msg;
    
    // Verificam dimensiunea
    if(packetbuf_datalen() != sizeof(protocol_msg_t)) {
        printf("[ERR] Received invalid size\n");
        return;
    }
    // Copiem datele din bufferul radio in structura noastra
    memcpy(&msg, packetbuf_dataptr(), sizeof(protocol_msg_t));

    printf("[RX] Node %u received from Node %u (Org Consumer: %u)\n", 
           node.unicID, from->u8[0], msg.original_consumer.u8[0]);

    /* LOGICA DE RUTARE */
    
    // Cazul 1: Sunt PRODUCER (Am primit de la Consumer, trimit la Processer)
    if (node.type == PRODUCER) {
        printf("[LOGIC] I am Producer. Finding best Processer...\n");
        
        // 1. Gasim cel mai bun Processer
        linkaddr_t next_hop = get_best_target_address(PROCESSER);
        
        // 2. Actualizam sender type si forwardam
        msg.sender_type = PRODUCER;
        
        if(next_hop.u8[0] != 0) {
            send_message(&next_hop, &msg);
        } else {
             printf("[ERR] No Processer found!\n");
        }
    }
    // Cazul 2: Sunt PROCESSER (Am primit de la Producer, trimit inapoi la Consumer)
    else if (node.type == PROCESSER) {
        printf("[LOGIC] I am Processer. Processing done. Replying to Consumer...\n");
        
        // Trimitem direct inapoi la cel care a initiat (stocat in structura)
        msg.sender_type = PROCESSER;
        
        send_message(&msg.original_consumer, &msg);
    }
    // Cazul 3: Sunt CONSUMER (S-a intors mesajul la mine)
    else if (node.type == CONSUMER) {
        // Verificam daca eu am initiat cererea
        if(linkaddr_cmp(&msg.original_consumer, &linkaddr_node_addr)) {
            printf("[SUCCESS] Cycle Completed! Data returned to me (Node %u).\n", node.unicID);
        }
    }
}

static const struct unicast_callbacks unicast_callbacks = {recv_uc};

PROCESS_THREAD(protocol_process, ev, data)
{
  PROCESS_BEGIN();

  // 1. Deschidem conexiunea radio
  unicast_open(&uc, UNICAST_CHANNEL, &unicast_callbacks);

  // 2. Initializam rolul in functie de ID-ul nodului
  uint8_t my_id = linkaddr_node_addr.u8[0];

  switch(my_id) {
      case 1: protocol_init_node_processer_1(); break;
      case 2: protocol_init_node_consumer_2(); break;
      case 3: protocol_init_node_consumer_3(); break;
      case 4: protocol_init_node_producer_4(); break; 
      case 5: protocol_init_node_processer_5(); break;
      default: protocol_init_node(); break;
  }
  
  print_status_node();

  // 3. Logica de start pentru CONSUMER (Nodul 2)
  if(node.type == CONSUMER && node.unicID == 2) {
      static struct etimer et;
      
      // Asteptam 5 secunde sa porneasca toate nodurile in simulare
      etimer_set(&et, CLOCK_SECOND * 5); 
      
      // AICI ERA EROAREA: Folosim PROCESS_WAIT_UNTIL
      PROCESS_WAIT_UNTIL(etimer_expired(&et));
      
      printf("\n[START] Consumer %u initiating request...\n", node.unicID);
      
      // Construim mesajul
      protocol_msg_t msg;
      linkaddr_copy(&msg.original_consumer, &linkaddr_node_addr);
      msg.sender_type = CONSUMER;
      
      // Cautam cel mai bun Producer (in tabela hardcodata)
      linkaddr_t dest = get_best_target_address(PRODUCER);
      
      if(dest.u8[0] != 0) {
          send_message(&dest, &msg);
      } else {
          printf("[ERR] No Producer found in table!\n");
      }
  }

  while(1) {
      PROCESS_WAIT_EVENT();
  }
  
  PROCESS_END();
}