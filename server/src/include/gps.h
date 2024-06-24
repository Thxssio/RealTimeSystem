#ifndef GPS_H
#define GPS_H

#include <termios.h>
#include <pthread.h>
#include <stdbool.h>
#include "nmea_types.h"

typedef struct {
    gpgga_t gpgga_data;
    gprmc_t gprmc_data;
    GSVData gsv_data;
    pthread_mutex_t data_mutex;
    pthread_cond_t data_cond;
    pthread_cond_t connection_cond;
    int data_ready;
    bool gps_connected;
} gps_shared_data_t;

int setup_gps_device(const char *device_path, speed_t baud_rate);
int read_gps_data(int device, gpgga_t *gpgga_data, gprmc_t *gprmc_data, GSVData *gsv_data);

#endif // GPS_H
