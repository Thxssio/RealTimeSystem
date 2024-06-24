#include "include/periodic_task_thread.h"
#include "include/utils.h"
#include <unistd.h>
#include <signal.h>  // Adicionar este include

extern volatile sig_atomic_t keep_running;

void* thread_periodic_task(void* arg) {
    set_thread_name("Periodic_Task");
    while (keep_running) {
        sleep(5);
    }
    return NULL;
}
