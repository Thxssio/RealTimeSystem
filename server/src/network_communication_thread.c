#include "include/network_communication_thread.h"
#include "include/utils.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>  // Adicionar este include


extern volatile sig_atomic_t keep_running;

void* thread_network_communication(void* arg) {
    set_thread_name("Network_Comm");
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
        printf("Enviando dados: Latitude: %f, Longitude: %f, Altitude: %f\n",
               shared_data->gpgga_data.latitude, shared_data->gpgga_data.longitude, shared_data->gpgga_data.altitude);
        shared_data->data_ready = 0;
        pthread_mutex_unlock(&shared_data->data_mutex);
    }
    return NULL;
}
