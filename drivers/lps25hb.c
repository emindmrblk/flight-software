#ifndef LPS25HB_H
#define LPS25HB_H

#include <stdint.h>
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

/* --- TEMPERATURE OUTPUT REGISTERS (16-bit) --- */
#define LPS25_REG_TEMP_OUT_L    0x2B
#define LPS25_REG_TEMP_OUT_H    0x2C

/* --- CALIBRATION / OFFSET REGISTERS --- */
#define LPS25_REG_RPDS_L        0x39
#define LPS25_REG_RPDS_H        0x3A

/* --- ST MULTI-BYTE READ MASK (Auto-Increment) --- */
#define LPS25_AUTO_INCREMENT    0x80

/* --- CTRL_REG1 BIT MASKS --- */
#define LPS25_CTRL1_PD          (1 << 7)  // Power down active (1 = aktif)
#define LPS25_CTRL1_ODR_12_5HZ  (3 << 4)  // Örnekleme hızı
#define LPS25_CTRL1_BDU          (1 << 2)  // Block data update

/* Fonksiyon İşaretçisi (Pointer) Tipleri */
typedef int (*lps_i2c_read_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef int (*lps_i2c_write_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

typedef struct
{
    uint8_t i2c_adress;
    lps_i2c_read_t i2c_read;
    lps_i2c_write_t i2c_write;
} lps_data;

/* Fonksiyon Prototipleri */
int lps_init(lps_data *dev, uint8_t adress, lps_i2c_read_t read_func, lps_i2c_write_t write_func);
int lps_configure(lps_data *dev, uint8_t ctrl1_val);
int lps_set_calibration(lps_data *dev, uint16_t calibration_value);
int lps_read_pressure(lps_data *dev, float *pressure);
int lps_read_temparature(lps_data *dev, float *temparature);
int lps_calc_altidute(lps_data *dev);

#endif /* LPS25HB_H */


/* --- C DOSYASI / FONKSİYON IMPLEMENTASYONLARI --- */

int lps_init(lps_data *dev, uint8_t adress, lps_i2c_read_t read_func, lps_i2c_write_t write_func) {
    if (dev == NULL || read_func == NULL || write_func == NULL) {
        return -1;
    }

    dev->i2c_adress = adress; 
    dev->i2c_read = read_func;
    dev->i2c_write = write_func;

    // 1. WHO_AM_I Kontrolü (Sensör hatta mı?)
    uint8_t who_am_i = 0;
    if (dev->i2c_read(dev->i2c_adress, LPS25_WHOAMI_ADDR, &who_am_i, 1) != 0) {
        return -1; // I2C İletişim Hatası
    }

    if (who_am_i != LPS25_WHOAMI_VALUE) { 
        return -2; // Yanlış Cihaz / Sensör Yanıt Vermiyor
    }

    return 0; // Başarılı
}

int lps_configure(lps_data *dev, uint8_t ctrl1_val) {
    if (dev == NULL || dev->i2c_write == NULL) {
        return -1;
    }

    // CTRL_REG1 (0x20) adresine 8-bitlik konfigürasyon değerini yaz
    return dev->i2c_write(dev->i2c_adress, LPS25_REG_CTRL1, &ctrl1_val, 1);
}