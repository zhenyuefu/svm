/** \file gc.c
 * Gestion mémoire: allocation désallocation automatique (GC)
 *   implantation
 *
 * Les valeurs allouées par la VM sont les fermetures, chaines et paires.
 * Chaque valeur allouée est munie d'un entête exploité par le mécanisme 
 * de récupération de la mémoire. L'algorithme de récupération de la
 * mémoire est "mark and sweep". Il repose sur le principe suivant:
 *  1) parcourir et marquer l'ensemble des valeurs accessibles depuis 
 *     les ressources de la VM (pile et environnement);
 *  2) parcourir l'ensemble de toutes les valeurs qui ont été allouées
 *     en supprimant celles qui n'ont pas été marquées.
 *
 * Le mécanisme de gestion mémoire repose sur une variable globale ('gc')
 * qui est une structure contenant (voir type gc_t):
 *  - (un pointeur vers) la fonction permettant de parcourir et de marquer
 *    l'ensemble des valeurs allouées accessibles depuis la VM;
 *  - une marque courante (0 ou 1) qui permettra de distinguer les
 *    valeurs encore accessibles depuis la VM de celles qui ne le sont plus;
 *  - une liste (circulaire, doublement chaînée) de tous des objets
 *    alloués depuis le dernier GC.
 *  - le nombre d'objets alloués depuis le dernier GC;
 *  - la fréquence de déclenchement du GC.
 *
 * Un objet mémoire contient (voir type gc_obj_t):
 *  - un pointeur vers la cellule gérée
 *  - un pointeur vers l'objet mémoire suivant;
 ******/

#include <assert.h>
#include <stdlib.h>  /* calloc */
#include <stdio.h>

#include "vm.h"

/** Affichage du tas par le GC (déboguage). */
void print_gc_list(char *msg, gc_cell_t *head) {
  gc_cell_t *cell;
  printf("[gc] state (%s) : %p", msg, head);
  cell = head;
  for(cell=cell->next; cell!=NULL; cell=cell->next)
    printf(" -> %p", cell);
  printf("\n");
}

static void gc_delete_pair(pair_t *pair);
static void gc_delete_env(env_t *env);
static void gc_delete_string(string_t *str);

/** Désallocation d'une cellule mémoire par le GC.
 * \param[in,out] cell la cellule à désallouer.
 */
static void gc_delete(gc_cell_t *cell) {
  if(cell->type==T_PAIR) {
    gc_delete_pair((pair_t *) cell->content.as_pair);
  } else if(cell->type==T_ENV) {
    gc_delete_env((env_t *) cell->content.as_env);
  } else if(cell->type==T_STRING) {
    gc_delete_string((string_t *) cell->content.as_string);
  } else {
    printf("[ABORT] Cannot delete cell : Unknown cell type '%d'\n",cell->type);
    abort();
  }
  
  free(cell);
}

/** Lecture de la marque pour une cellule mémoire.
 * \param cell la cellule mémoire à lire.
 * \return la valeur de la marque (0 ou 1).
 */
int gc_cell_mark(gc_cell_t *cell) {
  if(cell->type==T_PAIR) {
    return cell->content.as_pair->gc_mark;
  } else if(cell->type==T_ENV) {
    return cell->content.as_env->gc_mark;
  } else if (cell->type == T_STRING) {
    return cell->content.as_string->gc_mark;
  } else {
    printf("[ABORT] Cannot get GC mark : Unknown cell type '%d'\n",cell->type);
    abort();
  }
}

/** Phase de balayage (sweep) de l'algorithme de GC. */
static void gc_sweep(gc_t * gc) {
  if(gc->debug_gc) {
    printf("[GC]   Sweep phase started\n");
  }

  // sweep from the heap all objects marked with
  // the old mark
  gc_cell_t *prev;
  gc_cell_t *cell;
  
  // the first object is "empty": pass it
  prev = &(gc->heap);
  cell = prev->next;
  // while the end of heap is not reached
  while(cell != NULL) { 
    // tester la marque
    if (gc_cell_mark(cell) != gc->current_mark) { 
      // si la cellule n'a pas été marquée, on la récupère
      if(gc->debug_gc) {
        printf("[GC]    free cell %p\n", (void *) cell); 
      }
      prev->next = cell->next;
      gc_delete(cell);
      cell = prev->next;
    } else {
      prev = cell;
      cell = cell->next;
    }
  }
}


/** Algorithme de récupération automatique de mémoire (Garbage Colletion).
 */
