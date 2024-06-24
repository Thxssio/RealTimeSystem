#include "include/gps_thread.h"
#include "include/utils.h"
#include "include/gps.h"
#include "include/gps_satellites.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/prctl.h>
#include <signal.h>

#define DEVICE "/dev/ublox_gps"
extern volatile sig_atomic_t keep_running;  // Declarar keep_running como externa


void* thread_gps_capture(void* arg) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;

    while (keep_running) {
        log_with_timestamp("Tentando abrir o dispositivo GPS...");
        int device = setup_gps_device(DEVICE, B19200);
        if (device < 0) {
            log_with_timestamp("Erro ao abrir o dispositivo GPS, tentando novamente...");
            continue;
        }
        log_with_timestamp("Dispositivo GPS conectado.");

        while (shared_data->gps_connected && keep_running) {
            if (read_gps_data(device, &shared_data->gpgga_data, &shared_data->gprmc_data, &shared_data->gsv_data)) {
                log_with_timestamp("Dados do GPS lidos com sucesso.");
                pthread_mutex_lock(&shared_data->data_mutex);
                // Atualiza os dados aqui
                pthread_mutex_unlock(&shared_data->data_mutex);
            } else {
                log_with_timestamp("Falha ao ler dados do GPS.");
            }
            sleep(1);
        }

        log_with_timestamp("Fechando o dispositivo GPS...");
        close(device);
        log_with_timestamp("Dispositivo GPS fechado, tentando reconectar...");
        sleep(5);
    }
    return NULL;
}