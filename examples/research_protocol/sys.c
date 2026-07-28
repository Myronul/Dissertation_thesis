#include "sys.h"

nodeType hostNodeType;

void sys_init_random_node_type(void)
{
    hostNodeType = (nodeType)(random_rand()%3); /*randomly assign a node type*/
    printf("[INFO]Node %u assigned type: %u\n", node.id, hostNodeType);
}