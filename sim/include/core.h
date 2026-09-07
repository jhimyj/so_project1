/*
 * core.h — Simulación de un core del procesador con soporte a clock-gating
 */
#ifndef CORE_H
#define CORE_H

#include "c_lock.h"
#include "c_lock_manager.h"

typedef struct Core {
    int           core_id;
    CoreState     state;
    CLockManager *manager;

    /* Métricas */
    long          cycles_active;
    long          cycles_gated;

    /* Sincronización para clock-gating */
    pthread_mutex_t gate_mutex;
    pthread_cond_t  gate_cond;
    int             gated;   /* 1 = reloj detenido, 0 = activo */
} Core;

/* Inicializar / destruir */
void core_init(Core *core, int core_id, CLockManager *mgr);
void core_destroy(Core *core);

/* Interfaz C-Lock */
void core_begin_c_lock(Core *core, int lock_id);   /* BEGIN_C_LOCK */
void core_end_c_lock(Core *core, int lock_id);     /* END_C_LOCK   */

/* Llamado por CLockManager cuando el lock queda libre */
void core_wake_up(Core *core);

/* Simula N ciclos de trabajo (dentro o fuera de sección crítica) */
void core_work(Core *core, int cycles);

/* Retorna métricas del core */
CoreStats core_get_stats(const Core *core);

#endif /* CORE_H */
