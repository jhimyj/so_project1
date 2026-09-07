# Simulación C-Lock

## Estructura

```
sim/
└── src/
    ├── c_lock_manager.py  # Gestor central (árbitro + counter + pools)
    ├── core.py            # Core con clock-gating
    └── benchmark.py       # Escenarios de prueba
```

## Ejecutar benchmark rápido

```bash
cd sim
python -m src.benchmark
```

## Estado actual

- [x] Esqueleto de `CLockManager`, `Core`, `Benchmark`
- [ ] Modelo de energía refinado
- [ ] Escenario spin-lock para comparación
- [ ] Gráficas de resultados
