#include "gps/gps.h"
#include "utils/utils.h"
#include <stdio.h>
#include <unistd.h>  // Para o sleep
#include <fcntl.h>   // Para as flags de open
#include <termios.h> // Para configuração da porta serial
#include <pthread.h> // Para threads, mutexes e variáveis de condição

// Estrutura para armazenar dados GPS e um mutex para protegê-los
typedef struct {
    gpgga_t gpgga_data;
    gprmc_t gprmc_data;
    pthread_mutex_t data_mutex;
    pthread_cond_t data_cond;
    int data_ready;
} gps_shared_data_t;

// Prototipos das funções das threads
void* thread_gps_capture(void* arg);
void* thread_periodic_task(void* arg);
void* thread_network_communication(void* arg);
void* thread_condition_handler(void* arg);

// Função para obter a descrição da qualidade do sinal
const char* get_signal_quality_description(int quality) {
    switch (quality) {
        case 0: return "Fixação inválida";
        case 1: return "Fixação de GPS padrão (2D/3D)";
        case 2: return "Fixação diferencial GPS (DGPS)";
        case 3: return "PPS fix (pulse per second)";
        case 4: return "Fixação RTK";
        case 5: return "Float RTK";
        case 6: return "Estimativa baseada em dados antigos";
        case 7: return "Modo de entrada manual";
        case 8: return "Fixação simulada";
        default: return "Desconhecida";
    }
}

int main() {
    pthread_t thread1, thread2, thread3, thread4;
    gps_shared_data_t shared_data = {
        .data_mutex = PTHREAD_MUTEX_INITIALIZER,
        .data_cond = PTHREAD_COND_INITIALIZER,
        .data_ready = 0
    };

    // Inicialize as threads
    pthread_create(&thread1, NULL, thread_gps_capture, &shared_data);
    pthread_create(&thread2, NULL, thread_periodic_task, &shared_data);
    pthread_create(&thread3, NULL, thread_network_communication, &shared_data);
    pthread_create(&thread4, NULL, thread_condition_handler, &shared_data);

    // Aguarde as threads terminarem
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    return 0;
}

// Função da thread para capturar dados do GPS
void* thread_gps_capture(void* arg) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    int device = setup_gps_device("/dev/ublox_gps", B19200);
    if (device < 0) {
        printf("Erro ao abrir o dispositivo GPS.\n");
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&shared_data->data_mutex);
        if (read_gps_data(device, &shared_data->gpgga_data, &shared_data->gprmc_data)) {
            shared_data->data_ready = 1;
            pthread_cond_signal(&shared_data->data_cond);
        }
        pthread_mutex_unlock(&shared_data->data_mutex);
        sleep(0.5); // Periodicidade de 1 segundo
    }

    close(device);
    return NULL;
}

// Função da thread periódica adicional
void* thread_periodic_task(void* arg) {
    while (1) {
        // Realizar alguma tarefa periódica, como atualização de log
        sleep(5); // Periodicidade de 5 segundos
    }
    return NULL;
}

// Função da thread de comunicação em rede
void* thread_network_communication(void* arg) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    while (1) {
        pthread_mutex_lock(&shared_data->data_mutex);
        while (!shared_data->data_ready) {
            pthread_cond_wait(&shared_data->data_cond, &shared_data->data_mutex);
        }
        // Enviar dados via rede
        // Exemplo de envio:
        printf("Enviando dados: Latitude: %f, Longitude: %f, Altitude: %f\n",
               shared_data->gpgga_data.latitude, shared_data->gpgga_data.longitude, shared_data->gpgga_data.altitude);
        shared_data->data_ready = 0;
        pthread_mutex_unlock(&shared_data->data_mutex);
    }
    return NULL;
}

// Função da thread para lidar com a variável de condição
void* thread_condition_handler(void* arg) {
    gps_shared_data_t* shared_data = (gps_shared_data_t*)arg;
    while (1) {
        pthread_mutex_lock(&shared_data->data_mutex);
        while (!shared_data->data_ready) {
            pthread_cond_wait(&shared_data->data_cond, &shared_data->data_mutex);
        }
        // Processar os dados do GPS
        if (shared_data->gpgga_data.latitude == 0.0 && shared_data->gpgga_data.longitude == 0.0) {
            printf("Obtendo dados do GPS...\n");
        } else {
            printf("Latitude: %f, Longitude: %f, Altitude: %f\n",
                   shared_data->gpgga_data.latitude, shared_data->gpgga_data.longitude, shared_data->gpgga_data.altitude);
            printf("Qualidade do Sinal: %d (%s), Satélites: %d\n",
                   shared_data->gpgga_data.quality, get_signal_quality_description(shared_data->gpgga_data.quality), shared_data->gpgga_data.satellites);
        }
        shared_data->data_ready = 0;
        pthread_mutex_unlock(&shared_data->data_mutex);
        sleep(1); // Evitar busy-wait
    }
    return NULL;
}
