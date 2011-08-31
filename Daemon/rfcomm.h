#include <stdint.h>
#include <string.h>
#include <btstack/btstack.h>

// Control field values      bit no.       1 2 3 4 5   6 7 8
#define BT_RFCOMM_SABM       0x3F       // 1 1 1 1 P/F 1 0 0
#define BT_RFCOMM_UA         0x73       // 1 1 0 0 P/F 1 1 0
#define BT_RFCOMM_DM         0x0F       // 1 1 1 1 P/F 0 0 0
#define BT_RFCOMM_DM_PF      0x1F
#define BT_RFCOMM_DISC       0x53       // 1 1 0 0 P/F 0 1 1
#define BT_RFCOMM_UIH        0xEF       // 1 1 1 1 P/F 1 1 1
#define BT_RFCOMM_UIH_PF     0xFF

// Multiplexer message types 
#define BT_RFCOMM_PN_CMD     0x83
#define BT_RFCOMM_PN_RSP     0x81
#define BT_RFCOMM_TEST_CMD   0x23
#define BT_RFCOMM_TEST_RSP   0x21
#define BT_RFCOMM_FCON_CMD   0xA3
#define BT_RFCOMM_FCON_RSP   0xA1
#define BT_RFCOMM_FCOFF_CMD  0x63
#define BT_RFCOMM_FCOFF_RSP  0x61
#define BT_RFCOMM_MSC_CMD    0xE3
#define BT_RFCOMM_MSC_RSP    0xE1
#define BT_RFCOMM_RPN_CMD    0x93
#define BT_RFCOMM_RPN_RSP    0x91
#define BT_RFCOMM_RLS_CMD    0x53
#define BT_RFCOMM_RLS_RSP    0x51
#define BT_RFCOMM_NSC_RSP    0x11

#define	RFCOMM_CHANNEL_ID		4

void rfcomm_send_packet(uint16_t source_cid, uint8_t address, uint8_t control, uint8_t credits, uint8_t *data, uint16_t len);
//void _bt_rfcomm_send_sabm(uint16_t source_cid, uint8_t initiator, uint8_t channel);
//void _bt_rfcomm_send_disc(uint16_t source_cid, uint8_t initiator, uint8_t channel);
//void _bt_rfcomm_send_uih_data(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t *data, uint16_t len);
//void _bt_rfcomm_send_uih_test_cmd(uint16_t source_cid, uint8_t initiator, uint8_t channel);
void _bt_rfcomm_send_uih_msc_cmd(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t signals);
void _bt_rfcomm_send_uih_msc_rsp(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t signals);
//void _bt_rfcomm_send_uih_pn_command(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint16_t max_frame_size);
void _bt_rfcomm_send_ua(uint16_t source_cid, uint8_t address);
void _bt_rfcomm_send_uih_pn_response(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t max_frame_size_low, uint8_t max_frame_size_hi);
//void _bt_sap_send(uint16_t source_cid, uint8_t initiator, uint8_t channel, uint8_t *data, uint16_t len);
