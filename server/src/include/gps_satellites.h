#ifndef GPS_SATELLITES_H
#define GPS_SATELLITES_H

#include "nmea_types.h"

void parseGSV(const char *nmea, GSVData *gsvData);

#endif // GPS_SATELLITES_H
