#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gps/gps.h"
#include "utils/utils.h"

#define PORT 8888
#define GPS_DEVICE "/dev/ublox_gps"  // Certifique-se de que o caminho está correto

// Estrutura para armazenar dados GPS e um mutex para protegê-los
typedef struct {
    gpgga_t gpgga_data;
    gprmc_t gprmc_data;
    pthread_mutex_t data_mutex;
    pthread_cond_t data_cond;
    int data_ready;
} gps_shared_data_t;

// Dados compartilhados
gps_shared_data_t shared_data = {
    .data_mutex = PTHREAD_MUTEX_INITIALIZER,
    .data_cond = PTHREAD_COND_INITIALIZER,
    .data_ready = 0
};

// Função para adicionar cabeçalhos CORS
void add_cors_headers(struct MHD_Response *response) {
    MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "*");
    MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_METHODS, "GET, POST, OPTIONS");
    MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_HEADERS, "Content-Type");
}

// Função para lidar com requisições HTTP
enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                     const char *url, const char *method, const char *version,
                                     const char *upload_data, size_t *upload_data_size, void **con_cls) {
    const char *json;
    struct MHD_Response *response;
    int ret;

    if (strcmp(url, "/gps-data") == 0 && strcmp(method, "GET") == 0) {
        pthread_mutex_lock(&shared_data.data_mutex);
        char json_buffer[512];
        snprintf(json_buffer, sizeof(json_buffer),
                 "{\"latitude\":%f,\"longitude\":%f,\"altitude\":%f,\"quality\":%d,\"satellites\":%d}",
                 shared_data.gpgga_data.latitude, shared_data.gpgga_data.longitude,
                 shared_data.gpgga_data.altitude, shared_data.gpgga_data.quality,
                 shared_data.gpgga_data.satellites);
        json = json_buffer;
        printf("Sending JSON: %s\n", json);  // Mensagem de depuração
        pthread_mutex_unlock(&shared_data.data_mutex);

        response = MHD_create_response_from_buffer(strlen(json), (void*) json, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "application/json");
        add_cors_headers(response);  // Adiciona os cabeçalhos CORS
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
    } else if (strcmp(method, "OPTIONS") == 0) {
        response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
        add_cors_headers(response);  // Adiciona os cabeçalhos CORS
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
    } else {
        response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
        MHD_destroy_response(response);
    }

    return ret;
}

// Função da thread para capturar dados do GPS
void* thread_gps_capture(void* arg) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    int device = setup_gps_device(GPS_DEVICE, B19200);
    if (device < 0) {
        perror("Erro ao abrir o dispositivo");
        printf("Erro ao abrir o dispositivo GPS.\n");
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&shared_data->data_mutex);
        if (read_gps_data(device, &shared_data->gpgga_data, &shared_data->gprmc_data)) {
            shared_data->data_ready = 1;
            printf("Dados do GPS capturados: Latitude: %f, Longitude: %f, Altitude: %f, Qualidade: %d, Satélites: %d\n",
                   shared_data->gpgga_data.latitude, shared_data->gpgga_data.longitude,
                   shared_data->gpgga_data.altitude, shared_data->gpgga_data.quality,
                   shared_data->gpgga_data.satellites);  // Mensagem de depuração
            pthread_cond_signal(&shared_data->data_cond);
        }
        pthread_mutex_unlock(&shared_data->data_mutex);
        sleep(1); // Periodicidade de 1 segundo
    }

    close(device);
    return NULL;
}

int main() {
    struct MHD_Daemon *daemon;

    pthread_t gps_thread;
    pthread_create(&gps_thread, NULL, thread_gps_capture, &shared_data);

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Servidor rodando na porta %d\n", PORT);
    (void) getchar(); // Aguarda input para finalizar o servidor

    MHD_stop_daemon(daemon);
    pthread_join(gps_thread, NULL);

    return 0;
}
