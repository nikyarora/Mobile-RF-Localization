#ifndef MPU9250_H
#define MPU9205_H

#include <stdint.h>

class mpu9250 {
public:
    mpu9250() {};
    uint8_t open(uint8_t rate=200);
    
    uint8_t update();
    uint32_t *getQuarternion() {return _q;}
    int16_t *getGyros() {return _gyro;}
private:
    int16_t _gyro[3];
    uint32_t _q[4];
};

#endif

