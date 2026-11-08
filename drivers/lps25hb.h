#ifndef LPS25HB_H
#define LPS25HB_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* --- DEVICE IDENTIFIER --- */
#define LPS25_WHOAMI_ADDR       0x0F
#define LPS25_WHOAMI_VALUE      0xBD

/* --- CONTROL REGISTERS --- */
#define LPS25_REG_CTRL1         0x20
#define LPS25_REG_CTRL2         0x21
#define LPS25_REG_CTRL3         0x22
#define LPS25_REG_CTRL4         0x23

/* --- STATUS & INTERRUPT --- */
#define LPS25_REG_STATUS        0x27

/* --- PRESSURE OUTPUT REGISTERS (24-bit) --- */
#define LPS25_REG_PRESS_OUT_XL  0x28
#define LPS25_REG_PRESS_OUT_L   0x29
#define LPS25_REG_PRESS_OUT_H   0x2A

#define LPS25_REG_TEMP_OUT_L    0x2B
#define LPS25_REG_TEMP_OUT_H    0x2C

#define LPS25_REG_RPDS_L        0x39
#define LPS25_REG_RPDS_H        0x3A

#define LPS25_AUTO_INCREMENT    0x80

#define LPS25_CTRL1_PD          (1 << 7)
#define LPS25_CTRL1_ODR_12_5HZ  (3 << 4)  
#define LPS25_CTRL1_BDU          (1 << 2)  

volatile uint32_t system_timer = 0;

typedef int (*lps_i2c_read_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef int (*lps_i2c_write_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

typedef struct
{
    uint8_t i2c_adress;
    lps_i2c_read_t i2c_read;
    lps_i2c_write_t i2c_write;
    uint32_t (*get_tick)(void);

} lps_data;

static uint32_t counter = 0;
typedef struct{

    uint32_t id;
    uint32_t timestamp;

    double altitude;
    float temparature;
    float pressure;

}calibration;

int lps_init(lps_data *dev, uint8_t adress, lps_i2c_read_t read_func, lps_i2c_write_t write_func);
int lps_configure(lps_data *dev, uint8_t ctrl1_val);
int lps_set_calibration(lps_data *dev, calibration *cal);
int lps_read_pressure(lps_data *dev, float *pressure);
int lps_read_temparature(lps_data *dev, float *temparature);
int lps_calc_altitude(lps_data *dev, double *altitude);
uint32_t get_my_time();
#endif /* LPS25HB_H */
