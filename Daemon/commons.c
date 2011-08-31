#include "commons.h"

static uint8_t msc_resp_send = 0;
static uint8_t msc_resp_received = 0;
static uint8_t credits_used = 0;
static uint8_t credits_free = 0;
uint8_t packet_processed = 0;

void process_sap_data(int cmd_num, int cmd_offset, uint8_t* packet)
{		
	
	switch (packet[cmd_num]) 
	{
		case BT_SAP_CONNECT_REQ:
		{
			changeState(iSAP_state_starting_sap);
			packet_processed++;
			
			uint8_t payload[12];
			uint8_t pos;
			createMessage(payload, BT_SAP_CONNECT_RESP);
			uint8_t data[1] = { 0x00 };
			pos = addParam(payload, BT_SAP_ConnectionStatus, 1, data, 4);										
			
			_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload, pos);
			
			uint8_t payload2[12];			
			createMessage(payload2, BT_SAP_STATUS_IND);
			uint8_t data2[1] = { 0x01 };
			pos = addParam(payload2, BT_SAP_StatusChange, 1, data2, 4);
			
			_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload2, pos);
			
			break;
		}
		case BT_SAP_TRANSFER_ATR_REQ:
		{
			packet_processed++;
			
			uint8_t payload[40];
			uint8_t pos;
			createMessage(payload, BT_SAP_TRANSFER_ATR_RESP);
			uint8_t data[1] = { 0x00 };
			pos = addParam(payload, BT_SAP_ResultCode, 1, data, 4);				
			pos = addParam(payload, BT_SAP_ATR, 22, ATR, pos);
			
			_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload, pos);
		
			changeState(iSAP_state_starting_working);
			break;
		}
		case BT_SAP_TRANSFER_APDU_REQ:
		{						
			//sleep(1);
			packet_processed++;
			
			unsigned char* apdu_resp = process_APDU_req(packet, cmd_offset);				
			
			if(strstr((char *)apdu_resp, "OK") != NULL)
			{
				uint8_t data2[1024];
				int resp_len = parse_APDU_resp(apdu_resp, data2);
				uint8_t payload[20 + resp_len/2];
				uint8_t pos;
				
				createMessage(payload, BT_SAP_TRANSFER_APDU_RESP);
				uint8_t data[1] = { 0x00 };
				pos = addParam(payload, BT_SAP_ResultCode, 1, data, 4);												
				
				pos = addParam(payload, BT_SAP_ResponseAPDU, (resp_len / 2), data2, pos);
				
				_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload, pos);												
			}
			break;
		}
		case BT_SAP_POWER_SIM_OFF_REQ: 
		{
			
			packet_processed++;				
			uint8_t payload[12];
			uint8_t pos;
			
			createMessage(payload, BT_SAP_POWER_SIM_OFF_RESP);
			uint8_t data[1] = { 0x00 };
			pos = addParam(payload, BT_SAP_ResultCode, 1, data, 4);						
			
			_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload, pos);
			break;
		}
		case BT_SAP_POWER_SIM_ON_REQ: 
		{
			
			packet_processed++;				
			uint8_t payload[12];
			uint8_t pos;
			
			createMessage(payload, BT_SAP_POWER_SIM_ON_RESP);
			uint8_t data[1] = { 0x00 };
			pos = addParam(payload, BT_SAP_ResultCode, 1, data, 4);
			
			_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload, pos);
			break;
		}
		case BT_SAP_DISCONNECT_REQ:
		{
			packet_processed++;
			
			uint8_t payload[12];
			createMessage(payload, BT_SAP_DISCONNECT_RESP);
			_bt_sap_send(source_cid, 0, RFCOMM_CHANNEL_ID, payload, 4);			
			
			break;
		}		
		default:
			break;
	}	
}

void process_rfcomm_init(uint8_t *packet)
{
	switch (packet[1]) {
		case BT_RFCOMM_SABM:
			
			changeState(iSAP_state_starting_rfcomm);
			packet_processed++;
			_bt_rfcomm_send_ua(source_cid, packet[0]);	
			break;
			
		case BT_RFCOMM_UIH:
			
			switch (packet[3]) {
				case BT_RFCOMM_PN_CMD:
					
					packet_processed++;
					
					uint8_t max_frame_size_hi = packet[8];
					uint8_t max_frame_size_low = packet[9];
					
					_bt_rfcomm_send_uih_pn_response(source_cid, 0, RFCOMM_CHANNEL_ID, max_frame_size_low, max_frame_size_hi);
					
					break;
				case BT_RFCOMM_MSC_CMD:
					
					packet_processed++;
					printf("Received BT_RFCOMM_MSC_CMD\n");
					printf("Responding...\n");
					// fine with this
					//uint8_t address = (1 << 0) | (0 << 1); // set response 
					//packet[3]  = BT_RFCOMM_MSC_CMD;  //  "      "
					//rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0x30, (uint8_t*)&packet[3], 4);
					_bt_rfcomm_send_uih_msc_rsp(source_cid, 0, RFCOMM_CHANNEL_ID, 0x0d);
					msc_resp_send = 1;
					
					break;
					
				case BT_RFCOMM_MSC_RSP:
					packet_processed++;
					
					msc_resp_received = 1;
					break;
				default:
					break;
			}
			break;
			
		default:
			break;
	}	
}

