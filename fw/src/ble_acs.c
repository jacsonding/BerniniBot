/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */
#include "sdk_config.h"
#include "ble_acs.h"
#include "ble_srv_common.h"
#include "sdk_common.h"
#include "drv_imu.h"


static void on_connect(ble_acs_t * p_acs, ble_evt_t * p_ble_evt)
{
    p_acs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


static void on_disconnect(ble_acs_t * p_acs, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_acs->conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void on_write(ble_acs_t * p_acs, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_acs->accel_data_char_handles.cccd_handle) &&
        (p_evt_write->len == 2) &&
        (p_acs->data_subscr_handler != NULL))
    {
        if (*(p_evt_write->data))
        {
          p_acs->data_subscr_handler(p_acs, true);
        }
        else
        {
          p_acs->data_subscr_handler(p_acs, false);
        }
    }
    else if ((p_evt_write->handle == p_acs->btn_data_char_handles.cccd_handle) &&
        (p_evt_write->len == 2) &&
        (p_acs->btn_subscr_handler != NULL))
    {
        if (*(p_evt_write->data))
        {
          p_acs->btn_subscr_handler(p_acs, true);
        }
        else
        {
          p_acs->btn_subscr_handler(p_acs, false);
        }
    }
}


void ble_acs_on_ble_evt(ble_acs_t * p_acs, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_acs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_acs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_acs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

static uint32_t button_char_add(ble_acs_t * p_acs, const ble_acs_init_t * p_acs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_acs->uuid_type;
    ble_uuid.uuid = ACS_UUID_BUTTON_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    uint8_t init_val = 0;

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &init_val;

    return sd_ble_gatts_characteristic_add(p_acs->service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &p_acs->btn_data_char_handles);
}


static uint32_t accel_data_char_add(ble_acs_t * p_acs, const ble_acs_init_t * p_acs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_acs->uuid_type;
    ble_uuid.uuid = ACS_UUID_ACCEL_DATA_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    drv_imu_accel_data_t init_val =
    {
      .x = 0,
      .y = 0,
      .z = 0
    };

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(drv_imu_accel_data_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(drv_imu_accel_data_t);
    attr_char_value.p_value      = (uint8_t*)(&init_val);

    return sd_ble_gatts_characteristic_add(p_acs->service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &p_acs->accel_data_char_handles);
}

uint32_t ble_acs_init(ble_acs_t * p_acs, const ble_acs_init_t * p_acs_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure.
    p_acs->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_acs->data_subscr_handler = p_acs_init->data_subscr_handler;
    p_acs->btn_subscr_handler = p_acs_init->btn_subscr_handler;

    // Add service.
    ble_uuid128_t base_uuid = {ACS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_acs->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_acs->uuid_type;
    ble_uuid.uuid = ACS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_acs->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add characteristics.
    err_code = accel_data_char_add(p_acs, p_acs_init);
    VERIFY_SUCCESS(err_code);

    err_code = button_char_add(p_acs, p_acs_init);
    VERIFY_SUCCESS(err_code);


    return NRF_SUCCESS;
}

uint32_t ble_acs_accel_data_notify(ble_acs_t * p_acs, uint8_t * p_accel_data)
{
    ble_gatts_hvx_params_t hvx_params;
    uint16_t               length = sizeof(drv_imu_accel_data_t);

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_acs->accel_data_char_handles.value_handle;
    hvx_params.p_data = (uint8_t *)p_accel_data;
    hvx_params.p_len  = &length;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

    return sd_ble_gatts_hvx(p_acs->conn_handle, &hvx_params);
}

uint32_t ble_acs_on_button_change(ble_acs_t * p_acs, uint8_t button_state, uint8_t button_id)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(uint8_t);

    // |Bit 7 | Bit 6 - 0 |
    // |On/Off|Which Button (0,1,2,3)|
    uint8_t data = ((button_state << 7) | button_id);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_acs->btn_data_char_handles.value_handle;
    params.p_data = &data;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_acs->conn_handle, &params);
}
