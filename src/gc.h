/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _GC_H_
#define _GC_H_

#include "value.h"
#include "varray.h"
#include "env.h"

/** \file gc.h
 * Gestion mémoire: allocation désallocation automatique (GC)
 */

struct _vm;

/** Structure pour les objets mémoire gérés par le GC.
*/
typedef struct _gc_cell {
  /** type de l'objet géré (T_PAIR ou T_ENV) */
  int type;
  /** l'objet géré par le GC */
  union _gc_content {
    pair_t * as_pair; /*!< l'objet est une paire. */
    env_t * as_env; /*!< l'objet est un environnement. */
  } content;
  /** le successeur dans la liste des objets gérés par le GC. */
  struct _gc_cell *next; 
} gc_cell_t;

/** Structure décrivant l'état du GC.
 */
typedef struct _gc {
  int debug_gc; /*!< GC en mode debug (1) ou non (0) */
  int current_mark; /*!< la marque courante, qui alterne entre 0 ou 1 */
  gc_cell_t heap; /*!< le premier objet de la liste des objets du tas */   
  int nb_allocated  ; /*!< le nombre d'objets alloués. */     
  int collection_frequency; /*!< la fréquence de la récupération (0 pour pas de récupération avant manque de mémoire). */
} gc_t;

/* Initialisation */

gc_t * init_gc(int debug_gc, int collection_frequency);

/* Allocations */

pair_t *gc_alloc_pair(struct _vm *vm);
env_t *gc_alloc_env(gc_t *gc, unsigned int capacity, env_t *next);

/* Marquage/Traçage (cf. gc_mark.c) */

void mark_and_trace_roots(struct _vm *vm);

#endif
