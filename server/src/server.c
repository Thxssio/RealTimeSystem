#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sqlite3.h>
#include <unistd.h>
#include "gps/gps.h"
#include "utils/utils.h"

#define PORT 8888
#define GPS_DEVICE "/dev/ublox_gps"  // Certifique-se de que o caminho está correto
#define DB_PATH "/home/thxssio/RealTimeSystem/server/src/gps_data.db"  // Especifique o caminho completo para o banco de dados

// Estrutura para armazenar dados GPS e um mutex para protegê-los
typedef struct {
    gpgga_t gpgga_data;
    gprmc_t gprmc_data;
    pthread_mutex_t data_mutex;
    pthread_cond_t data_cond;
    int data_ready;
    int gps_connected; // Adicionado para monitorar estado de conexão
    sqlite3 *db;
} gps_shared_data_t;

// Dados compartilhados
gps_shared_data_t shared_data = {
    .data_mutex = PTHREAD_MUTEX_INITIALIZER,
    .data_cond = PTHREAD_COND_INITIALIZER,
    .data_ready = 0,
    .gps_connected = 0  // Inicializa como desconectado
};

// Função para adicionar cabeçalhos CORS
void add_cors_headers(struct MHD_Response *response) {
    MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "*");
    MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_METHODS, "GET, POST, OPTIONS");
    MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_HEADERS, "Content-Type");
}

// Função para salvar dados no banco de dados
void save_gps_data(sqlite3 *db, gpgga_t gpgga_data) {
    char *err_msg = 0;
    char sql[512];

    snprintf(sql, sizeof(sql),
             "INSERT INTO gps_data (latitude, longitude, altitude, quality, satellites) VALUES (%f, %f, %f, %d, %d);",
             gpgga_data.latitude, gpgga_data.longitude, gpgga_data.altitude,
             gpgga_data.quality, gpgga_data.satellites);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

// Função para recuperar a última posição conhecida
void get_last_gps_data(sqlite3 *db, gpgga_t *gpgga_data) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT latitude, longitude, altitude, quality, satellites FROM gps_data ORDER BY timestamp DESC LIMIT 1;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        gpgga_data->latitude = sqlite3_column_double(stmt, 0);
        gpgga_data->longitude = sqlite3_column_double(stmt, 1);
        gpgga_data->altitude = sqlite3_column_double(stmt, 2);
        gpgga_data->quality = sqlite3_column_int(stmt, 3);
        gpgga_data->satellites = sqlite3_column_int(stmt, 4);
    }

    sqlite3_finalize(stmt);
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
        if (!shared_data.data_ready) {
            get_last_gps_data(shared_data.db, &shared_data.gpgga_data);
        }
        char json_buffer[512];
        snprintf(json_buffer, sizeof(json_buffer),
                 "{\"latitude\":%f,\"longitude\":%f,\"altitude\":%f,\"quality\":%d,\"satellites\":%d,\"status\":\"%s\"}",
                 shared_data.gpgga_data.latitude, shared_data.gpgga_data.longitude,
                 shared_data.gpgga_data.altitude, shared_data.gpgga_data.quality,
                 shared_data.gpgga_data.satellites, shared_data.gps_connected ? "connected" : "disconnected");
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
        printf("Erro ao abrir o dispositivo GPS.\n");
        return NULL;
    }
    shared_data->gps_connected = 1;  // Marca como conectad

    while (1) {
        int device = setup_gps_device(GPS_DEVICE, B19200);
        if (device < 1) {
            printf("Erro ao abrir o dispositivo GPS.\n");
            shared_data->gps_connected = 0;  // Marca como desconectado
            sleep(1);  // Adiciona um tempo de espera antes de tentar novamente
            continue;
        }
        shared_data->gps_connected = 1;  // Marca como conectado
        memset(&shared_data->gpgga_data, 0, sizeof(gpgga_t));
        memset(&shared_data->gprmc_data, 0, sizeof(gprmc_t));

        if (!read_gps_data(device, &shared_data->gpgga_data, &shared_data->gprmc_data)) {
            printf("Falha na leitura dos dados do GPS. Dados recebidos são inválidos ou zero. Tentando novamente...\n");
            sleep(1);  // Adiciona um tempo de espera antes de tentar novamente
            continue;
        }
        if(read_gps_data(device, &shared_data->gpgga_data, &shared_data->gprmc_data)) {
            shared_data->data_ready = 1;
            pthread_cond_signal(&shared_data->data_cond);
        }

        // Verificação da validade dos dados recebidos.
        if (shared_data->gpgga_data.latitude == 0 && shared_data->gpgga_data.longitude == 0) {
            printf("Dados do GPS são zero, possível perda de sinal. Tentando novamente...\n");
            sleep(1);  // Adiciona um tempo de espera antes de tentar novamente
            continue;
        }

        // Se a qualidade do sinal for suficiente para uma leitura válida
        if (shared_data->gpgga_data.quality > 0) {
            printf("Dados do GPS capturados: Latitude: %f, Longitude: %f, Altitude: %f, Qualidade: %d, Satélites: %d\n",
                   shared_data->gpgga_data.latitude, shared_data->gpgga_data.longitude,
                   shared_data->gpgga_data.altitude, shared_data->gpgga_data.quality,
                   shared_data->gpgga_data.satellites);

            pthread_mutex_lock(&shared_data->data_mutex);
            if (shared_data->gpgga_data.latitude != 0.0 && shared_data->gpgga_data.longitude != 0.0 &&
                shared_data->gpgga_data.altitude != 0.0 && shared_data->gpgga_data.quality > 0 && 
                shared_data->gpgga_data.satellites > 0) {
                save_gps_data(shared_data->db, shared_data->gpgga_data);
            } else {
                printf("Dados inválidos detectados, não armazenados no banco de dados.\n");
            }
            pthread_cond_signal(&shared_data->data_cond);
            pthread_mutex_unlock(&shared_data->data_mutex);
        } else {
            printf("Sinal GPS fraco. Dados podem não ser precisos.\n");
        }
    }
    return NULL;
}

// Função para inicializar o banco de dados SQLite
void init_db(sqlite3 **db) {
    int rc = sqlite3_open(DB_PATH, db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        exit(1);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    // Cria a tabela se não existir
    char *sql = "CREATE TABLE IF NOT EXISTS gps_data ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "latitude REAL NOT NULL,"
                "longitude REAL NOT NULL,"
                "altitude REAL NOT NULL,"
                "quality INTEGER NOT NULL,"
                "satellites INTEGER NOT NULL,"
                "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    char *err_msg = 0;
    rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

int main() {
    struct MHD_Daemon *daemon;

    // Inicializa o banco de dados
    init_db(&shared_data.db);

    // Inicializa a captura de dados do GPS em uma thread separada
    pthread_t gps_thread;
    pthread_create(&gps_thread, NULL, thread_gps_capture, (void*)&shared_data);

    // Configura o servidor HTTP usando MicroHTTPD
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);

    if (NULL == daemon) {
        fprintf(stderr, "Failed to start server.\n");
        return 1;
    }

    printf("Server running on port %d...\n", PORT);

    // Aguarda a thread do GPS terminar
    pthread_join(gps_thread, NULL);

    // Encerra o servidor e libera recursos
    MHD_stop_daemon(daemon);
    sqlite3_close(shared_data.db);

    return 0;
}
