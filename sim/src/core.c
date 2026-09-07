/*
 * core.c — Simulación de un core con clock-gating
 */
#include <unistd.h>
#include "../include/core.h"

void core_init(Core *core, int core_id, CLockManager *mgr) {
    core->core_id      = core_id;
    core->state        = CORE_RUNNING;
    core->manager      = mgr;
    core->cycles_active = 0;
    core->cycles_gated  = 0;
    core->gated         = 0;
    pthread_mutex_init(&core->gate_mutex, NULL);
    pthread_cond_init(&core->gate_cond, NULL);

    /* Registrar este core en el manager */
    mgr->cores[core_id] = core;
}

void core_destroy(Core *core) {
    pthread_mutex_destroy(&core->gate_mutex);
    pthread_cond_destroy(&core->gate_cond);
}

/* BEGIN_C_LOCK */
void core_begin_c_lock(Core *core, int lock_id) {
    int acquired = clm_acquire(core->manager, lock_id, core->core_id);
    if (!acquired) {
        /* Conflicto: activar clock-gating (suspender este core) */
        pthread_mutex_lock(&core->gate_mutex);
        core->gated = 1;
        core->state = CORE_GATED;

        while (core->gated) {
            /* Simula el core suspendido (cuenta ciclos gateados) */
            core->cycles_gated++;
            pthread_cond_wait(&core->gate_cond, &core->gate_mutex);
        }

        core->state = CORE_RUNNING;
        pthread_mutex_unlock(&core->gate_mutex);
    }
}

/* END_C_LOCK */
void core_end_c_lock(Core *core, int lock_id) {
    clm_release(core->manager, lock_id, core->core_id);
}

/* Llamado por CLockManager: reactiva el reloj del core */
void core_wake_up(Core *core) {
    pthread_mutex_lock(&core->gate_mutex);
    core->gated = 0;
    pthread_cond_signal(&core->gate_cond);
    pthread_mutex_unlock(&core->gate_mutex);
}

/* Simula N ciclos de trabajo activo */
void core_work(Core *core, int cycles) {
    for (int i = 0; i < cycles; i++) {
        core->cycles_active++;
        usleep(1000); /* 1 ms por ciclo simulado */
    }
}

CoreStats core_get_stats(const Core *core) {
    CoreStats s;
    s.cycles_active = core->cycles_active;
    s.cycles_gated  = core->cycles_gated;
    s.energy = (core->cycles_active * E_ACTIVE) +
               (core->cycles_gated  * E_GATED);
    return s;
}
