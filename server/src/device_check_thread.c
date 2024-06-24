#include "include/device_check_thread.h"
#include "include/utils.h"
#include "include/gps.h"
#include <unistd.h>
#include <pthread.h>
#include <signal.h>  // Adicionar este include

#define DEVICE "/dev/ublox_gps"
extern volatile sig_atomic_t keep_running;  // Declarar keep_running como externa

void* thread_check_device_connection(void* arg) {
    set_thread_name("Device_Check");
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    const char* devicePath = DEVICE;

    while (keep_running) {
        pthread_mutex_lock(&shared_data->data_mutex);
        if (isDeviceConnected(devicePath)) {
            if (!shared_data->gps_connected) {
                log_with_timestamp("O dispositivo está conectado.");
                shared_data->gps_connected = true;
                pthread_cond_signal(&shared_data->connection_cond);
            }
        } else {
            if (shared_data->gps_connected) {
                log_with_timestamp("O dispositivo não está conectado.");
                shared_data->gps_connected = false;
                pthread_cond_signal(&shared_data->connection_cond);
            }
        }
        pthread_mutex_unlock(&shared_data->data_mutex);
        sleep(0.5);
    }
    return NULL;
}
