#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "rfcomm.h"
#include "at_utils.h"
#include "commons.h"
#include "sdp_utils.h"

// SAP Messages
#define BT_SAP_CONNECT_REQ							0x00 
#define BT_SAP_CONNECT_RESP							0x01 
#define BT_SAP_DISCONNECT_REQ						0x02 
#define BT_SAP_DISCONNECT_RESP						0x03 
#define BT_SAP_DISCONNECT_IND						0x04 
#define BT_SAP_TRANSFER_APDU_REQ					0x05 
#define BT_SAP_TRANSFER_APDU_RESP					0x06 
#define BT_SAP_TRANSFER_ATR_REQ						0x07 
#define BT_SAP_TRANSFER_ATR_RESP					0x08
#define BT_SAP_POWER_SIM_OFF_REQ					0x09 
#define BT_SAP_POWER_SIM_OFF_RESP					0x0A 
#define BT_SAP_POWER_SIM_ON_REQ						0x0B 
#define BT_SAP_POWER_SIM_ON_RESP					0x0C 
#define BT_SAP_RESET_SIM_REQ						0x0D 
#define BT_SAP_RESET_SIM_RESP						0x0E 
#define BT_SAP_TRANSFER_CARD_READER_STATUS_REQ      0x0F 
#define BT_SAP_TRANSFER_CARD_READER_STATUS_RESP		0x10 
#define BT_SAP_STATUS_IND							0x11 
#define BT_SAP_ERROR_RESP							0x12 
#define BT_SAP_SET_TRANSPORT_PROTOCOL_REQ			0x13 
#define BT_SAP_TRANSPORT_PROTOCOL_RESP				0x14
	
// SAP Params
#define BT_SAP_MaxMsgSize 							0x00 
#define BT_SAP_ConnectionStatus 					0x01 
#define BT_SAP_ResultCode 							0x02 
#define BT_SAP_DisconnectionType					0x03 
#define BT_SAP_CommandAPDU 							0x04 
#define BT_SAP_CommandAPDU7816 						0x10 
#define BT_SAP_ResponseAPDU							0x05 
#define BT_SAP_ATR		 							0x06 
#define BT_SAP_CardReaderStatus						0x07 
#define BT_SAP_StatusChange 						0x08 
#define BT_SAP_Transport Protocol 					0x09 
	
uint8_t ATR[22];
int fd;

void sap_init();
void sap_close();
void createMessage(uint8_t *payload, uint8_t type);
int addParam(uint8_t* payload, uint8_t paramType, uint8_t paramLen, uint8_t* paramData, int pos);	
void _bt_sap_send(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t *data, uint16_t len);
unsigned char* process_APDU_req(uint8_t* packet, int cmd_offset);
int parse_APDU_resp(unsigned char* apdu_resp, uint8_t* data);