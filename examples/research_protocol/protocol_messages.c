#include"protocol_messages.h"
static uint8_t NodesID[MAX_NDR_NODES] = {0}; /*store current ids network in listening mode*/
static uint8_t indexNodesID = 0;

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
        if(NodesID[indexNodesID] == tempData.id)
        {
            /*case for duplicate message*/
            return 0;
        }

        else
        {
            /*case for new message*/
            NodesID[indexNodesID++] = tempData.id;
            return 1; /*true*/
        }
    }

    return 0; /*false*/
}


uint8_t message_search_for_type_UC(uint8_t type)
{
    DATA tempData;
    if(pop_data_comUc_stack(type, &tempData) && indexNodesID<MAX_NDR_NODES)
    {
        return tempData.id; /*return the id of the unicast node message*/
    }

    return 255;
}
