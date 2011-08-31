#include "rfcomm.h"
#include "sap_util.h"
#include <stdio.h>
#include "CF_util.h"

#define CREDITS_CNT		0x30
#define DEBUG			1

uint16_t source_cid;
hci_con_handle_t con_handle;
bd_addr_t event_addr;
char* device_name;
char pin[20];

void process_rfcomm_packet(uint8_t *packet);
void process_hci_event(uint8_t* packet);