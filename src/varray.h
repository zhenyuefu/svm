/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _VARRAY_H_
#define _VARRAY_H_

#include "value.h"

/** \file varray.h
 * Représentation des tableaux de valeurs.
 * Les tableaux de valeurs se retrouvent à plusieurs endroit de la VM :
 *  - pour représenter l'environnement de chaque stack frame
 *  - et ???
 */

/** Structure générique pour tout tableau de cellules.
 */
typedef struct {
  value_t* content; /*!< le contenu (tableau dynamique de cellules) */      
  unsigned int capacity; /*!< capacité du tableau (taille allouée) */
  unsigned int top; /*!< dernier élément utilisé (ou sommet de pile).
                     * Remarque : tous les éléments entre top+1 et capacity-1 sont inutilisés (mais alloués) */ 
} varray_t;

/*
 * Fonctions de manipulation des tableaux.
 */

varray_t *varray_allocate(unsigned int initial_capacity);
void varray_expandn(varray_t *varray, unsigned int n);
void varray_popn(varray_t *varray, unsigned int n);
value_t *varray_at(varray_t *varray, unsigned int n);
void varray_set_at(varray_t *varray, unsigned int n, value_t *value);
value_t *varray_top_at(varray_t *varray, unsigned int n);
value_t *varray_top(varray_t *varray);
void varray_set_top(varray_t *varray, value_t *value);
void varray_set_top_at(varray_t *varray, unsigned int n, value_t *value);
void varray_push(varray_t *varray, value_t *value);
value_t *varray_pop(varray_t *varray);
int varray_size(varray_t *varray);
int varray_empty(varray_t *varray);
void varray_destroy(varray_t *varray);
void varray_print(varray_t *varray);
void varray_stack_print(varray_t *varray);

#endif
