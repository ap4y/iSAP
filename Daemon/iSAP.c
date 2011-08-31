#include "sap_util.h"
#include "commons.h"
#include "CF_util.h"

timer_source_t timeout;

void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
			
	switch (packet_type) {
			
		case L2CAP_DATA_PACKET:			
			
			process_rfcomm_packet(packet);
			break;
			
		case HCI_EVENT_PACKET:
			
			process_hci_event(packet);
			break;
		default:
			if (DEBUG) {
				printf("Unknown packet type %02x\n", packet_type);
			}			
			break;
	}
}

void run_loop_to_GUI() 
{	
	CFDataRef cfData;
	int result = sendMessageToGUI(CFMSG_nop, 1, &curState, &cfData, CFSTR(_messagePortName_client));
	if (result == 0){
		const uint8_t *data = CFDataGetBytePtr(cfData);
		UInt16 dataLen = CFDataGetLength(cfData);
		if (dataLen)
		{
			switch (data[0]) {
				case CFMSG_setMode:{
					if (data[1]) {
						//fprintf(stderr, "get HCI_POWER_ON\n");
						bt_send_cmd(&btstack_set_power_mode, HCI_POWER_ON );
						changeState(iSAP_state_starting);
					} else {
						//fprintf(stderr, "get HCI_POWER_OFF\n");
						if (con_handle) {
							bt_send_cmd(&hci_disconnect, con_handle, 0x03);
						}
						bt_send_cmd(&btstack_set_power_mode, HCI_POWER_OFF );
						changeState(iSAP_state_off);
					}					
					break;
				}
				case CFMSG_setPIN_Resp:{
					memcpy(pin, &data[1], 16);
					fprintf(stderr, "PIN recieved! %s. data %s\n", pin, data);
					
					bt_send_cmd(&hci_pin_code_request_reply, &event_addr, strlen(pin), pin);
					
					break;
				}
				default:
					break;
			}					
		}
	}			
	else{
		//fprintf(stderr, "Error while sending %d\n", result);
	}
	
	CFRelease(cfData);
	run_loop_set_timer(&timeout, 10000);
	run_loop_add_timer(&timeout);
}

int main (int argc, const char * argv[]){		
	run_loop_init(RUN_LOOP_POSIX);
	int err = bt_open();
	
	if (err) {
		printf("Failed to open connection to BTdaemon\n");
		return(err);
	}
	
	bt_register_packet_handler(packet_handler);	
	sap_init();									
	
	device_name = "iSAP";			
	changeState(iSAP_state_off);

	timeout.process = run_loop_to_GUI;
	run_loop_set_timer(&timeout, 1000);
	run_loop_add_timer(&timeout);
		
	run_loop_execute();	
	
	bt_close();
	sap_close();	
}