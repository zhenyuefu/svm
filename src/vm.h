/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _VM_H_
#define _VM_H_

/** \file vm.h
 * Représentation de l'état de la machine virtuelle.
 * L'état de la VM est composé de :
 * - un pointeur de code
 * - une pile
 * - un environnement
 * - une pile de bloc d'appels (frame)
 * - l'environnement global
 * - les primitives (?)
 * - la zone de code
 * - le GC
 */

#include "value.h"
#include "gc.h"
#include "bytecode.h"
#include "frame.h"
#include "constants.h"

/** La représentation de la machine virtuelle. */
typedef struct _vm {
  int debug_vm; /*!< VM en mode debug (1) ou non (0) */
  varray_t *globs; /*!< l'environnement global (variables globales) */
  varray_t *stack;  /*!< la pile */
  frame_t *frame; /*!< la fenêtre d'entrée */
  program_t *program;
  gc_t *gc;
} vm_t;



/** La taille allouée pour la pile */
#define STACK_SIZE 256

/** La taille allouée pour les variables globales */
#define GLOBS_SIZE 256

/** La fréquence de GC par défaut (en nombre d'instructions exécutées) */
#define DEFAULT_GC_FREQUENCY 10000

/* Manipulation de l'état de la VM */

vm_t * init_vm(program_t *program, int debug_vm, int debug_gc, int collection_frequency);

/* Exécution du bytecode (cf. vm_execute.c) */

void vm_execute(vm_t * vm);

#endif
