#include <microhttpd.h>
#include "gps/gps.h"
#include "utils/utils.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/prctl.h>
#include <time.h>
#include <signal.h>
#include <json-c/json.h>
#include <errno.h>
#include "include/threads.h"
#define DEVICE "/dev/ublox_gps"
#define PORT 8888

typedef struct {
    gpgga_t gpgga_data;
    gprmc_t gprmc_data;
    pthread_mutex_t data_mutex;
    pthread_cond_t data_cond;
    pthread_cond_t connection_cond;
    int data_ready;
    bool gps_connected;
} gps_shared_data_t;

gps_shared_data_t shared_data = {
    .data_mutex = PTHREAD_MUTEX_INITIALIZER,
    .data_cond = PTHREAD_COND_INITIALIZER,
    .connection_cond = PTHREAD_COND_INITIALIZER,
    .data_ready = 0,
    .gps_connected = false
};

void set_thread_name(const char* name) {
#ifdef __linux__
    prctl(PR_SET_NAME, name, 0, 0, 0);
#else
    pthread_setname_np(pthread_self(), name);
#endif
}

int isDeviceConnected(const char* devicePath) {
    return access(devicePath, F_OK) != -1;
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

void log_with_timestamp(const char* message) {
    time_t now;
    time(&now);
    struct tm* timeinfo = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("[%s] %s\n", time_str, message);
}

volatile sig_atomic_t keep_running = 1;

void int_handler(int dummy) {
    keep_running = 0;
}

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection, const char* url,
                                const char* method, const char* version, const char* upload_data,
                                size_t* upload_data_size, void** con_cls) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)cls;
    struct json_object* jobj = json_object_new_object();

    pthread_mutex_lock(&shared_data->data_mutex);
    json_object_object_add(jobj, "latitude", json_object_new_double(shared_data->gpgga_data.latitude));
    json_object_object_add(jobj, "longitude", json_object_new_double(shared_data->gpgga_data.longitude));
    json_object_object_add(jobj, "altitude", json_object_new_double(shared_data->gpgga_data.altitude));
    json_object_object_add(jobj, "quality", json_object_new_int(shared_data->gpgga_data.quality));
    json_object_object_add(jobj, "satellites", json_object_new_int(shared_data->gpgga_data.satellites));
    json_object_object_add(jobj, "status", json_object_new_string(shared_data->gps_connected ? "connected" : "disconnected"));
    pthread_mutex_unlock(&shared_data->data_mutex);

    const char* json_str = json_object_to_json_string(jobj);
    struct MHD_Response* response = MHD_create_response_from_buffer(strlen(json_str), (void*)json_str, MHD_RESPMEM_MUST_COPY);

    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    json_object_put(jobj);

    return ret;
}

void* thread_http_server(void* arg) {
    set_thread_name("HTTP_Server");
    log_with_timestamp("Iniciando servidor HTTP...");
    struct MHD_Daemon* daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &request_handler, arg, MHD_OPTION_END);
    if (NULL == daemon) {
        log_with_timestamp("Falha ao iniciar o servidor HTTP.");
        return NULL;
    }
    log_with_timestamp("Servidor HTTP iniciado.");

    while (keep_running) {
        sleep(1);
    }

    MHD_stop_daemon(daemon);
    log_with_timestamp("Servidor HTTP encerrado.");
    return NULL;
}

int main() {
    signal(SIGINT, int_handler);  // Captura Ctrl+C

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

    return 0;
}

void* thread_gps_capture(void* arg) {
    set_thread_name("GPS_Capture");
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;

    while (keep_running) {
        pthread_mutex_lock(&shared_data->data_mutex);
        while (!shared_data->gps_connected && keep_running) {
            log_with_timestamp("Aguardando conexão do dispositivo GPS...");
            pthread_cond_wait(&shared_data->connection_cond, &shared_data->data_mutex);
        }
        pthread_mutex_unlock(&shared_data->data_mutex);

        if (!keep_running) break;

        int device = setup_gps_device(DEVICE, B19200);
        if (device < 0) {
            log_with_timestamp("Erro ao abrir o dispositivo GPS.");
            pthread_mutex_lock(&shared_data->data_mutex);
            shared_data->gps_connected = false;
            pthread_mutex_unlock(&shared_data->data_mutex);
            continue;
        }
        log_with_timestamp("Dispositivo GPS conectado.");

        while (shared_data->gps_connected && keep_running) {
            pthread_mutex_lock(&shared_data->data_mutex);
            if (!shared_data->gps_connected) {
                close(device);
                pthread_mutex_unlock(&shared_data->data_mutex);
                break;
            }
            if (read_gps_data(device, &shared_data->gpgga_data, &shared_data->gprmc_data)) {
                if (shared_data->gpgga_data.latitude == 0 && shared_data->gpgga_data.longitude == 0) {
                    shared_data->gps_connected = false;
                    log_with_timestamp("Dados do GPS não disponíveis.");
                } else {
                    shared_data->data_ready = 1;
                    pthread_cond_signal(&shared_data->data_cond);
                    log_with_timestamp("Dados do GPS disponíveis.");
                }
            }
            pthread_mutex_unlock(&shared_data->data_mutex);
            usleep(100000);  // Periodicidade de 0.1 segundo
        }

        close(device);

        log_with_timestamp("Tentando reconectar o dispositivo GPS...");
        sleep(1);
    }

    return NULL;
}

void* thread_periodic_task(void* arg) {
    set_thread_name("Periodic_Task");
    while (keep_running) {
        sleep(5);
    }
    return NULL;
}

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
