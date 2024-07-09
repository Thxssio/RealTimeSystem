#ifndef PERIODIC_TASK_THREAD_H
#define PERIODIC_TASK_THREAD_H

#include <linux/i2c-dev.h>
#include <pthread.h>
#include "shared_data_types.h"

// Endereços I2C e configurações para o MPU9250
#define I2C_DEVICE "/dev/i2c-1"
#define MPU9250_ADDRESS 0x68
#define PWR_MGMT_1 0x6B
#define CONFIG 0x1A
#define SMPLRT_DIV 0x19
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define MAG_XOUT_L 0x03

// Declarações de funções
void* thread_periodic_task(void* arg);
int init_mpu9250(int fd);
int read_mpu9250_data(int fd, mpu9250_data_t* data);

#endif
