/*
 * c_lock_manager.c — Implementación del gestor central de C-Lock
 */
#include <stdio.h>
#include <assert.h>
#include "../include/c_lock_manager.h"
#include "../include/core.h"

void clm_init(CLockManager *mgr, int num_cores) {
    mgr->num_locks = 0;
    mgr->num_cores = num_cores;
    for (int i = 0; i < MAX_LOCKS; i++) {
        mgr->locks[i].owner    = -1;
        mgr->locks[i].nwaiters = 0;
    }
    for (int i = 0; i < MAX_CORES; i++) {
        mgr->cores[i] = NULL;
    }
    pthread_mutex_init(&mgr->mutex, NULL);
}

void clm_destroy(CLockManager *mgr) {
    pthread_mutex_destroy(&mgr->mutex);
}

/* ADD_ITEM */
void clm_register_lock(CLockManager *mgr, int lock_id) {
    assert(lock_id >= 0 && lock_id < MAX_LOCKS);
    /* El lock ya está inicializado en clm_init; solo marcamos que existe */
    pthread_mutex_lock(&mgr->mutex);
    if (lock_id >= mgr->num_locks)
        mgr->num_locks = lock_id + 1;
    pthread_mutex_unlock(&mgr->mutex);
}

/* BEGIN_C_LOCK */
int clm_acquire(CLockManager *mgr, int lock_id, int core_id) {
    assert(lock_id >= 0 && lock_id < MAX_LOCKS);
    pthread_mutex_lock(&mgr->mutex);

    LockEntry *entry = &mgr->locks[lock_id];
    int acquired;

    if (entry->owner == -1) {
        entry->owner = core_id;
        acquired = 1;
    } else {
        /* Conflicto: encolar este core */
        assert(entry->nwaiters < MAX_WAITERS);
        entry->waiters[entry->nwaiters++] = core_id;
        acquired = 0;
    }

    pthread_mutex_unlock(&mgr->mutex);
    return acquired;
}

/* END_C_LOCK */
void clm_release(CLockManager *mgr, int lock_id, int core_id) {
    assert(lock_id >= 0 && lock_id < MAX_LOCKS);
    pthread_mutex_lock(&mgr->mutex);

    LockEntry *entry = &mgr->locks[lock_id];
    assert(entry->owner == core_id);

    if (entry->nwaiters > 0) {
        /* Pasar el lock al siguiente en espera y despertarlo */
        int next = entry->waiters[0];
        for (int i = 0; i < entry->nwaiters - 1; i++)
            entry->waiters[i] = entry->waiters[i + 1];
        entry->nwaiters--;
        entry->owner = next;

        pthread_mutex_unlock(&mgr->mutex);

        /* Despertar al core (reactivar reloj) */
        if (mgr->cores[next] != NULL)
            core_wake_up(mgr->cores[next]);
    } else {
        entry->owner = -1;
        pthread_mutex_unlock(&mgr->mutex);
    }
}
