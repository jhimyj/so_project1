/*
 * c_lock_manager.h — Gestor central de C-Lock (Árbitro + Global Counter + Pools)
 */
#ifndef C_LOCK_MANAGER_H
#define C_LOCK_MANAGER_H

#include "c_lock.h"

/* Referencia circular: el manager necesita despertar cores */
struct Core;

typedef struct {
    LockEntry     locks[MAX_LOCKS];
    int           num_locks;
    int           num_cores;
    struct Core  *cores[MAX_CORES];  /* punteros a los cores registrados */
    pthread_mutex_t mutex;           /* protege la tabla de locks */
} CLockManager;

/* ADD_ITEM: registrar un lock antes de usarlo */
void clm_register_lock(CLockManager *mgr, int lock_id);

/* BEGIN_C_LOCK: adquirir lock.
 * Retorna 1 si se adquirió inmediatamente, 0 si hay conflicto
 * (el llamador debe activar clock-gating). */
int  clm_acquire(CLockManager *mgr, int lock_id, int core_id);

/* END_C_LOCK: liberar lock y notificar al próximo en espera */
void clm_release(CLockManager *mgr, int lock_id, int core_id);

/* Inicializar / destruir */
void clm_init(CLockManager *mgr, int num_cores);
void clm_destroy(CLockManager *mgr);

#endif /* C_LOCK_MANAGER_H */
