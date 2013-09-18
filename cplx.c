#include "cplx.h"
#include <math.h>

double Cplx_module(Complexe z) {
    return sqrt(z.a*z.a + z.b*z.b);
}

Complexe Cplx_conjugue(Complexe z) {
    Complexe ret = {z.a, -z.b};
    return ret;
}

bool Cplx_estPur(Complexe z) {
    return fabs(z.a) <= DBL_EPSILON;
}
bool Cplx_estReel(Complexe z) {
    return fabs(z.b) <= DBL_EPSILON;
}
Complexe Cplx_addition(Complexe z1, Complexe z2) {
    Complexe ret = {z1.a + z2.a, z1.b + z2.b};
    return ret;
}
Complexe Cplx_soustraction(Complexe z1, Complexe z2) {
    Complexe ret = {z1.a - z2.a, z1.b - z2.a};
    return ret;
}
Complexe Cplx_multiplication(Complexe z1, Complexe z2) {
    Complexe ret = {z1.a*z2.a - z1.b*z2.b, z1.a*z2.b + z1.b*z2.a};
    return ret;
}
Complexe Cplx_division(Complexe z1, Complexe z2) {
    if(fabs(z2.a) > DBL_EPSILON && fabs(z2.b) > DBL_EPSILON) {
        double den = z2.a*z2.a + z2.b*z2.b;
        Complexe ret = {(z1.a*z2.a + z1.b*z2.b)/den, (z1.b*z2.a - z1.a*z2.b)/den};
        return ret;
    }
}
Complexe Cplx_puissance(Complexe z, unsigned int p) {
    Complexe ret = z;
    for(unsigned i = 1; i < p; ++i)
        ret = Cplx_multiplication(ret, z);
    return ret;
}
