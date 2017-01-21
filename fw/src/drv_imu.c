#include "drv_imu.h"
#include "nrf_drv_twi.h"
#include "app_util_platform.h"
#include "nordic_common.h"
#include <string.h>

#define RETURN_IF_ERROR(PARAM)                                                                    \
        if ((PARAM) != NRF_SUCCESS)                                                               \
        {                                                                                         \
            return (PARAM);                                                                       \
        }

const nrf_drv_twi_config_t twi_accel_config =
{
    .scl                = TWI_SCL_PIN_NUMBER,
    .sda                = TWI_SDA_PIN_NUMBER,
    .frequency          = NRF_TWI_FREQ_400K,
    .interrupt_priority = APP_IRQ_PRIORITY_LOW
};

typedef struct
{
    const nrf_drv_twi_t *         p_twi_instance;
} drv_imu_t;

static drv_imu_t            m_drv_imu;

/**
 * @brief Function to deinit the TWI module when this driver does not need to
 *        communicate on the TWI bus, so that other drivers can use the module
 */
static void __inline twi_close(void)
{
    nrf_drv_twi_disable(m_drv_imu.p_twi_instance);
    nrf_drv_twi_uninit(m_drv_imu.p_twi_instance);
}
/**
 * @brief Function to occupy the TWI module when this driver needs to
 *        communicate on the TWI bus, so that other drivers cannot use the module
 */
static ret_code_t __inline twi_open(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_twi_init(m_drv_imu.p_twi_instance,
                                &twi_accel_config,
                                NULL,
                                NULL);
    RETURN_IF_ERROR(err_code);
    nrf_drv_twi_enable(m_drv_imu.p_twi_instance);
    return NRF_SUCCESS;
}

static ret_code_t reg_read(uint8_t reg_addr, uint8_t * p_reg_val)
{
    ret_code_t err_code;

    err_code = twi_open();
    RETURN_IF_ERROR(err_code);

    err_code = nrf_drv_twi_tx( m_drv_imu.p_twi_instance,
                               LSM303_ADDRESS_ACCEL,
                               &reg_addr,
                               1,
                               true );
    RETURN_IF_ERROR(err_code);

    err_code = nrf_drv_twi_rx( m_drv_imu.p_twi_instance,
                               LSM303_ADDRESS_ACCEL,
                               p_reg_val,
                               1 );
    RETURN_IF_ERROR(err_code);

    twi_close();


    return NRF_SUCCESS;
}

static ret_code_t reg_write(uint8_t reg_addr, uint8_t reg_val)
{
    uint32_t err_code;

    uint8_t buffer[2] = {reg_addr,reg_val};

    err_code = twi_open();
    RETURN_IF_ERROR(err_code);

    err_code = nrf_drv_twi_tx( m_drv_imu.p_twi_instance,
                               LSM303_ADDRESS_ACCEL,
                               buffer,
                               2,
                               false);
    RETURN_IF_ERROR(err_code);

    twi_close();

    return NRF_SUCCESS;
}

ret_code_t drv_imu_init(drv_imu_init_t * p_params)
{
    ret_code_t err_code;
    uint8_t ctrl_reg1_read_value;
    uint8_t ctrl_reg1_write_value = 0x27; //10 Hz, Normal Power Mode. XYZ Enabled

    m_drv_imu.p_twi_instance = p_params->p_twi_instance;

    err_code = reg_write(LSM303_REGISTER_ACCEL_CTRL_REG1_A, ctrl_reg1_write_value); //10 Hz, Normal Power Mode. XYZ Enabled
    RETURN_IF_ERROR(err_code);

    err_code = reg_read(LSM303_REGISTER_ACCEL_CTRL_REG1_A, &ctrl_reg1_read_value);
    RETURN_IF_ERROR(err_code);

    if (ctrl_reg1_read_value != ctrl_reg1_write_value)
    {
      return NRF_ERROR_INVALID_STATE;
    }
    else
    {
      return NRF_SUCCESS;
    }
}

ret_code_t drv_imu_accel_data_read(drv_imu_accel_data_t * p_accel_data)
{
  //Stuff goes here
  uint8_t x_low;
  uint8_t x_high;
  uint8_t y_low;
  uint8_t y_high;
  uint8_t z_low;
  uint8_t z_high;
  ret_code_t err_code;

 // Read from the x accel registers
  err_code = reg_read(LSM303_REGISTER_ACCEL_OUT_X_L_A, &x_low);
  RETURN_IF_ERROR(err_code);
  err_code = reg_read(LSM303_REGISTER_ACCEL_OUT_X_H_A, &x_high);
  RETURN_IF_ERROR(err_code);

// Read from the y accel registers
  err_code = reg_read(LSM303_REGISTER_ACCEL_OUT_Y_L_A, &y_low);
  RETURN_IF_ERROR(err_code);
  err_code = reg_read(LSM303_REGISTER_ACCEL_OUT_Y_H_A, &y_high);
  RETURN_IF_ERROR(err_code);

  // Read from the z accel registers
  err_code = reg_read(LSM303_REGISTER_ACCEL_OUT_Z_L_A, &z_low);
  RETURN_IF_ERROR(err_code);
  err_code = reg_read(LSM303_REGISTER_ACCEL_OUT_Z_H_A, &z_high);
  RETURN_IF_ERROR(err_code);

  p_accel_data->x = (int16_t)(((x_high << 8) | x_low)) >> 4;
  p_accel_data->y = (int16_t)(((y_high << 8) | y_low)) >> 4;
  p_accel_data->z = (int16_t)(((z_high << 8) | z_low)) >> 4;
  //WHY RIGHT SHIFT BY 4?????

  return NRF_SUCCESS;
}
