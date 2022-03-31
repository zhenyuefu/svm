#ifndef _PRIM_H_
#define _PRIM_H_

#include "varray.h"
#include "vm.h"

/** Exécution d'une primitive.
 * Les arguments sont sur la pile du cadre d'appel (frame) courant
 * (premier argument au sommet, etc.).
 * \param[in,out] stack la zone de pile.
 * \param prim le numéro de la primitive à invoquer.
 * \param n le nombre d'arguments à dépiler.
 */
void execute_prim(vm_t *vm, varray_t *stack, int prim, int n);

#endif