#include "include/gps.h"
#include "include/utils.h"
#include "include/threads.h"
#include "include/gps_thread.h"
#include "include/periodic_task_thread.h"
#include "include/network_communication_thread.h"
#include "include/condition_handler_thread.h"
#include "include/device_check_thread.h"
#include "include/http_server_thread.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

volatile sig_atomic_t keep_running = 1;

void int_handler(int dummy) {
    keep_running = 0;
}

int main() {
    signal(SIGINT, int_handler);

    gps_shared_data_t shared_data;
    pthread_mutex_init(&shared_data.data_mutex, NULL);
    pthread_cond_init(&shared_data.data_cond, NULL);
    pthread_cond_init(&shared_data.connection_cond, NULL);
    shared_data.data_ready = 0;
    shared_data.gps_connected = false;

    pthread_t thread1, thread2, thread3, thread4, thread5, thread6;

    pthread_create(&thread1, NULL, thread_gps_capture, &shared_data);
    pthread_create(&thread2, NULL, thread_periodic_task, &shared_data);
    pthread_create(&thread3, NULL, thread_network_communication, &shared_data);
    pthread_create(&thread4, NULL, thread_condition_handler, &shared_data);
    pthread_create(&thread5, NULL, thread_check_device_connection, &shared_data);
    pthread_create(&thread6, NULL, thread_http_server, &shared_data);

    while (keep_running) {
        sleep(1);
    }

    log_with_timestamp("Encerrando o programa...");

    pthread_cancel(thread1);
    pthread_cancel(thread2);
    pthread_cancel(thread3);
    pthread_cancel(thread4);
    pthread_cancel(thread5);
    pthread_cancel(thread6);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
    pthread_join(thread6, NULL);

    pthread_mutex_destroy(&shared_data.data_mutex);
    pthread_cond_destroy(&shared_data.data_cond);
    pthread_cond_destroy(&shared_data.connection_cond);

    return 0;
}