void gc_collect(vm_t * vm) {
  if(vm->gc->debug_gc) {
    printf("[GC] Collector started\n");
  }
  
  // alterner la marque courante
  // => comme tout était marqué à l'issu de la dernière récupération
  // maintenant on a tout "démarqué" d'un seul coup
  vm->gc->current_mark = (vm->gc->current_mark==0) ? 1 : 0;                                     
  if(vm->gc->debug_gc) {
    printf("[GC] Current mark set to : %d\n",vm->gc->current_mark);
  }

  // on va recompter le nombre d'objets alloués effectivement (ce sont ceux qui
  // resteront à la fin du mark&sweep)
  vm->gc->nb_allocated = 0;                                   
  
  // Phase 1 : marquage depuis les racines de la VM
  mark_and_trace_roots(vm);
  
  // Phase 2 : sweep
  gc_sweep(vm->gc);

  if(vm->gc->debug_gc) {
    printf("[GC] Collector finished\n");
  }
  
}

/** Allocation d'une cellule pour le GC et chaînage dans le tas.
 * \param[in,out] gc le garbage collector.
 * \return la cellule allouée.
 */
static gc_cell_t * gc_alloc_cell(gc_t * gc) {
  gc_cell_t *cell = (gc_cell_t *) malloc(sizeof(gc_cell_t));
  assert(cell!=NULL);
  cell->next = gc->heap.next;
  gc->heap.next = cell;
  
  return cell;
}

/** Allocation d'une paire gérée par le GC.
 * \param[in,out] vm l'état global de la VM.
 * \return un pointeur sur une paire allouée vide.
 */
pair_t * gc_alloc_pair(vm_t *vm) {
  pair_t * pair = (pair_t *) malloc(sizeof(pair_t));
  assert(pair!=NULL);
  value_fill_unit(&pair->car);
  value_fill_unit(&pair->cdr);
  pair->gc_mark = vm->gc->current_mark; // paire non-marquée initialement

  gc_cell_t * cell = gc_alloc_cell(vm->gc);
  cell->type = T_PAIR;
  cell->content.as_pair = pair;
  return pair;
}

static void gc_delete_pair(pair_t *pair) {
  free(pair);
}

/** Allocation d'un environnement local gérée par le GC.
 * \param[in,out] gc le garbage collector.
 * \param[in] capacity la taille allouée pour l'environnement.
 * \param[in] next l'environnement père.
 * \return un pointeur sur le nouvel environnement.
 */
env_t* gc_alloc_env(gc_t * gc, unsigned int capacity, env_t *next) {

  if(capacity == 0) {
    // cas particulier : rien à allouer donc on se contente de l'environnement parent.
    return next;
  } else {
    // sinon on effectue l'allocation.
    env_t *env = (env_t *) malloc(sizeof(env_t));
    assert(env!=NULL);
    
    env->content = varray_allocate(capacity);
    env->content->top = capacity;  // on considère que le tableau 
                                   // est plein (environnement non-dynamique)
    env->next = next; // on chaîne vers le parent.

    env->gc_mark = gc->current_mark; // paire non-marquée initialement


    gc_cell_t * cell = gc_alloc_cell(gc);
    cell->type = T_ENV;
    cell->content.as_env = env;

    return env;
  }
}

static void gc_delete_env(env_t *env) {
  varray_destroy(env->content);
  free(env);
}

/** Allocation d'une chaine gérée par le GC.
 * \param[in,out] gc le garbage collector.
 * \param[in] size la taille de la chaine
 * \return un pointeur sur la chaine
 */
string_t *gc_alloc_string(gc_t *gc, unsigned int length) {
  string_t *str = malloc(sizeof(string_t));
  assert(str != NULL);

  str->length = length;
  str->str = malloc((length+1)*sizeof(char));
  str->str[length] = '\0';
  str->gc_mark = gc->current_mark; // string non-marquée initialement

  gc_cell_t *cell = gc_alloc_cell(gc);
  cell->type = T_STRING;
  cell->content.as_string = str;
  return str;
}

static void gc_delete_string(string_t *str) {
  free(str->str);
  free(str);
}

#define T_HEAP 666

/** Initialisation de l'état initial du gestionaire automatique de mémoire (GC).
 * \param debug_gc GC en mode debug (1) ou non (0)
 * \param collection_frequency indique la fréquence de la récupération mémoire.
 * Cette fréquence est comptée en nombre d'instructions exécutées par la VM. 
*/
gc_t * init_gc(int debug_gc, int collection_frequency) {
  gc_t * gc = (gc_t *) malloc(sizeof(gc_t));
  assert(gc!=NULL);
  
  gc->debug_gc = debug_gc;
  gc->current_mark         = 0;
  // le tas est circulaire
  gc->heap.type = T_HEAP;
  gc->heap.next = NULL;
  gc->nb_allocated         = 0;
  gc->collection_frequency = collection_frequency;

  if(gc->debug_gc) {
    printf("[GC] Initialized with frequency = %d\n",collection_frequency);
  }

  return gc;
}

