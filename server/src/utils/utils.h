#ifndef UTILS_H
#define UTILS_H

#include "gps/gps.h"

#define _EMPTY 0x00
#define NMEA_GPRMC 0x01
#define NMEA_GPRMC_STR "$GPRMC"
#define NMEA_GPGGA 0x02
#define NMEA_GPGGA_STR "GPGGA"
#define NMEA_UNKNOWN 0x00
#define _COMPLETED 0x03

#define NMEA_CHECKSUM_ERR 0x80
#define NMEA_MESSAGE_ERR 0xC0

void nmea_parse_gpgga(char *nmea, gpgga_t *loc);
void nmea_parse_gprmc(char *nmea, gprmc_t *loc);
int nmea_valid_checksum(const char *message);
int nmea_get_message_type(const char *message);

#endif
