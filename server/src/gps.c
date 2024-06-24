#include "include/gps.h"
#include "include/utils.h"
#include "include/gps_satellites.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int setup_gps_device(const char *device_path, speed_t baud_rate) {
    int device = open(device_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (device < 0) {
        perror("Erro ao abrir o dispositivo");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(device, &tty) != 0) {
        perror("Erro ao obter atributos do dispositivo");
        close(device);
        return -1;
    }

    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 1;            // read blocks
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag &= ~CSTOPB;

    if (tcsetattr(device, TCSANOW, &tty) != 0) {
        perror("Erro ao definir atributos do dispositivo");
        close(device);
        return -1;
    }

    return device;
}

int read_gps_data(int device, gpgga_t *gpgga_data, gprmc_t *gprmc_data, GSVData *gsv_data) {
    char buffer[1024];
    int bytes_read;
    char *lineMsg;
    int res;

    while ((bytes_read = read(device, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        lineMsg = strtok(buffer, "\r\n");
        while (lineMsg != NULL) {
            res = nmea_get_message_type(lineMsg);
            if (res == NMEA_GPGGA) {
                nmea_parse_gpgga(lineMsg, gpgga_data);
                log_with_timestamp("GPGGA data parsed.");
            } else if (res == NMEA_GPRMC) {
                nmea_parse_gprmc(lineMsg, gprmc_data);
                log_with_timestamp("GPRMC data parsed.");
            } else if (res == NMEA_GSV) {
                parseGSV(lineMsg, gsv_data);
                log_with_timestamp("GSV data parsed.");
            }
            lineMsg = strtok(NULL, "\r\n");
        }
    }

    if (bytes_read < 0) {
        perror("Erro ao ler dados do dispositivo");
        return -1;
    }

    return 0;
}
