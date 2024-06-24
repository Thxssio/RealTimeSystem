#include "include/gps_satellites.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_SATELLITES 12

void parseGSV(const char *nmea, GSVData *gsv_data) {
    char *token;
    int field_count = 0;
    int satIndex = 0;

    char nmea_copy[256];
    strncpy(nmea_copy, nmea, sizeof(nmea_copy));
    nmea_copy[sizeof(nmea_copy) - 1] = '\0';  // Garantir que a string está terminada em null

    token = strtok(nmea_copy, ",");

    while (token != NULL) {
        if (field_count == 3) {
            gsv_data->num_satellites = atoi(token);
        } else if (field_count >= 4 && (field_count - 4) % 4 == 0 && satIndex < MAX_SATELLITES) {
            int id = atoi(token);
            if (id < 1 || id > 32) {  // Verificar se o PRN é válido
                printf("PRN inválido: %d\n", id);
                id = 0;
            }
            gsv_data->satellites[satIndex].prn = id;
        } else if ((field_count - 4) % 4 == 1 && satIndex < MAX_SATELLITES) {
            int elevation = atoi(token);
            if (elevation < 0 || elevation > 90) {  // Verificar se a elevação é válida
                printf("Elevação inválida: %d\n", elevation);
                elevation = 0;
            }
            gsv_data->satellites[satIndex].elevation = elevation;
        } else if ((field_count - 4) % 4 == 2 && satIndex < MAX_SATELLITES) {
            int azimuth = atoi(token);
            if (azimuth < 0 || azimuth > 360) {  // Verificar se o azimute é válido
                printf("Azimute inválido: %d\n", azimuth);
                azimuth = 0;
            }
            gsv_data->satellites[satIndex].azimuth = azimuth;
        } else if ((field_count - 4) % 4 == 3 && satIndex < MAX_SATELLITES) {
            int snr = atoi(token);
            if (snr < 0 || snr > 99) {  // Verificar se o SNR é válido
                printf("SNR inválido: %d\n", snr);
                snr = 0;
            }
            gsv_data->satellites[satIndex].snr = snr;
            satIndex++;
        }
        token = strtok(NULL, ",");
        field_count++;
    }

    // Ajustar número de satélites detectados
    gsv_data->num_satellites = satIndex;
}
