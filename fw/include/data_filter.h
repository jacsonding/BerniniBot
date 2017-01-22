#ifndef DATA_FILTER__H__
#define DATA_FILTER__H__

#define MOVING_AVG_SIZE 2

#include "drv_imu.h"


drv_imu_accel_data_t* calculate_running_avg(drv_imu_accel_data_t * p_accel_data);


void data_filter_init(drv_imu_accel_data_t * p_init_accel_data);

#endif //DATA_FILTER__H__
