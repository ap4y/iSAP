#include "sap_util.h"

void sap_init()
{
	int ln = 0;
	ATR[ln++] = 0x3b;						// reserved
	ATR[ln++] = 0xff;						// reserved				
	ATR[ln++] = 0x94;						// reserved
	ATR[ln++] = 0x00;						// reserved
	ATR[ln++] = 0x00;						// reserved				
	ATR[ln++] = 0x40;						// reserved	
	ATR[ln++] = 0x0a;						// reserved
	ATR[ln++] = 0x80;						// reserved				
	ATR[ln++] = 0x31;						// reserved	
	ATR[ln++] = 0x00;						// reserved
	ATR[ln++] = 0x73;						// reserved				
	ATR[ln++] = 0x12;						// reserved	
	ATR[ln++] = 0x21;						// reserved
	ATR[ln++] = 0x13;						// reserved				
	ATR[ln++] = 0x57;						// reserved	
	ATR[ln++] = 0x4a;						// reserved
	ATR[ln++] = 0x33;						// reserved				
	ATR[ln++] = 0x0e;						// reserved	
	ATR[ln++] = 0x02;						// reserved
	ATR[ln++] = 0x31;						// reserved				
	ATR[ln++] = 0x41;						// reserved
		
	bt_send_cmd(&l2cap_register_service, 0x03, 250);
	
	uint8_t service[200];
	create_sap_service(service);
	de_dump_data_element(service);
	
	bt_send_cmd(&sdp_register_service_record, service);
	
	fd = InitConn(115200);	
}

void sap_close()
{
	CloseConn(fd);	
}

void createMessage(uint8_t *payload, uint8_t type)
{
	int pos = 0;
	payload[pos++] = type;						// MsgId
	payload[pos++] = 0x00;						// ParamCnt
	payload[pos++] = 0x00;						// reserved
	payload[pos++] = 0x00;						// reserved	
}

int addParam(uint8_t* payload, uint8_t paramType, uint8_t paramLen, uint8_t* paramData, int pos)
{	
	payload[1]++;
	
	payload[pos++] = paramType;					// Parameter Id
	payload[pos++] = 0x00;						// reserved
	
	payload[pos++] = paramLen >> 8;				// Param length hi
	payload[pos++] = paramLen & 0xff;			// Param length low
	
	memcpy(&payload[pos], paramData, paramLen);	// Param value low
	pos += paramLen;					
	
	uint8_t reminder = paramLen % 4;
    if (reminder > 0)
	{
		int i;
		for (i = 0; i < 4 - reminder; i++) {
			payload[pos++] = 0x00;				// reserved
		}
	}
	
	return pos;
}

void _bt_sap_send(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t *data, uint16_t len) {
	uint8_t address = (1 << 0) | (initiator << 1) |  (initiator << 1) | (channel << 3); 
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0x1, data, len);
}

unsigned char* process_APDU_req(uint8_t* packet, int cmd_offset)
{
	union { 
		uint16_t word; 
		uint8_t bytes[2]; 
	} len; 
	
	len.bytes[0] = packet[10 + cmd_offset]; 
	len.bytes[1] = packet[9 + cmd_offset];
	
	char cmd[1024];				
	sprintf(cmd,"AT+CSIM=%u,\"",len.word*2);
	int i;
	int start = 11 + cmd_offset; 
	for (i = start ; i < start + len.word; i++) {
		sprintf(cmd, "%s%02x", cmd, packet[i]);
	}				
	sprintf(cmd,"%s\"\r",cmd);
	SendStrCmd(fd,cmd);
	return ReadResp(fd);						
}

int parse_APDU_resp(unsigned char* apdu_resp, uint8_t* data)
{
	char* resp_str = strstr((char*)apdu_resp, "+CSIM:");				
	int resp_len;
	sscanf(&resp_str[7], "%d", &resp_len);	
		
	int str_data = 10;
	if (resp_len > 9) {
		str_data = 11;
	}
	int i;
	int j = 0;
	for (i = str_data ; i < str_data + resp_len; i=i+2) {
		unsigned int num;
		sscanf(&resp_str[i], "%2x", &num);
		
		data[j] = num;
		j++;
	}	
	return resp_len;
}