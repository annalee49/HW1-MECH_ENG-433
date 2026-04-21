#ifndef IMU_H
#define IMU_H

#include <stdbool.h>
#include <stdint.h>

// Initialize the MPU6050
bool imu_init(void);

// Read full sensor data
bool imu_read(float *accel_x, float *accel_y, float *accel_z,
              float *temp_c,
              float *gyro_x, float *gyro_y, float *gyro_z);

#endif