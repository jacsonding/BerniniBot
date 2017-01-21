#ifndef __DRV_IMU_H__
#define __DRV_IMU_H__

#include "nrf_drv_twi.h"

#define LSM303_ADDRESS_ACCEL          (0x32 >> 1)         // 0011001x
#define LSM303_ADDRESS_MAG            (0x3C >> 1)         // 0011110x
#define LSM303_ID                     (0xD4)
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
                                                // DEFAULT    TYPE
#define LSM303_REGISTER_ACCEL_CTRL_REG1_A         (0x20)   // 00000111   rw
#define LSM303_REGISTER_ACCEL_CTRL_REG2_A         (0x21)   // 00000000   rw
#define LSM303_REGISTER_ACCEL_CTRL_REG3_A         (0x22)   // 00000000   rw
#define LSM303_REGISTER_ACCEL_CTRL_REG4_A         (0x23)   // 00000000   rw
#define LSM303_REGISTER_ACCEL_CTRL_REG5_A         (0x24)   // 00000000   rw
#define LSM303_REGISTER_ACCEL_CTRL_REG6_A         (0x25)   // 00000000   rw
#define LSM303_REGISTER_ACCEL_REFERENCE_A         (0x26)   // 00000000   r
#define LSM303_REGISTER_ACCEL_STATUS_REG_A        (0x27)   // 00000000   r
#define LSM303_REGISTER_ACCEL_OUT_X_L_A           (0x28)
#define LSM303_REGISTER_ACCEL_OUT_X_H_A           (0x29)
#define LSM303_REGISTER_ACCEL_OUT_Y_L_A           (0x2A)
#define LSM303_REGISTER_ACCEL_OUT_Y_H_A           (0x2B)
#define LSM303_REGISTER_ACCEL_OUT_Z_L_A           (0x2C)
#define LSM303_REGISTER_ACCEL_OUT_Z_H_A           (0x2D)
#define LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A     (0x2E)
#define LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A      (0x2F)
#define LSM303_REGISTER_ACCEL_INT1_CFG_A          (0x30)
#define LSM303_REGISTER_ACCEL_INT1_SOURCE_A       (0x31)
#define LSM303_REGISTER_ACCEL_INT1_THS_A          (0x32)
#define LSM303_REGISTER_ACCEL_INT1_DURATION_A     (0x33)
#define LSM303_REGISTER_ACCEL_INT2_CFG_A          (0x34)
#define LSM303_REGISTER_ACCEL_INT2_SOURCE_A       (0x35)
#define LSM303_REGISTER_ACCEL_INT2_THS_A          (0x36)
#define LSM303_REGISTER_ACCEL_INT2_DURATION_A     (0x37)
#define LSM303_REGISTER_ACCEL_CLICK_CFG_A         (0x38)
#define LSM303_REGISTER_ACCEL_CLICK_SRC_A         (0x39)
#define LSM303_REGISTER_ACCEL_CLICK_THS_A         (0x3A)
#define LSM303_REGISTER_ACCEL_TIME_LIMIT_A        (0x3B)
#define LSM303_REGISTER_ACCEL_TIME_LATENCY_A      (0x3C)
#define LSM303_REGISTER_ACCEL_TIME_WINDOW_A       (0x3D)

#define TWI_SCL_PIN_NUMBER (11)
#define TWI_SDA_PIN_NUMBER (12)

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} drv_imu_accel_data_t;

/**
 * @brief Initialization struct for accelerometer driver
 */
typedef struct
{
    nrf_drv_twi_t const *        p_twi_instance;
} drv_imu_init_t;

ret_code_t drv_imu_init(drv_imu_init_t * p_params);

ret_code_t drv_imu_accel_data_get(drv_imu_accel_data_t * p_accel_data);

#endif //__DRV_IMU_H__
