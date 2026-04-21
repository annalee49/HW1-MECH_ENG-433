#include "imu.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdint.h>

#define I2C_PORT i2c1
#define MPU6050_ADDR 0x68

#define PWR_MGMT_1 0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B
#define ACCEL_XOUT_H 0x3B
#define WHO_AM_I 0x75

static void set_reg(uint8_t address, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(i2c1, address, buf, 2, false);
}

static uint8_t read_reg(uint8_t address, uint8_t reg) {
    uint8_t buf[1];
    i2c_write_blocking(i2c1, address, &reg, 1, true);
    i2c_read_blocking(i2c1, address, buf, 1, false);
    return buf[0];
}

bool imu_init(void) {
    set_reg(MPU6050_ADDR, PWR_MGMT_1, 0x00);
    set_reg(MPU6050_ADDR, ACCEL_CONFIG, 0x00);
    set_reg(MPU6050_ADDR, GYRO_CONFIG, 0x18);

    sleep_ms(100);

    // optional sanity check
    uint8_t who = read_reg(MPU6050_ADDR, WHO_AM_I);
    return (who == 0x68 || who == 0x98);
}

bool imu_read(float *accel_x, float *accel_y, float *accel_z,
              float *temp_c,
              float *gyro_x, float *gyro_y, float *gyro_z) {

    uint8_t reg = ACCEL_XOUT_H;
    uint8_t buf[14];

    i2c_write_blocking(i2c1, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c1, MPU6050_ADDR, buf, 14, false);

    int16_t ax = (buf[0] << 8) | buf[1];
    int16_t ay = (buf[2] << 8) | buf[3];
    int16_t az = (buf[4] << 8) | buf[5];

    int16_t temp_raw = (buf[6] << 8) | buf[7];

    int16_t gx = (buf[8] << 8) | buf[9];
    int16_t gy = (buf[10] << 8) | buf[11];
    int16_t gz = (buf[12] << 8) | buf[13];

    *accel_x = ax * 0.000061f;
    *accel_y = ay * 0.000061f;
    *accel_z = az * 0.000061f;

    *temp_c = (temp_raw / 340.0f) + 36.53f;

    *gyro_x = gx * 0.007630f;
    *gyro_y = gy * 0.007630f;
    *gyro_z = gz * 0.007630f;

    return true;
}