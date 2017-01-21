#ifndef BLE_NTS_H__
#define BLE_NTS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// BASE UUID: 87de0449-51b5-43c3-9ccb-993004dd54aa

#define NTS_UUID_BASE               {0xAA, 0x54, 0xDD, 0x04, 0x30, 0x99, 0xCB, 0x9C, \
                                    0xC3, 0x43, 0xB5, 0x51, 0xFF, 0xFF, 0xDE, 0x87}
#define NTS_UUID_SERVICE            0x0001
#define NTS_UUID_TYPE_CHAR          0x0002
#define NTS_UUID_STATUS_CHAR        0x0003
#define NTS_UUID_CONFIRMATION_CHAR  0x0004
#define NTS_UUID_QUEUE_CHAR         0x0005


// Forward declaration of the ble_nts_t type.
typedef struct ble_nts_s ble_nts_t;

typedef void (*ble_nts_type_write_handler_t) (ble_nts_t * p_nts, uint8_t coffee_type);
typedef void (*ble_nts_confirm_write_handler_t) (ble_nts_t * p_nts, bool confirm);
typedef void (*ble_nts_status_subscr_handler_t) (ble_nts_t * p_nts, bool status_subscr);
typedef void (*ble_nts_queue_subscr_handler_t) (ble_nts_t * p_nts, bool queue_subscr);


typedef struct
{
    ble_nts_type_write_handler_t     type_write_handler;
    ble_nts_confirm_write_handler_t  confirm_write_handler;
    ble_nts_status_subscr_handler_t  status_subscr_handler;
    ble_nts_queue_subscr_handler_t   queue_subscr_handler;
    uint16_t                         current_queue_index;
    uint16_t                         current_status;
} ble_nts_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_nts_s
{
    uint16_t                          service_handle;
    ble_gatts_char_handles_t          type_char_handles;
    ble_gatts_char_handles_t          confirmation_char_handles;
    ble_gatts_char_handles_t          queue_char_handles;
    ble_gatts_char_handles_t          status_char_handles;
    uint8_t                           uuid_type;
    uint16_t                          conn_handle;
    ble_nts_type_write_handler_t     type_write_handler;
    ble_nts_confirm_write_handler_t  confirm_write_handler;
    ble_nts_status_subscr_handler_t  status_subscr_handler;
    ble_nts_queue_subscr_handler_t   queue_subscr_handler;
};

uint32_t ble_nts_init(ble_nts_t * p_nts, const ble_nts_init_t * p_nts_init);

void ble_nts_on_ble_evt(ble_nts_t * p_nts, ble_evt_t * p_ble_evt);

uint32_t ble_nts_queue_notify(ble_nts_t * p_nts, uint8_t * p_current_queue);

uint32_t ble_nts_status_notify(ble_nts_t * p_nts, uint8_t * p_status);


#ifdef __cplusplus
}
#endif

#endif // BLE_NTS_H__

/** @} */
