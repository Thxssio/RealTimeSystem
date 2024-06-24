#include "include/utils.h"
#include <stdio.h>
#include <time.h>
#include <sys/prctl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>  // Adicionar este include para atof e atoi

void set_thread_name(const char* name) {
#ifdef __linux__
    prctl(PR_SET_NAME, name, 0, 0, 0);
#else
    pthread_setname_np(pthread_self(), name);
#endif
}

void log_with_timestamp(const char* message) {
    time_t now;
    time(&now);
    struct tm* timeinfo = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    //printf("[%s] %s\n", time_str, message);
}

double convertToDecimalDegrees(double nmeaValue, char direction) {
    int degrees = (int)(nmeaValue / 100);
    double minutes = nmeaValue - degrees * 100;
    double decimal = degrees + minutes / 60;

    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    return decimal;
}
void nmea_parse_gpgga(char *nmea, gpgga_t *loc) {
    char *token = strtok(nmea, ",");
    int field_count = 0;
    char latDirection, lonDirection;

    while (token != NULL) {
        switch (field_count) {
            case 2: // Latitude value
                loc->latitude = atof(token);
                break;
            case 3: // Latitude direction
                latDirection = token[0];
                break;
            case 4: // Longitude value
                loc->longitude = atof(token);
                break;
            case 5: // Longitude direction
                lonDirection = token[0];
                break;
            case 6: // Quality of GPS fix
                loc->quality = atoi(token);
                break;
            case 7: // Number of satellites
                loc->satellites = atoi(token);
                break;
            case 9: // Altitude
                loc->altitude = atof(token);
                break;
            default:
                break;
        }
        token = strtok(NULL, ",");
        field_count++;
    }

    // Apply conversion with direction
    loc->latitude = convertToDecimalDegrees(loc->latitude, latDirection);
    loc->longitude = convertToDecimalDegrees(loc->longitude, lonDirection);

    printf("Latitude: %f, Direction: %c\n", loc->latitude, latDirection);
    printf("Longitude: %f, Direction: %c\n", loc->longitude, lonDirection);
    printf("Quality: %d\n", loc->quality);
    printf("Satellites: %d\n", loc->satellites);
    printf("Altitude: %f\n", loc->altitude);
}

void nmea_parse_gprmc(char *nmea, gprmc_t *loc) {
    char *token = strtok(nmea, ",");
    int field_count = 0;
    char latDirection, lonDirection;

    while (token != NULL) {
        switch (field_count) {
            case 3: // Latitude value
                loc->latitude = atof(token);
                break;
            case 4: // Latitude direction
                latDirection = token[0];
                break;
            case 5: // Longitude value
                loc->longitude = atof(token);
                break;
            case 6: // Longitude direction
                lonDirection = token[0];
                break;
            case 7: // Speed
                loc->speed = atof(token);
                break;
            case 8: // Course
                loc->course = atof(token);
                break;
            default:
                break;
        }
        token = strtok(NULL, ",");
        field_count++;
    }

    // Apply conversion with direction
    loc->latitude = convertToDecimalDegrees(loc->latitude, latDirection);
    loc->longitude = convertToDecimalDegrees(loc->longitude, lonDirection);
}


int nmea_get_message_type(const char *message) {
    if (strstr(message, "$GPGGA")) {
        return NMEA_GPGGA;
    } else if (strstr(message, "$GPRMC")) {
        return NMEA_GPRMC;
    } else if (strstr(message, "$GPGSV")) {
        return NMEA_GSV;
    }
    return 0;
}

const char* get_signal_quality_description(int quality) {
    switch (quality) {
        case 0: return "Fixação inválida";
        case 1: return "Fixação de GPS padrão (2D/3D)";
        case 2: return "Fixação diferencial GPS (DGPS)";
        case 3: return "PPS fix";
        case 4: return "Fixação RTK";
        case 5: return "Float RTK";
        case 6: return "Estimativa baseada em dados antigos";
        case 7: return "Modo de entrada manual";
        case 8: return "Fixação simulada";
        default: return "Desconhecida";
    }
}

int isDeviceConnected(const char* devicePath) {
    return access(devicePath, F_OK) != -1;
}
