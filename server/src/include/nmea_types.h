#ifndef NMEA_TYPES_H
#define NMEA_TYPES_H

#define NMEA_GPGGA 1
#define NMEA_GPRMC 2
#define NMEA_GSV 3

#define MAX_SATELLITES 12

typedef struct {
    double latitude;
    double longitude;
    double altitude;
    int quality;
    int satellites;
} gpgga_t;

typedef struct {
    double latitude;
    double longitude;
    double speed;
    double course;
} gprmc_t;

typedef struct {
    int prn;
    int elevation;
    int azimuth;
    int snr;
} SatelliteInfo;

typedef struct {
    int num_messages;
    int message_number;
    int num_satellites;
    SatelliteInfo satellites[MAX_SATELLITES];
} GSVData;

#endif // NMEA_TYPES_H
