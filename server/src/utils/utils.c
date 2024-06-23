#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

void nmea_parse_gpgga(char *nmea, gpgga_t *loc) {   
    char *p = nmea;
    
    p = strchr(p, ',')+1; //skip time
    
    p = strchr(p, ',')+1;
    loc->latitude = atof(p);
    
    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'N':
            loc->lat = 'N';
            break;
        case 'S':
            loc->lat = 'S';
            break;
        case ',':
            loc->lat = '\0';
            break;
    }
    
    p = strchr(p, ',')+1;
    loc->longitude = atof(p);
    
    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'W':
            loc->lon = 'W';
            break;
        case 'E':
            loc->lon = 'E';
            break;
        case ',':
            loc->lon = '\0';
            break;
    }
    
    p = strchr(p, ',')+1;
    loc->quality = (int)atoi(p);
    
    p = strchr(p, ',')+1;
    loc->satellites = (int)atoi(p);
    
    p = strchr(p, ',')+1;
    
    p = strchr(p, ',')+1;
    loc->altitude = atof(p);

    // Converta latitude e longitude de graus e minutos para graus decimais
    double lat_deg = (int)(loc->latitude / 100);
    double lat_min = loc->latitude - (lat_deg * 100);
    loc->latitude = lat_deg + (lat_min / 60.0);
    if (loc->lat == 'S') {
        loc->latitude *= -1.0;
    }

    double lon_deg = (int)(loc->longitude / 100);
    double lon_min = loc->longitude - (lon_deg * 100);
    loc->longitude = lon_deg + (lon_min / 60.0);
    if (loc->lon == 'W') {
        loc->longitude *= -1.0;
    }
}

void nmea_parse_gprmc(char *nmea, gprmc_t *loc) {   
    char *p = nmea;
    
    p = strchr(p, ',')+1; //skip time
    p = strchr(p, ',')+1; //skip status
    
    p = strchr(p, ',')+1;
    loc->latitude = atof(p);
    
    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'N':
            loc->lat = 'N';
            break;
        case 'S':
            loc->lat = 'S';
            break;
        case ',':
            loc->lat = '\0';
            break;
    }
    
    p = strchr(p, ',')+1;
    loc->longitude = atof(p);
    
    p = strchr(p, ',')+1;
    switch (p[0]) {
        case 'W':
            loc->lon = 'W';
            break;
        case 'E':
            loc->lon = 'E';
            break;
        case ',':
            loc->lon = '\0';
            break;
    }
    
    p = strchr(p, ',')+1;
    loc->speed = atof(p);
    
    p = strchr(p, ',')+1;
    loc->course = atof(p);
}

int nmea_valid_checksum(const char *message) {
    int checksum = (int)strtol(strchr(message, '*')+1, NULL, 16);

    char p;
    int sum = 0;
    ++message;
    while ((p = *message++) != '*') {
        sum ^= p;
    }

    if (sum != checksum) {
        return NMEA_CHECKSUM_ERR;
    }

    return _EMPTY;
}

int nmea_get_message_type(const char *message) {
    int checksum = 0;
    if ((checksum = nmea_valid_checksum(message)) != _EMPTY) {
        return checksum;
    }

    if (strstr(message, NMEA_GPGGA_STR) != NULL) {
        return NMEA_GPGGA;
    }

    if (strstr(message, NMEA_GPRMC_STR) != NULL) {
        return NMEA_GPRMC;
    }

    return NMEA_UNKNOWN;
}
