#include "CF_util.h"

//CFMessagePortRef accessMessagePort = 0;

int sendMessageToGUI(UInt8 cmd, UInt16 dataLen, UInt8 *data, CFDataRef *resultData, CFStringRef messagePortName){
	
	CFMessagePortRef accessMessagePort = CFMessagePortCreateRemote(NULL, messagePortName);
	int result = 0;
	
	if (accessMessagePort == NULL) {		
		return kCFMessagePortIsInvalid;
	}
	else{
		// create and send message
		CFDataRef cfData = CFDataCreate(NULL, data, dataLen);
		CFStringRef replyMode = NULL;
		if (resultData) {
			replyMode = kCFRunLoopDefaultMode;
		}
		result = CFMessagePortSendRequest(accessMessagePort, cmd, cfData, 1, 1, replyMode, resultData);
		if (result == -4) {
			CFMessagePortInvalidate(accessMessagePort);
		}
		CFRelease(cfData);
		CFRelease(accessMessagePort);
	}
	return result;
}

CFMessagePortRef createMessagePortGUI(CFStringRef messagePortName, CFMessagePortCallBack myCallBack)
{
	Boolean shouldFreeInfo = false;
	CFMessagePortRef local = CFMessagePortCreateLocal(NULL, messagePortName, myCallBack, NULL, &shouldFreeInfo);	
	CFRunLoopSourceRef source = CFMessagePortCreateRunLoopSource(NULL, local, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);	

	if (shouldFreeInfo || local == NULL) {
		return NULL;
	}
	
	return local;
}

void changeState(uint8_t newState)
{
	curState = newState;
	sendMessageToGUI(CFMSG_newState, 1, &newState, NULL, CFSTR(_messagePortName_client));
}
