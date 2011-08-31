#include "rfcomm.h"

uint8_t rfcomm_out_buffer[1000];

void rfcomm_send_packet(uint16_t source_cid, uint8_t address, uint8_t control, uint8_t credits, uint8_t *data, uint16_t len){
	
	uint16_t pos = 0;
	uint8_t crc_fields = 3;
	
	rfcomm_out_buffer[pos++] = address;
	rfcomm_out_buffer[pos++] = control;
	
	// length field can be 1 or 2 octets
	if (len < 128){
		rfcomm_out_buffer[pos++] = (len << 1)| 1;     // bits 0-6
	} else {
		rfcomm_out_buffer[pos++] = (len & 0x7f) << 1; // bits 0-6
		rfcomm_out_buffer[pos++] = len >> 7;          // bits 7-14
		crc_fields++;
	}
	
	// add credits for UIH frames when PF bit is set
	if (control == BT_RFCOMM_UIH_PF){
		rfcomm_out_buffer[pos++] = credits;
	}
	
	// copy actual data
	memcpy(&rfcomm_out_buffer[pos], data, len);
	pos += len;
	
	// UIH frames only calc FCS over address + control (5.1.1)
	if ((control & 0xef) == BT_RFCOMM_UIH){
		crc_fields = 2;
	}
	rfcomm_out_buffer[pos++] =  crc8_calc(rfcomm_out_buffer, crc_fields); // calc fcs
    bt_send_l2cap( source_cid, rfcomm_out_buffer, pos);
}

void _bt_rfcomm_send_sabm(uint16_t source_cid, uint8_t initiator, uint8_t channel)
{
	uint8_t address = (1 << 0) | (initiator << 1) |  (initiator << 1) | (channel << 3); 
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_SABM, 0, NULL, 0);
}

void _bt_rfcomm_send_disc(uint16_t source_cid, uint8_t initiator, uint8_t channel)
{
	uint8_t address = (1 << 0) | (initiator << 1) |  (initiator << 1) | (channel << 3); 
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_DISC, 0, NULL, 0);
}

void _bt_rfcomm_send_uih_data(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t *data, uint16_t len) {
	uint8_t address = (1 << 0) | (initiator << 1) |  (initiator << 1) | (channel << 3); 
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0, data, len);
}	

void _bt_rfcomm_send_uih_test_cmd(uint16_t source_cid, uint8_t initiator, uint8_t channel)
{
	uint8_t address = (1 << 0) | (initiator << 1) | (initiator << 1) | (channel << 3) ; // EA and C/R bit set - always server channel 0
	uint8_t payload[13]; 
	uint8_t pos = 0;
	payload[pos++] = BT_RFCOMM_TEST_CMD;
	payload[pos++] = 11 << 1 | 1;  // len
	
	payload[pos++] = 0x00;
	payload[pos++] = 0x01;
	payload[pos++] = 0x02;
	payload[pos++] = 0x03;
	payload[pos++] = 0x04;
	payload[pos++] = 0x05;
	payload[pos++] = 0x06;
	payload[pos++] = 0x07;
	payload[pos++] = 0x08;
	payload[pos++] = 0x09;
	payload[pos++] = 0x0a;
	
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0, (uint8_t *) payload, pos);
}

void _bt_rfcomm_send_uih_msc_cmd(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t signals)
{
	uint8_t address = (1 << 0) | (initiator << 1); // EA and C/R bit set - always server channel 0
	uint8_t payload[4]; 
	uint8_t pos = 0;
	payload[pos++] = BT_RFCOMM_MSC_CMD;
	payload[pos++] = 2 << 1 | 1;  // len
	payload[pos++] = (1 << 0) | (1 << 1) | (0 << 2) | (channel << 3);; // shouldn't D = initiator = 1 ?
	payload[pos++] = signals;
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0, (uint8_t *) payload, pos);
}

void _bt_rfcomm_send_uih_msc_rsp(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t signals)
{
	uint8_t address = (1 << 0) | (initiator << 1); // EA and C/R bit set - always server channel 0
	uint8_t payload[4]; 
	uint8_t pos = 0;
	payload[pos++] = BT_RFCOMM_MSC_RSP;
	payload[pos++] = 2 << 1 | 1;  // len
	payload[pos++] = (1 << 0) | (1 << 1) | (0 << 2) | (channel << 3); // shouldn't D = initiator = 1 ?
	payload[pos++] = signals;
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0, (uint8_t *) payload, pos);
}

void _bt_rfcomm_send_uih_pn_command(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint16_t max_frame_size){
	uint8_t payload[10];
	uint8_t address = (1 << 0) | (initiator << 1); // EA and C/R bit set - always server channel 0
	uint8_t pos = 0;
	payload[pos++] = BT_RFCOMM_PN_CMD;
	payload[pos++] = 8 << 1 | 1;  // len
	payload[pos++] = channel << 1;
	payload[pos++] = 0xf0; // pre defined for Bluetooth, see 5.5.3 of TS 07.10 Adaption for RFCOMM
	payload[pos++] = 0; // priority
	payload[pos++] = 0; // max 60 seconds ack
	payload[pos++] = max_frame_size & 0xff; // max framesize low
	payload[pos++] = max_frame_size >> 8;   // max framesize high
	payload[pos++] = 0x00; // number of retransmissions
	payload[pos++] = 0x00; // unused error recovery window
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0, (uint8_t *) payload, pos);
}

void _bt_rfcomm_send_ua(uint16_t source_cid, uint8_t address)
{
	//uint8_t address = (1 << 0) | (initiator << 1) |  (initiator << 1) | (channel << 3); 
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UA, 0, NULL, 0);
}

void _bt_rfcomm_send_uih_pn_response(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t max_frame_size_low, uint8_t max_frame_size_hi){
	uint8_t payload[10];
	uint8_t address = (1 << 0) | (initiator << 1); // EA and C/R bit set - always server channel 0
	uint8_t pos = 0;
	payload[pos++] = BT_RFCOMM_PN_RSP;
	payload[pos++] = 8 << 1 | 1;  // len
	payload[pos++] = channel << 1;
	payload[pos++] = 0xe0; // pre defined for Bluetooth, see 5.5.3 of TS 07.10 Adaption for RFCOMM
	payload[pos++] = 0; // priority
	payload[pos++] = 0; // max 60 seconds ack
	//payload[pos++] = max_frame_size_low; // max framesize low
	//payload[pos++] = max_frame_size_hi;   // max framesize high
	payload[pos++] = 0x91; // max framesize low
	payload[pos++] = 0x00;   // max framesize high
	payload[pos++] = 0x00; // number of retransmissions
	payload[pos++] = 0x03; // unused error recovery window
	rfcomm_send_packet(source_cid, address, BT_RFCOMM_UIH, 0, (uint8_t *) payload, pos);
}
