#include "lps25hb.h"

int main(){
    
    lps_data sensor;

    sensor.get_tick = get_my_time;

}


int lps_init(lps_data *dev, uint8_t adress, lps_i2c_read_t read_func, lps_i2c_write_t write_func) {
    if (dev == NULL || read_func == NULL || write_func == NULL) {
        return -1;
    }

    dev->i2c_adress = adress; 
    dev->i2c_read = read_func;
    dev->i2c_write = write_func;

    uint8_t who_am_i = 0;
    if (dev->i2c_read(dev->i2c_adress, LPS25_WHOAMI_ADDR, &who_am_i, 1) != 0) {
        return -1; 
    }

    if (who_am_i != LPS25_WHOAMI_VALUE) { 
        return -2;
    }

    return 0;
}

int lps_configure(lps_data *dev, uint8_t ctrl1_val) {
    if (dev == NULL || dev->i2c_write == NULL) {
        return -1;
    }
    
    return dev->i2c_write(dev->i2c_adress, LPS25_REG_CTRL1, &ctrl1_val, 1);
}

int lps_read_pressure(lps_data *dev, float *pressure) {
    if (dev == NULL || dev->i2c_read == NULL || pressure == NULL) return -1;

    uint8_t buffer[3];
    uint8_t start_addr = LPS25_REG_PRESS_OUT_XL | LPS25_AUTO_INCREMENT;

    if (dev->i2c_read(dev->i2c_adress, start_addr, buffer, 3) != 0) return -2;
    uint32_t raw_pressure = ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[1] << 8)  | (uint32_t)buffer[0];

    *pressure = (float)raw_pressure / 4096.0f;
    return 0;
}

/*#define LPS25_REG_TEMP_OUT_L    0x2B
#define LPS25_REG_TEMP_OUT_H    0x2C*/
int lps_read_temparature(lps_data *dev, float *temparature){
    if(dev == NULL || temparature == NULL || dev->i2c_read == NULL) return -1;

    uint8_t buffer[2]; // baytlar her zaman unsigned olarak tanımlanmalıdır!

    uint8_t start_addr = LPS25_REG_TEMP_OUT_L | LPS25_AUTO_INCREMENT;

    if(dev->i2c_read(dev->i2c_adress,start_addr,buffer,2) != 0) return -2;
    
    int16_t raw_temparature = ((int16_t)buffer[1] << 8) | ((int16_t)buffer[0]);

    *temparature = 42.5f + ((float)raw_temparature/ 480.0f);
    return 0;
}

#pragma GCC optimize ("O3") // this actually matters when reached the apogee.
int lps_calc_altitude(lps_data *dev,double *altitude){

    if(dev==NULL || dev->i2c_read == NULL || altitude == NULL) return -1; // function problem

    float sealevelPO = 1013.25f; // sea-level pressure
    float pressure;

    if(!lps_read_pressure(dev,&pressure)){
        *altitude = 44330.0f* (1-(powf(((pressure/sealevelPO)),0.1903f)));
        return 0;
    }else{ 
        return -2; // pressure problem
    }
}



int lps_set_calibration(lps_data *dev, calibration *c){

    if(dev->i2c_read == NULL || dev->i2c_write == NULL) return -1; /*before calibration check the system with ISR*/
    

    double altitude;
    float pressure;


    /*float pressure_rate = 50;
    float pressuresum;

    #pragma GCC unroll 10
    for (uint8_t i = 0; i < 50; i++) // read it 50 times to get absoloute pressure 
    {
        if(lps_read_pressure(dev,&pressure)) return -2; // to prevent segfault

        pressuresum += pressure;
    }
        if not wanted sea-level-pressure to calc. then use this.
    */
    
    float temp;
    if(!lps_calc_altitude(dev,&altitude) && !lps_read_pressure(dev,&pressure) && !lps_read_temparature(dev,&temp)){ /*if successfully chip could got data so;*/
        
        if(dev->get_tick != NULL) c->timestamp = dev->get_tick();

        else c->timestamp = 0;

        c->id = counter;
        counter++;
        c->altitude = altitude;
        c->pressure = pressure;
        c->temparature = temp;
        return 0;
    }
    return -3;   
}

void system_time_handler(){
    system_timer++;// per 1 ms / ++
}

uint32_t get_my_time(){
    return  system_timer;
}

/*#pragma warnings(disable:4996)
void __attribute__((interrupt)) ERRHANDLER(){

}
    if abruptly needs the routine check use the func. 
*/

