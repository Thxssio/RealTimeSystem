#include "include/http_server_thread.h"
#include "include/shared_data_types.h" 
#include "include/utils.h"
#include "include/gps.h"
#include <microhttpd.h>
#include <json-c/json.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define PORT 8888  // Definindo a porta

extern volatile sig_atomic_t keep_running;  // Declarar keep_running como externa

// Função auxiliar para adicionar cabeçalhos CORS
void add_cors_headers(struct MHD_Response *response) {
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");
}

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection, const char* url,
                                const char* method, const char* version, const char* upload_data,
                                size_t* upload_data_size, void** con_cls) {
    log_with_timestamp("Recebendo requisição HTTP");
    log_with_timestamp(url);
    log_with_timestamp(method);

    gps_shared_data_t* shared_data = (gps_shared_data_t*)cls;
    struct json_object* jobj = json_object_new_object();

    if (strcmp(url, "/gps-data") == 0) {
        pthread_mutex_lock(&shared_data->data_mutex);
        json_object_object_add(jobj, "latitude", json_object_new_double(shared_data->gpgga_data.latitude));
        json_object_object_add(jobj, "longitude", json_object_new_double(shared_data->gpgga_data.longitude));
        json_object_object_add(jobj, "altitude", json_object_new_double(shared_data->gpgga_data.altitude));
        json_object_object_add(jobj, "quality", json_object_new_int(shared_data->gpgga_data.quality));
        json_object_object_add(jobj, "satellites", json_object_new_int(shared_data->gpgga_data.satellites));
        json_object_object_add(jobj, "status", json_object_new_string(shared_data->gps_connected ? "connected" : "disconnected"));
        json_object_object_add(jobj, "accel_x", json_object_new_double(shared_data->mpu9250_data.accel_x));
        json_object_object_add(jobj, "accel_y", json_object_new_double(shared_data->mpu9250_data.accel_y));
        json_object_object_add(jobj, "accel_z", json_object_new_double(shared_data->mpu9250_data.accel_z));
        json_object_object_add(jobj, "gyro_x", json_object_new_double(shared_data->mpu9250_data.gyro_x));
        json_object_object_add(jobj, "gyro_y", json_object_new_double(shared_data->mpu9250_data.gyro_y));
        json_object_object_add(jobj, "gyro_z", json_object_new_double(shared_data->mpu9250_data.gyro_z));
        json_object_object_add(jobj, "mag_x", json_object_new_double(shared_data->mpu9250_data.mag_x));
        json_object_object_add(jobj, "mag_y", json_object_new_double(shared_data->mpu9250_data.mag_y));
        json_object_object_add(jobj, "mag_z", json_object_new_double(shared_data->mpu9250_data.mag_z));

        struct json_object* sat_array = json_object_new_array();
        for (int i = 0; i < shared_data->gsv_data.num_satellites; ++i) {
            struct json_object* sat_obj = json_object_new_object();
            if (shared_data->gsv_data.satellites[i].prn > 0 && shared_data->gsv_data.satellites[i].elevation >= 0 && 
                shared_data->gsv_data.satellites[i].azimuth >= 0 && shared_data->gsv_data.satellites[i].snr >= 0) {
                json_object_object_add(sat_obj, "prn", json_object_new_int(shared_data->gsv_data.satellites[i].prn));
                json_object_object_add(sat_obj, "elevation", json_object_new_int(shared_data->gsv_data.satellites[i].elevation));
                json_object_object_add(sat_obj, "azimuth", json_object_new_int(shared_data->gsv_data.satellites[i].azimuth));
                json_object_object_add(sat_obj, "snr", json_object_new_int(shared_data->gsv_data.satellites[i].snr));
                json_object_array_add(sat_array, sat_obj);
            }
        }
        json_object_object_add(jobj, "satellites_info", sat_array);

        pthread_mutex_unlock(&shared_data->data_mutex);

        const char* json_str = json_object_to_json_string(jobj);
        log_with_timestamp(json_str);  // Log da resposta JSON

        struct MHD_Response* response = MHD_create_response_from_buffer(strlen(json_str), (void*)json_str, MHD_RESPMEM_MUST_COPY);
        if (response == NULL) {
            log_with_timestamp("Falha ao criar resposta HTTP.");
            return MHD_NO;
        }

        add_cors_headers(response); // Adiciona cabeçalhos CORS

        log_with_timestamp("Enviando resposta HTTP");
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        if (ret != MHD_YES) {
            log_with_timestamp("Falha ao enviar resposta HTTP.");
        } else {
            log_with_timestamp("Resposta enviada");
        }
        MHD_destroy_response(response);
        json_object_put(jobj);

        return ret;
    }

    if (strcmp(url, "/send-command") == 0) {
        log_with_timestamp("Recebendo comando HTTP");
        if (strcmp(method, "POST") != 0) {
            return MHD_NO;
        }

        if (*upload_data_size != 0) {
            char* data = malloc(*upload_data_size + 1);
            memcpy(data, upload_data, *upload_data_size);
            data[*upload_data_size] = '\0';

            struct json_object* jobj = json_tokener_parse(data);
            struct json_object* command_obj;

            if (json_object_object_get_ex(jobj, "command", &command_obj)) {
                const char* command = json_object_get_string(command_obj);
                log_with_timestamp(command);  // Log do comando recebido
                // Processar o comando aqui
            } else {
                log_with_timestamp("Comando não encontrado no JSON recebido.");
            }

            free(data);
            json_object_put(jobj);
        } else {
            log_with_timestamp("Nenhum dado recebido no corpo da solicitação.");
        }

        struct json_object* response_obj = json_object_new_object();
        json_object_object_add(response_obj, "status", json_object_new_string("Comando recebido"));

        const char* response_str = json_object_to_json_string(response_obj);
        struct MHD_Response* response = MHD_create_response_from_buffer(strlen(response_str), (void*)response_str, MHD_RESPMEM_MUST_COPY);

        if (response == NULL) {
            log_with_timestamp("Falha ao criar resposta HTTP.");
            return MHD_NO;
        }

        add_cors_headers(response); // Adiciona cabeçalhos CORS

        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        if (ret != MHD_YES) {
            log_with_timestamp("Falha ao enviar resposta HTTP.");
        } else {
            log_with_timestamp("Resposta enviada");
        }
        MHD_destroy_response(response);
        json_object_put(response_obj);

        return ret;
    }

    return MHD_NO;
}

void* thread_http_server(void* arg) {
    set_thread_name("HTTP_Server");
    log_with_timestamp("Iniciando servidor HTTP...");
    struct MHD_Daemon* daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &request_handler, arg, MHD_OPTION_END);

    if (daemon == NULL) {
        log_with_timestamp("Falha ao iniciar o servidor HTTP.");
        return NULL;
    }

    log_with_timestamp("Servidor HTTP iniciado na porta 8888.");

    while (keep_running) {
        sleep(1);
    }

    MHD_stop_daemon(daemon);
    log_with_timestamp("Servidor HTTP encerrado.");
    return NULL;
}
