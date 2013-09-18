#ifndef COMPLEXE_H
#define COMPLEXE_H

#include <stdbool.h>
#include <float.h>

typedef struct Complexe_S {
    double  a,
            b;
} Complexe;

double      Cplx_module(Complexe);
Complexe    Cplx_conjugue(Complexe);
bool        Cplx_estPur(Complexe);
bool        Cplx_estReel(Complexe);
Complexe    Cplx_addition(Complexe, Complexe);
Complexe    Cplx_soustraction(Complexe, Complexe);
Complexe    Cplx_multiplication(Complexe, Complexe);
Complexe    Cplx_division(Complexe, Complexe);
Complexe    Cplx_puissance(Complexe, unsigned);

#endif // COMPLEXE_H

