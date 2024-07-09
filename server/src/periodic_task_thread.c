#include "periodic_task_thread.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "include/gps.h"

extern volatile sig_atomic_t keep_running;

// Inicializa o MPU9250
int init_mpu9250(int fd) {
    if (ioctl(fd, I2C_SLAVE, MPU9250_ADDRESS) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.");
        return -1;
    }

    // Despertar o MPU9250
    char config[2] = {PWR_MGMT_1, 0x00};
    if (write(fd, config, 2) != 2) {
        perror("Failed to wake up MPU9250");
        return -1;
    }

    return 0;
}

// Lê dados do MPU9250
int read_mpu9250_data(int fd, mpu9250_data_t* data) {
    unsigned char buf[14];
    buf[0] = ACCEL_XOUT_H;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to the I2C bus");
        return -1;
    }

    if (read(fd, buf, 14) != 14) {
        perror("Failed to read from the I2C bus");
        return -1;
    }

    // Conversão dos dados brutos para valores significativos
    data->accel_x = (float)((buf[0] << 8) | buf[1]) / 16384.0;  // Exemplo de conversão
    data->accel_y = (float)((buf[2] << 8) | buf[3]) / 16384.0;
    data->accel_z = (float)((buf[4] << 8) | buf[5]) / 16384.0;
    data->gyro_x = (float)((buf[8] << 8) | buf[9]) / 131.0;
    data->gyro_y = (float)((buf[10] << 8) | buf[11]) / 131.0;
    data->gyro_z = (float)((buf[12] << 8) | buf[13]) / 131.0;

    // Se houver um magnetômetro, incluir a leitura e a conversão aqui

    return 0;
}

void* thread_periodic_task(void* arg) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    int fd = open(I2C_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the I2C bus");
        return NULL;
    }

    if (init_mpu9250(fd) < 0) {
        close(fd);
        return NULL;
    }

    mpu9250_data_t local_mpu9250_data;

    while (keep_running) {
        if (read_mpu9250_data(fd, &local_mpu9250_data) == 0) {
            pthread_mutex_lock(&shared_data->data_mutex);
            shared_data->mpu9250_data = local_mpu9250_data;
            pthread_mutex_unlock(&shared_data->data_mutex);
        }
        sleep(1);
    }

    close(fd);
    return NULL;
}