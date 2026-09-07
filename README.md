# Proyecto SO — C-Lock: Simulación y Análisis

Proyecto de Sistemas Operativos basado en el paper:
> *C-Lock: An Energy-Efficient Hardware Synchronization Method for Multicore Embedded Systems*, IEEE Transactions on Computers.

## Integrantes
- Integrante 1
- Integrante 2
- Integrante 3

## Estructura del repositorio

```
.
├── latex/          # Informe en LaTeX (avances semanales)
│   ├── main.tex
│   ├── preamble.tex
│   ├── refs.bib
│   ├── figures/
│   └── sections/
└── sim/            # Simulación de C-Lock
    └── src/
```

## Cómo compilar el informe

```bash
cd latex
pdflatex main.tex
bibtex main
pdflatex main.tex
pdflatex main.tex
```

O con latexmk:

```bash
cd latex
latexmk -pdf main.tex
```

## Avances

| Semana | Descripción                                | Branch     |
|--------|--------------------------------------------|------------|
| 5      | Análisis del paper + diseño arquitectura   | `avance/5` |
