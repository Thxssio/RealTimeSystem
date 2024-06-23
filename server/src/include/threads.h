#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>

void* thread_gps_capture(void* arg);
void* thread_periodic_task(void* arg);
void* thread_network_communication(void* arg);
void* thread_condition_handler(void* arg);
void* thread_check_device_connection(void* arg);
void* thread_http_server(void* arg);

#endif // THREADS_H