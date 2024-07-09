#ifndef SHARED_DATA_TYPES_H
#define SHARED_DATA_TYPES_H

#include "nmea_types.h"      
#include "mpu9250_types.h"  

typedef struct {
    gpgga_t gpgga_data;
    gprmc_t gprmc_data;
    GSVData gsv_data;
    mpu9250_data_t mpu9250_data;  
} shared_data_t;

#endif