void process_rfcomm_data(uint8_t *packet)
{
	if(packet[1] == BT_RFCOMM_SABM)
	{
		_bt_rfcomm_send_ua(source_cid, packet[0]);
		_bt_rfcomm_send_uih_msc_cmd(source_cid, 0, RFCOMM_CHANNEL_ID, 0x8d);
	}
	
	if (packet[1] == BT_RFCOMM_UIH_PF || packet[1] == BT_RFCOMM_UIH) {
		int cmd_num = 3;
		int cmd_offset = 0;
		
		credits_used++;
		if(DEBUG){
			printf("RX: address %02x, control %02x: ", packet[0], packet[1]);
			//hexdump( (uint8_t*) &packet[3], size-4);
		}
		
		if (packet[1] == BT_RFCOMM_UIH_PF) {
			cmd_num = 4;
			cmd_offset = 1;
			
			if (!credits_free) {
				printf("Got %u credits, can send!\n", packet[3]);					
			}
			credits_free = packet[2];			
		}
		
		process_sap_data(cmd_num, cmd_offset, packet);
	}	
	
}
void process_rfcomm_packet(uint8_t *packet)
{

	switch (packet[0]) {
		case (3 | (0 << 3)):
			
			process_rfcomm_init(packet);			
			break;
		case ((1 << 0) | (1 << 1) | (0 << 2) | (RFCOMM_CHANNEL_ID << 3)):
		
			process_rfcomm_data(packet);
			break;

		default:
			break;
	}
																	
	uint8_t send_credits_packet = 0;
	
	if (credits_used >= CREDITS_CNT ) {
		send_credits_packet = 1;
		credits_used -= CREDITS_CNT;
	}
	
	if (msc_resp_send && msc_resp_received) {
		send_credits_packet = 1;
		msc_resp_send = msc_resp_received = 0;				
	}
	
	if (send_credits_packet) {
		uint8_t initiator = 0;
		uint8_t address = (1 << 0) | (initiator << 1) |  (initiator << 1) | (RFCOMM_CHANNEL_ID << 3); 
		rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH_PF, CREDITS_CNT, NULL, 0);
	}
	
	if (!packet_processed && DEBUG){
		printf("%02x: address %02x, control %02x: ", packet[3], packet[0], packet[1]);
		//hexdump( packet, size );
	}		
}

void process_hci_event(uint8_t* packet)
{
	uint16_t local_cid;
	//char pin[20];
	//int i;
	//CFDataRef cfData;

	switch (packet[0]) {
			
		case BTSTACK_EVENT_POWERON_FAILED:
			printf("HCI Init failed - make sure you have turned off Bluetooth in the System Settings\n");
			changeState(iSAP_state_error);
			break;
			
		case BTSTACK_EVENT_STATE:
			if (packet[2] == HCI_STATE_WORKING) {
				bt_send_cmd(&hci_write_local_name, device_name);
				changeState(iSAP_state_ready);
			}
			break;
			
		case L2CAP_EVENT_INCOMING_CONNECTION:					
			local_cid  = READ_BT_16(packet, 12); 
			
			// accept
			bt_send_cmd(&l2cap_accept_connection, local_cid);
			changeState(iSAP_state_starting_l2cap);
			break;
			
		case HCI_EVENT_LINK_KEY_NOTIFICATION:
			//implement link key saving
			
			break;
			
		case HCI_EVENT_LINK_KEY_REQUEST:
			// link key request
			bt_flip_addr(event_addr, &packet[2]); 
			
			//unsigned char lk[16] = {0x88, 0xf2, 0x5a, 0x92, 0x5a, 0x9e, 0x00, 0x4b, 0x05, 0xf9, 0xf7, 0x02, 0xd9, 0x1a, 0x43, 0xbb};
			//bt_send_cmd(&hci_link_key_request_reply, &event_addr, lk);
			bt_send_cmd(&hci_link_key_request_negative_reply, &event_addr);

			break;
			
		case HCI_EVENT_PIN_CODE_REQUEST:
			// inform about pin code request
			sendMessageToGUI(CFMSG_setPIN, 0, NULL, NULL, CFSTR(_messagePortName_client));
				
			//while (!allowPin) {
			//	sleep(1);
			//}
			//printf("Please enter PIN here: ");
			//fgets(pin, 20, stdin);
			//i = strlen(pin)-1;
			//if( pin[i] == '\n') { 
			//	pin[i] = '\0';
			//}
			//printf("PIN = '%s'\n", pin);
			bt_flip_addr(event_addr, &packet[2]); 
			//bt_send_cmd(&hci_pin_code_request_reply, &event_addr, strlen(pin), pin);
			break;
						
		case L2CAP_EVENT_CHANNEL_OPENED:
			// inform about new l2cap connection			
			source_cid = READ_BT_16(packet, 13);
			con_handle = READ_BT_16(packet, 9);

			break;
			
		case HCI_EVENT_CONNECTION_REQUEST: {
			// accept incoming connections
			bt_flip_addr(event_addr, &packet[2]); 
			bt_send_cmd(&hci_accept_connection_request, &event_addr, 1);
			changeState(iSAP_state_starting_hci);
			
			break;
		}
		case HCI_EVENT_CONNECTION_COMPLETE:
			// handle connections

			break;
			
		case HCI_EVENT_DISCONNECTION_COMPLETE:
			printf("Basebank connection closed\n");
			changeState(iSAP_state_ready);
			break;
			
		case HCI_EVENT_COMMAND_COMPLETE:
			// use pairing yes/no
			if ( COMMAND_COMPLETE_EVENT(packet, hci_write_local_name) ) {
				bt_send_cmd(&hci_write_authentication_enable, 0);
			}
			else if ( COMMAND_COMPLETE_EVENT(packet, hci_write_authentication_enable) ) {
				bt_send_cmd(&hci_write_class_of_device, 0x5a020C);
			}
						
			break;
		default:
			// other event
			if (DEBUG) {							
				printf("Unknown packet %02x\n", packet[0]);
			}
			break;
	}
}
