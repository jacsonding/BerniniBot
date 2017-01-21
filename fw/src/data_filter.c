#include "data_filter.h"

drv_imu_accel_data_t averaged_data =
{
 .x = 0,
 .y = 0,
 .z = 0
};

drv_imu_accel_data_t zero =
{
  .x = 0,
  .y = 0,
  .z = 0
};

drv_imu_accel_data_t*p_current_accel_data[MOVING_AVG_SIZE];

static void update_moving_average(drv_imu_accel_data_t * p_accel_data) {
  // Shift every element in the array to the left by one. Eliminate first item
  for (uint8_t i = 1; i < MOVING_AVG_SIZE; i++) {
    p_current_accel_data[i-1] = p_current_accel_data[i];
  }
  // Add in the new data to the end of the current_accel_data array
  p_current_accel_data[MOVING_AVG_SIZE-1] = p_accel_data;
}

void data_filter_init(void)
{
	for(uint8_t x = 0; x < MOVING_AVG_SIZE; x++)
	{
		p_current_accel_data[x] = &zero;
	}
}

drv_imu_accel_data_t* calculate_running_avg(drv_imu_accel_data_t * p_accel_data) {

    update_moving_average(p_accel_data);

    averaged_data.x = 0;
    averaged_data.y = 0;
    averaged_data.z = 0;

     int32_t x_total = 0;
     int32_t y_total = 0;
     int32_t z_total = 0;

     for (int i=0; i < MOVING_AVG_SIZE; i++) {
       x_total += p_current_accel_data[i]->x;
       y_total += p_current_accel_data[i]->y;
       z_total += p_current_accel_data[i]->z;
     }


     averaged_data.x = (int16_t)(x_total / MOVING_AVG_SIZE);
     averaged_data.y = (int16_t)(y_total / MOVING_AVG_SIZE);
     averaged_data.z = (int16_t)(z_total / MOVING_AVG_SIZE);

     return &averaged_data;
}
