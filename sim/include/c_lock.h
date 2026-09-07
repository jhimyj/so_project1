/*
 * c_lock.h — Tipos y constantes compartidas del simulador C-Lock
 */
#ifndef C_LOCK_H
#define C_LOCK_H

#include <pthread.h>

/* ---- Parámetros configurables ---- */
#define MAX_CORES    16
#define MAX_LOCKS    64
#define MAX_WAITERS  MAX_CORES

/* Modelo de energía simplificado (unidades arbitrarias por ciclo) */
#define E_ACTIVE     1.0
#define E_GATED      0.05   /* clock-gating: mucho menor que activo */
#define E_SPINLOCK   1.0    /* spin-lock: igual que activo (busy-waiting) */

/* ---- Estado de un core ---- */
typedef enum {
    CORE_RUNNING,   /* ejecutando normalmente */
    CORE_GATED      /* reloj detenido (esperando lock) */
} CoreState;

/* ---- Entrada en la tabla de locks del manager ---- */
typedef struct {
    int  owner;                    /* core_id dueño (-1 si libre) */
    int  waiters[MAX_WAITERS];     /* cola de espera */
    int  nwaiters;
} LockEntry;

/* ---- Métricas de un core ---- */
typedef struct {
    long cycles_active;
    long cycles_gated;
    double energy;
} CoreStats;

#endif /* C_LOCK_H */
