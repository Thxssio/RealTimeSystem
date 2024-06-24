#ifndef UTILS_H
#define UTILS_H

#include "nmea_types.h"

void set_thread_name(const char* name);
void log_with_timestamp(const char* message);
double convertToDecimalDegrees(double nmeaValue, char direction);
void nmea_parse_gpgga(char *nmea, gpgga_t *loc);
void nmea_parse_gprmc(char *nmea, gprmc_t *loc);
int nmea_get_message_type(const char *message);
const char* get_signal_quality_description(int quality);
int isDeviceConnected(const char* devicePath);

#endif // UTILS_H
