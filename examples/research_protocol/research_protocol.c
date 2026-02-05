#include "contiki.h"
#include "protocol.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>

#define UNICAST_CHANNEL 146 /*unic radio channel active*/

PROCESS(protocol_process, "Research Protocol Process");
AUTOSTART_PROCESSES(&protocol_process);

static struct unicast_conn uc; /*rime structure that handle the radio com*/


static void send_message(linkaddr_t *dest, DATA *data) 
{
    packetbuf_copyfrom(data, sizeof(DATA)); /*copy data in the stack radio*/
    unicast_send(&uc, dest);
    printf("[TX] Sent packet to Node %u\n", dest->u8[0]);
}


static void recv_uc(struct unicast_conn *c, const linkaddr_t *from) 
{
    /*
    * Callback funciton like ISR for the receiving interrupt message
    * via radio
    */

    DATA data;

    if(packetbuf_datalen() != sizeof(DATA)) 
    {
        printf("[ERR]Received invalid size\n");
        return;
    }

    memcpy(&data, packetbuf_dataptr(), sizeof(DATA));
    printf("[RX]Node %u received from Node %u (by Consumer: %u)\n", 
            node.unicID, from->u8[0], data.consumerID.u8[0]);


    /*Start routing logic in base of the nod type*/

    if (node.type == PRODUCER) 
    {
        printf("[Producer]:Finding best Processer...\n");
        /*find min metric processer*/
        linkaddr_t nextHop = protocol_get_min_target_metric(PROCESSER);
        /*update sender header message and send*/
        data.senderType = PRODUCER;
    
        if(nextHop.u8[0] != 0) 
        {
            send_message(&nextHop, &data);
        } 
        
        else 
        {
             printf("[ERR]No Processer found!\n");
        }
    }
    

    if (node.type == PROCESSER) 
    {
        printf("[PROCESSER]Processing done. Replying to Consumer...\n");
        data.senderType = PROCESSER;
        send_message(&data.consumerID, &data);
    }
   
    if (node.type == CONSUMER) 
    {
        if(linkaddr_cmp(&data.consumerID, &linkaddr_node_addr)) 
        {
            printf("[SUCCESS]Cycle Completed! Data returned to Node %u).\n", node.unicID);
        }
    }
}



static const struct unicast_callbacks unicast_callbacks = {recv_uc};

PROCESS_THREAD(protocol_process, ev, data)
{
  PROCESS_BEGIN();

  unicast_open(&uc, UNICAST_CHANNEL, &unicast_callbacks);

  uint8_t myID = linkaddr_node_addr.u8[0];

  switch(myID) 
  {
      case 1: 
      protocol_init_node_processer_1(); 
      break;

      case 2: 
      protocol_init_node_consumer_2(); 
      break;

      case 3: 
      protocol_init_node_consumer_3(); 
      break;

      case 4: 
      protocol_init_node_producer_4(); 
      break; 

      case 5: 
      protocol_init_node_processer_5(); 
      break;

      default: 
      protocol_init_node(); 
      break;
  }
  
  print_status_node();

  if(node.type == CONSUMER && node.unicID == 2) 
  {
      static struct etimer et;
      
      etimer_set(&et, CLOCK_SECOND * 5); 
      PROCESS_WAIT_UNTIL(etimer_expired(&et));
      printf("\n[Consumer] %u initiating request...\n", node.unicID);
      
      /*build the message*/
      DATA data;
      linkaddr_copy(&data.consumerID, &linkaddr_node_addr);
      data.senderType = CONSUMER;
      
      linkaddr_t dest = protocol_get_min_target_metric(PRODUCER);
      
      if(dest.u8[0] != 0) 
      {
        send_message(&dest, &data);
      } 
      else 
      {
        printf("[ERR] No Producer found in table!\n");
      }

  }

  while(1) 
  {
      PROCESS_WAIT_EVENT();
  }
  
  PROCESS_END();
}