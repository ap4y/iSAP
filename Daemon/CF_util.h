#pragma once
#import <CoreFoundation/CoreFoundation.h>

#define _messagePortName_client "iSAP_GUI"
#define _messagePortName_daemon "iSAP"

#define CFMSG_nop					0
#define CFMSG_setMode				1
#define CFMSG_setPIN				2
#define CFMSG_setPIN_Resp			3
#define CFMSG_newState				4

#define iSAP_state_error			10
#define iSAP_state_off				0
#define iSAP_state_starting			1
#define iSAP_state_ready			2
#define iSAP_state_starting_hci		3
#define iSAP_state_starting_l2cap	4
#define iSAP_state_starting_rfcomm	5
#define iSAP_state_starting_sap		6
#define iSAP_state_starting_working	7

uint8_t curState;
CFMessagePortRef createMessagePortGUI(CFStringRef messagePortName, CFMessagePortCallBack myCallBack);
int sendMessageToGUI(UInt8 cmd, UInt16 dataLen, UInt8 *data, CFDataRef *resultData, CFStringRef messagePortName);
void changeState(uint8_t newState);