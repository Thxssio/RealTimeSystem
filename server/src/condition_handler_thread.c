#include "include/condition_handler_thread.h"
#include "include/utils.h"
#include "include/gps.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>  // Adicionar este include

extern volatile sig_atomic_t keep_running;  // Declarar keep_running como externa

void* thread_condition_handler(void* arg) {
    set_thread_name("Cond_Handler");
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    while (keep_running) {
        pthread_mutex_lock(&shared_data->data_mutex);
        while (!shared_data->data_ready && keep_running) {
            pthread_cond_wait(&shared_data->data_cond, &shared_data->data_mutex);
        }
        if (!keep_running) {
            pthread_mutex_unlock(&shared_data->data_mutex);
            break;
        }
        if (shared_data->gpgga_data.latitude == 0.0 && shared_data->gpgga_data.longitude == 0.0) {
            log_with_timestamp("Obtendo dados do GPS...");
        } else {
            printf("Latitude: %f, Longitude: %f, Altitude: %f\n",
                   shared_data->gpgga_data.latitude, shared_data->gpgga_data.longitude, shared_data->gpgga_data.altitude);
            printf("Qualidade do Sinal: %d (%s), Satélites: %d\n",
                   shared_data->gpgga_data.quality, get_signal_quality_description(shared_data->gpgga_data.quality), shared_data->gpgga_data.satellites);
        }
        shared_data->data_ready = 0;
        pthread_mutex_unlock(&shared_data->data_mutex);
        sleep(1);
    }
    return NULL;
}
