#include"protocol_messages.h"

uint8_t NodesID[MAX_NDR_NODES] = {0}; /*store current ids network in listening mode*/
uint8_t indexNodesID = 0;

void message_discovery_searching()
{
    char payload[] = "MAC_ADDRESS_DISCOVERY";
    linkaddr_copy((unsigned char*)&dataTxBroadCast.id, &linkaddr_node_addr); /*test*/
    dataTxBroadCast.roleCode = 0x00; 
    dataTxBroadCast.msgType = msgType_BC_AUTODISCOVERY_START; /*discovery message*/
    dataTxBroadCast.msgLen = 3;
    memcpy(dataTxBroadCast.payload, payload, sizeof(payload));

    send_message_broadcast(&dataTxBroadCast);
}

void message_clear_buffer_id(void)
{
    memset(NodesID, 0, sizeof(NodesID));
}


uint8_t message_search_for_type_BC(uint8_t type)
{
    DATA tempData;

    if(pop_data_comBc_stack(type, &tempData) && indexNodesID<MAX_NDR_NODES)
    {
        uint8_t i = 0;

        for(i=0; i<indexNodesID; i++)
        {
            if(NodesID[i] == tempData.id)
            {
                /*case for duplicate message*/
                printf("[FILTER]Message filtered for duplicate id %u\n", tempData.id);
                return 2; /*just duplicates*/
            }
        }
        
        /*case for new message*/
        NodesID[indexNodesID++] = tempData.id;
        printf("[INFO]New node discovered with id %u\n", tempData.id);
        return 1; /*true*/
        
    }

    printf("[INFO]No message of type %u found in BC stack\n", type);
    return 0; /*false*/
}


uint8_t message_process_all_BC(uint8_t type)
{
    DATA tempData;
    uint8_t flagCopy = 0;
    uint8_t flagDataNew = 0;
    uint8_t flag = 0;


    flag = message_search_for_type_BC(type);

    if(flag == 1)
    {
        flagDataNew = 1; /*at least one new element*/
    }
    
    while(flag)
    {
        flag = message_search_for_type_BC(type);

        if(flag == 1)
        {
            flagDataNew = 1; /*at least one new element*/
        }
    }
    
    return flagDataNew; /*return 1 if at least one element is new and 0 for none or just duplicates*/
}

void log_print_nods_id()
{
    uint8_t i = 0;
    printf("[INFO]<<NOD %i>> discovered nodes: ", node.unicID);
    for(i=0; i<indexNodesID; i++)
    {
        printf("%u ", NodesID[i]);
    }
    printf(" Total: %i\n", indexNodesID);
}

uint8_t message_search_for_type_UC(uint8_t type)
{
    DATA tempData;
    if(pop_data_comUc_stack(type, &tempData) && indexNodesID<MAX_NDR_NODES)
    {
        printf("[INFO]Unicast message received from node with id %u\n", tempData.id);
        return tempData.id; /*return the id of the unicast node message*/
    }

    return 255;
}
