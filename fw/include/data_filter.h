#ifndef DATA_FILTER__H__
#define DATA_FILTER__H__

#define MOVING_AVG_SIZE 8

#include "drv_imu.h"


drv_imu_accel_data_t* calculate_running_avg(drv_imu_accel_data_t * p_accel_data);
void data_filter_init(void);

#endif //DATA_FILTER__H__
