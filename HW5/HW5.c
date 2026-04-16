#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdbool.h>
#include <stdint.h>


// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
// Heartbeat LED pin (on Raspberry Pi Pico)
#define HEARTBEAT_PIN 15  //GP15 on the Raspberry Pi Pico
//MPU6050 PCB sets the 7 bit address of the chip to 0b1101000 (0x68).
#define MPU6050_ADDR 0x68

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

//function to write
void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    uint8_t buf[2];
    buf[0] = reg;      //Register address
    buf[1] = value;    //Value to write
    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

//function to read
unsigned char readPin(unsigned char address, unsigned char reg) {
    uint8_t buf[1];
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);  //Write register address to the device
    i2c_read_blocking(I2C_PORT, address, buf, 1, false);  //Read the data from the device
    return buf[0];  //Return the data read from the register
}

//write a function to initialize the MPU6050
static int mpu6050_init() {
    //To turn on the chip, write 0x00 to the PWR_MGMT_1 register to turn the chip on.
    //To enable the accelerometer, write to the ACCEL_CONFIG register. Set the sensitivity to plus minus 2g.
    //To enable the gyroscope, write to the GYRO_CONFIG register. Set the sensitivity to plus minus 2000 dps.
    setPin(MPU6050_ADDR, PWR_MGMT_1, 0x00);
    setPin(MPU6050_ADDR, ACCEL_CONFIG, 0x00);
    setPin(MPU6050_ADDR, GYRO_CONFIG, 0x18);
}

//write a function to read out all the data in a burst
bool read_mpu6050_data(float *accel_x, float *accel_y, float *accel_z,
                       float *temp_c,
                       float *gyro_x, float *gyro_y, float *gyro_z) {

    // Write starting register address (ACCEL_XOUT_H)
    uint8_t reg = ACCEL_XOUT_H;
    if (i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true) != 1) {
        return false;
    }
    uint8_t buf;  // buffer to hold 14 bytes

    // Read 14 bytes into buf
    if (i2c_read_blocking(I2C_PORT, MPU6050_ADDR, &buf, 14, false) != 14) {
        return false;
    }

    // Combine two bytes into int16_t helper macro
    #define COMBINE_BYTES(high, low) ((int16_t)((high << 8) | low))

    int16_t ax = COMBINE_BYTES(buf, buf);
    int16_t ay = COMBINE_BYTES(buf, buf);
    int16_t az = COMBINE_BYTES(buf, buf);
    int16_t temp_raw = COMBINE_BYTES(buf, buf);
    int16_t gx = COMBINE_BYTES(buf, buf);
    int16_t gy = COMBINE_BYTES(buf, buf);
    int16_t gz = COMBINE_BYTES(buf, buf);

    // Convert to physical units
    *accel_x = ax * 0.000061f;   // g
    *accel_y = ay * 0.000061f;
    *accel_z = az * 0.000061f;
    *temp_c = (temp_raw / 340.00f) + 36.53f;
    *gyro_x = gx * 0.00763f;     // degrees/sec
    *gyro_y = gy * 0.00763f;
    *gyro_z = gz * 0.00763f;

    #undef COMBINE_BYTES

    return true;
}

int main()
{
    stdio_init_all();

    //initialize heartbeat LED (GP15)
    gpio_init(HEARTBEAT_PIN);
    gpio_set_dir(HEARTBEAT_PIN, GPIO_OUT);


    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    //here is my function
    mpu6050_init();

    //initialize data points to read 
    float ax, ay, az, temp, gx, gy, gz;
    
    while (true) {
        
        //heartbeat for the pico
        
        //turn on the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 1);
        sleep_ms(500);  //wait 500 ms

        //turn off the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 0);
        sleep_ms(500);  //wait 500 ms

        
        //need to read from the WHO AM I register and verify that it is 0x68 (maybe 0x98)
        unsigned char who_am_i = readPin(MPU6050_ADDR, WHO_AM_I);

        if (who_am_i == 0x68) {
            printf("WHO AM I is correct\n");
        } else {
            printf("WHO AM I is not correct\n");
        }
        }

        if (read_mpu6050_data(&ax, &ay, &az, &temp, &gx, &gy, &gz)) {
            printf("Accel: X=%.3fg Y=%.3fg Z=%.3fg\n", ax, ay, az);
            printf("Temp: %.2f C\n", temp);
            printf("Gyro: X=%.2f Y=%.2f Z=%.2f dps\n", gx, gy, gz);
        } else {
            printf("Failed to read MPU6050 data\n");
        }
        sleep_ms(10);  // 100 Hz reading

        return 0;
}
