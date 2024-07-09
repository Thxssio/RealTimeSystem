// mpu9250_types.h
#ifndef MPU9250_TYPES_H
#define MPU9250_TYPES_H

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float mag_x;
    float mag_y;
    float mag_z;
} mpu9250_data_t;

#endif // MPU9250_TYPES_H
