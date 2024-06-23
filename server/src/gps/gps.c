#include "gps.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include "utils/utils.h"

int setup_gps_device(const char *device_path, speed_t baud_rate) {
    int device = open(device_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (device == -1) {
        perror("Erro ao abrir o dispositivo");
        return -1;
    }

    struct termios options;
    tcgetattr(device, &options);
    cfsetospeed(&options, baud_rate);
    cfsetispeed(&options, baud_rate);
    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    options.c_iflag &= ~IGNBRK;         // Disable break processing
    options.c_lflag = 0;                // No signaling chars, no echo,
                                        // no canonical processing
    options.c_oflag = 0;                // No remapping, no delays
    options.c_cc[VMIN]  = 1;            // Read at least 1 character
    options.c_cc[VTIME] = 0;            // No timeout

    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Shut off xon/xoff ctrl
    options.c_cflag |= (CLOCAL | CREAD);// Ignore modem controls,
                                        // Enable reading
    options.c_cflag &= ~(PARENB | PARODD);      // Shut off parity
    options.c_cflag |= 0;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;

    tcsetattr(device, TCSANOW, &options);
    return device;
}

int read_gps_data(int device, gpgga_t *gpgga_data, gprmc_t *gprmc_data) {
    char lineMsg[250], c;
    int res = 0, posicao = 0;

    // Fica na espera ate ter uma msg do GPS
    memset(lineMsg, '\0', 250);
    posicao = 0;
    while (1) {
        res = read(device, &c, 1); 
        if (res == 1) {
            lineMsg[posicao] = c;
            posicao++;
            if (c == '\n') {
                break;
            } 
        } else if (res == 0) {
            continue;
        } else {
            continue;
        }
    }

    if (lineMsg[0] == '$') {
        res = nmea_get_message_type(lineMsg);
        if (res == NMEA_GPGGA) {
            nmea_parse_gpgga(lineMsg, gpgga_data);
            return 1;
        } else if (res == NMEA_GPRMC) {
            nmea_parse_gprmc(lineMsg, gprmc_data);
        }
    }
    return 0;
}
