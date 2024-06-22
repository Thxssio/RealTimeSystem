#ifndef GPS_H
#define GPS_H

#include <termios.h> // Para speed_t

#define BAUDRATE_GPS B19200

typedef struct gpgga {
    double latitude;
    char lat;
    double longitude;
    char lon;
    int quality;
    int satellites;
    double altitude;
} gpgga_t;

typedef struct gprmc {
    double latitude;
    char lat;
    double longitude;
    char lon;
    double speed;
    double course;
} gprmc_t;

int setup_gps_device(const char *device_path, speed_t baud_rate);
int read_gps_data(int device, gpgga_t *gpgga_data, gprmc_t *gprmc_data);

#endif
