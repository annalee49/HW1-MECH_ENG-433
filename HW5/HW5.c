#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306.h"

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 8 //OLED SDA
#define I2C_SCL 9 //OLED SCL 
#define I2C_SDA_IMU 16 //IMU SDA
#define I2C_SCL_IMU 17 //IMU SCL

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

//OLED defines
#define OLED_WIDTH 128 //screen is 128 pixels wide
#define OLED_HEIGHT 64 //screen is 64 pixels talls
#define CENTER_X (OLED_WIDTH / 2)
#define CENTER_Y (OLED_HEIGHT / 2)
#define MAX_LINE_LENGTH 30

//function to write
void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    uint8_t buf[2];
    buf[0] = reg;      //register address
    buf[1] = value;    //value to write
    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

//function to read
unsigned char readPin(unsigned char address, unsigned char reg) {
    uint8_t buf[1];
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);  //write register address to the device
    i2c_read_blocking(I2C_PORT, address, buf, 1, false);  //read the data from the device
    return buf[0];  //return the data read from the register
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

    //write starting register address (ACCEL_XOUT_H)
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t buf;
    if (i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true) != 1) {
        return false;
    }
    //read 14 bytes into buf
    if (i2c_read_blocking(I2C_PORT, MPU6050_ADDR, &buf, 14, false) != 14) {
        return false;
    }

    //combine two bytes into int16_t helper macro
    #define COMBINE_BYTES(high, low) ((int16_t)((high << 8) | low))

    int16_t ax = COMBINE_BYTES(buf, buf);
    int16_t ay = COMBINE_BYTES(buf, buf);
    int16_t az = COMBINE_BYTES(buf, buf);
    int16_t temp_raw = COMBINE_BYTES(buf, buf);
    int16_t gx = COMBINE_BYTES(buf, buf);
    int16_t gy = COMBINE_BYTES(buf, buf);
    int16_t gz = COMBINE_BYTES(buf, buf);

    //convert to physical units
    *accel_x = ax * 0.000061f;   // g
    *accel_y = ay * 0.000061f;
    *accel_z = az * 0.000061f;
    *temp_c = (temp_raw / 340.00f) + 36.53f;
    *gyro_x = gx * 0.007630f;     // degrees per sec
    *gyro_y = gy * 0.007630f;
    *gyro_z = gz * 0.007630f;

    return true;
}

//function to write line
void ssd1306_draw_line(int x0, int y0, int x1, int y1, unsigned char color) {
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;  //error value e_xy

    while (1) {
        ssd1306_drawPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

//function to write on the OLED screen 
void draw_accel_lines(float ax, float ay) {

    //make the acceleration values proportional
    int16_t x_len = (int16_t)(ax * MAX_LINE_LENGTH); //acceleration in x direction
    int16_t y_len = (int16_t)(ay * MAX_LINE_LENGTH); //acceleration in y direction

    //draw horizontal line (X acceleration)
    ssd1306_draw_line(CENTER_X, CENTER_Y, CENTER_X + x_len, CENTER_Y, 1);

    //draw vertical line (Y acceleration)
    ssd1306_draw_line(CENTER_X, CENTER_Y, CENTER_X, CENTER_Y - y_len, 1);

    ssd1306_update();
}

void flash_pixels_on_oled(int flashes, int delay_ms) {
    for (int i = 0; i < flashes; i++) {
        // Draw a small square of pixels near the top-left corner
        ssd1306_drawPixel(2, 2, 1);
        ssd1306_drawPixel(3, 2, 1);
        ssd1306_drawPixel(2, 3, 1);
        ssd1306_drawPixel(3, 3, 1);

        ssd1306_update();  // Update OLED to show pixels
        sleep_ms(delay_ms);

        // Clear those pixels
        ssd1306_drawPixel(2, 2, 0);
        ssd1306_drawPixel(3, 2, 0);
        ssd1306_drawPixel(2, 3, 0);
        ssd1306_drawPixel(3, 3, 0);

        ssd1306_update();  // Update OLED to clear pixels
        sleep_ms(delay_ms);
    }
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
    gpio_set_function(I2C_SDA_IMU, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_IMU, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    gpio_pull_up(I2C_SDA_IMU);
    gpio_pull_up(I2C_SCL_IMU);

    sleep_ms(2000); //allow time for the system to initialize

    ssd1306_setup();  //initialize OLED
    ssd1306_clear(); //clear display buffer
    
    //here is my function
    mpu6050_init();

    //see if this works
    flash_pixels_on_oled(3, 200);

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

        //check the WHO_AM_I register
        if (who_am_i == 0x98) {
            printf("WHO AM I is correct\n");
        } else {
            printf("WHO AM I is not correct\n");
        }

        // //read the IMU data and draw lines on the OLED 
        // if (read_mpu6050_data(&ax, &ay, &az, &temp, &gx, &gy, &gz)) {
        //     //flash_pixels_on_oled(3, 200);  //sanity check
        //     draw_accel_lines(ax, ay);
        // }

        // Always attempt to read IMU data and draw lines
        bool success = read_mpu6050_data(&ax, &ay, &az, &temp, &gx, &gy, &gz);

        // Optionally, you can handle failure by setting default values or clearing the display
        if (!success) {
            // For example, clear lines or set zero acceleration
            ax = 0.0f;
            ay = 0.0f;
            // Optionally clear display or show error indicator here
        }

        // Draw lines regardless of success
        draw_accel_lines(ax, ay);
        sleep_ms(10);  // 100 Hz reading
        }

}