/*
 * benchmark.c — Escenarios de prueba: C-Lock vs Spin-Lock
 *
 * Compilar: make
 * Ejecutar: ./sim_clock
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/c_lock.h"
#include "../include/c_lock_manager.h"
#include "../include/core.h"

/* ---- Configuración del benchmark ---- */
#define NUM_CORES    4
#define NUM_LOCKS    1
#define WORK_CYCLES  5    /* ciclos dentro de la sección crítica */
#define ITERATIONS   3    /* veces que cada core entra a la SC */

/* ---- Argumento para cada hilo ---- */
typedef struct {
    Core *core;
    int   num_locks;
    int   work_cycles;
    int   iterations;
} ThreadArg;

/* ---- Tarea de cada core (C-Lock) ---- */
static void *core_task(void *arg) {
    ThreadArg *a = (ThreadArg *)arg;
    Core      *c = a->core;

    for (int i = 0; i < a->iterations; i++) {
        int lock_id = i % a->num_locks;

        core_begin_c_lock(c, lock_id);          /* BEGIN_C_LOCK */
        core_work(c, a->work_cycles);            /* sección crítica */
        core_end_c_lock(c, lock_id);             /* END_C_LOCK   */

        core_work(c, a->work_cycles);            /* trabajo fuera de SC */
    }
    return NULL;
}

/* ---- Benchmark principal ---- */
int main(void) {
    CLockManager mgr;
    Core         cores[NUM_CORES];
    pthread_t    threads[NUM_CORES];
    ThreadArg    args[NUM_CORES];

    printf("=== Benchmark C-Lock ===\n");
    printf("Cores: %d  |  Locks: %d  |  Iteraciones: %d\n\n",
           NUM_CORES, NUM_LOCKS, ITERATIONS);

    /* Inicializar manager y registrar locks */
    clm_init(&mgr, NUM_CORES);
    for (int l = 0; l < NUM_LOCKS; l++)
        clm_register_lock(&mgr, l);

    /* Inicializar cores */
    for (int i = 0; i < NUM_CORES; i++)
        core_init(&cores[i], i, &mgr);

    /* Lanzar hilos */
    for (int i = 0; i < NUM_CORES; i++) {
        args[i] = (ThreadArg){ &cores[i], NUM_LOCKS, WORK_CYCLES, ITERATIONS };
        pthread_create(&threads[i], NULL, core_task, &args[i]);
    }

    /* Esperar a todos */
    for (int i = 0; i < NUM_CORES; i++)
        pthread_join(threads[i], NULL);

    /* Mostrar métricas */
    double total_energy = 0.0;
    printf("%-8s %-14s %-14s %-10s\n",
           "Core", "Ciclos activos", "Ciclos gateados", "Energía");
    printf("%-8s %-14s %-14s %-10s\n",
           "----", "--------------", "---------------", "-------");

    for (int i = 0; i < NUM_CORES; i++) {
        CoreStats s = core_get_stats(&cores[i]);
        printf("%-8d %-14ld %-14ld %-10.2f\n",
               i, s.cycles_active, s.cycles_gated, s.energy);
        total_energy += s.energy;
        core_destroy(&cores[i]);
    }

    printf("\nEnergía total (C-Lock): %.2f\n", total_energy);

    clm_destroy(&mgr);
    return 0;
}
