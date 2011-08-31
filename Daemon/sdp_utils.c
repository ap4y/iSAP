#include "sdp_utils.h"

void create_sap_service(uint8_t* service){
	uint8_t * attribute;
	uint8_t * protocolStack;
	uint8_t * l2cpProtocol;
	uint8_t * rfcommChannel;
	
    de_create_sequence(service);
    
    de_add_number(service, DE_UINT, DE_SIZE_16, SDP_ServiceClassIDList);
	attribute = de_push_sequence(service); 
	{ 
		de_add_number(attribute, DE_UUID, DE_SIZE_16, 0x112d); 
		de_add_number(attribute, DE_UUID, DE_SIZE_16, 0x1204); 
	} 
	de_pop_sequence(service, attribute);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, SDP_ProtocolDescriptorList);
	protocolStack = de_push_sequence(service); 
	{ 
		l2cpProtocol = de_push_sequence(protocolStack);
		{
			de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, 0x0100);
		}
		de_pop_sequence(protocolStack, l2cpProtocol);
		
		rfcommChannel = de_push_sequence(protocolStack);
		{
			de_add_number(rfcommChannel,  DE_UUID, DE_SIZE_16, 0x0003);
			de_add_number(rfcommChannel,  DE_UINT, DE_SIZE_8, RFCOMM_CHANNEL_ID);
		}
		de_pop_sequence(protocolStack, rfcommChannel);
	}
	de_pop_sequence(service, protocolStack);
	
    de_add_number(service, DE_UINT, DE_SIZE_16, SDP_BrowseGroupList);
	attribute = de_push_sequence(service); 
	{ 
		de_add_number(attribute, DE_UUID, DE_SIZE_16, 0x1002); 
	} 
	de_pop_sequence(service, attribute);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, SDP_LanguageBaseAttributeIDList);
	attribute = de_push_sequence(service); 
	{ 
		de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x656e);
		de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x006a);
		de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x0100); 
	} 
	de_pop_sequence(service, attribute);
	
	uint8_t * profileDescriptors;
	uint8_t * hidProtocol;
	de_add_number(service, DE_UINT, DE_SIZE_16, SDP_BluetoothProfileDescriptorList);
	profileDescriptors = de_push_sequence(service);
	{ 
		hidProtocol = de_push_sequence(profileDescriptors);
		{
			de_add_number(hidProtocol,  DE_UUID, DE_SIZE_16, 0x112d);
			de_add_number(hidProtocol,  DE_UINT, DE_SIZE_16, 0x0102);
		}
		de_pop_sequence(profileDescriptors, hidProtocol);
	}
	de_pop_sequence(service, profileDescriptors);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, 0x0100);
	de_add_data(service, DE_STRING, 16, (uint8_t *) "SIM Access");
}

void create_serial_port_service(uint8_t* service){
	uint8_t * attribute;
	
	// create HID service
    de_create_sequence(service);
    
    de_add_number(service, DE_UINT, DE_SIZE_16, SDP_ServiceClassIDList);
	attribute = de_push_sequence(service); 
	{ 
		de_add_number(attribute, DE_UUID, DE_SIZE_16, 0x1101); 
	} 
	de_pop_sequence(service, attribute);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, SDP_ProtocolDescriptorList);
	uint8_t * protocolStack = de_push_sequence(service); 
	{ 
		uint8_t * l2cpProtocol = de_push_sequence(protocolStack);
		{
			de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, 0x0100);
		}
		de_pop_sequence(protocolStack, l2cpProtocol);
		
		uint8_t * rfcommChannel = de_push_sequence(protocolStack);
		{
			de_add_number(rfcommChannel,  DE_UUID, DE_SIZE_16, 0x0003);
			de_add_number(rfcommChannel,  DE_UINT, DE_SIZE_8, 0x0001);
		}
		de_pop_sequence(protocolStack, rfcommChannel);
	}
	de_pop_sequence(service, protocolStack);
	
    de_add_number(service, DE_UINT, DE_SIZE_16, SDP_BrowseGroupList);
	attribute = de_push_sequence(service); 
	{ 
		de_add_number(attribute, DE_UUID, DE_SIZE_16, 0x1002); 
	} 
	de_pop_sequence(service, attribute);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, SDP_LanguageBaseAttributeIDList);
	attribute = de_push_sequence(service); 
	{ 
		de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x656e);
		de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x006a);
		de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x0100); 
	} 
	de_pop_sequence(service, attribute);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, SDP_BluetoothProfileDescriptorList);
	uint8_t * profileDescriptors = de_push_sequence(service);
	{ 
		uint8_t * hidProtocol = de_push_sequence(profileDescriptors);
		{
			de_add_number(hidProtocol,  DE_UUID, DE_SIZE_16, 0x1124);
			de_add_number(hidProtocol,  DE_UINT, DE_SIZE_16, 0x0100);
		}
		de_pop_sequence(profileDescriptors, hidProtocol);
	} 
	de_pop_sequence(service, profileDescriptors);
	
	de_add_number(service, DE_UINT, DE_SIZE_16, 0x0100);
	de_add_data(service, DE_STRING, 16, (uint8_t *) "RFCOMM");
}
