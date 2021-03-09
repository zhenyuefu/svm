/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <stdio.h>

#include "vm.h"

/** \file gc_mark.c
 * Implémentation de l'algorithme de marquage par le GC.
 */

static void pair_mark_and_trace(gc_t *gc, pair_t *pair);
static void closure_mark_and_trace(gc_t *gc, closure_t *closure);

/** Marquage des valeurs simples.
 * Remarque : les valeurs ne sont marquées explicitement
 * que s'il s'agit de paires ou de fermetures. Ce sont
 * les deux seuls cas qui nécessitent l'emploi du GC
 * dans cette version de la VM.
 */
static void value_mark_and_trace(gc_t * gc, value_t *value) {
  if(value_is_pair(value)) {
    // marquer la paire
    pair_mark_and_trace(gc, value->data.as_pair);
  } else if(value_is_closure(value)) {
    // marquer la fermeture.
    closure_mark_and_trace(gc, &(value->data.as_closure)); 
  } // les autres types de valeur ne sont pas gérés par le GC
}

/** Traçage et marquage du contenu d'une paire */
static void pair_mark_and_trace(gc_t * gc, pair_t *pair) {
  if(pair!=NULL) { // Remarque : la paire vide est NULL
    if(pair->gc_mark != gc->current_mark) {
      // la paire n'est pas encore marquée
      if(gc->debug_gc) {
        printf("[GC]       ==> 1 pair marked\n");
      }
      pair->gc_mark = gc->current_mark;
      // marquer/tracer le car
      value_mark_and_trace(gc, &(pair->car));
      // marquer/tracer le cdr  (qui n'est pas forcément une paire !)
      value_mark_and_trace(gc, &(pair->cdr));
    } // sinon rien à faire
  }
}

/** Marquage d'un tableau de valeurs.
 */
void varray_mark_and_trace(gc_t * gc, varray_t *varray) {
  int i;
  // Le procédé consiste à marquer (et tracer) individuellement les valeurs.
  for(i=0; i<varray->top; i++) {
    value_mark_and_trace(gc, &varray->content[i]);
  }
}

void env_mark_and_trace(gc_t *gc, env_t *env) {
  if(env) {
    if(env->gc_mark != gc->current_mark) {
      if(gc->debug_gc) {
        printf("[GC]       ==> 1 env marked\n");
      }
      env->gc_mark = gc->current_mark;
      varray_mark_and_trace(gc, env->content);
      env_mark_and_trace(gc, env->next);
    }
  }
}

/** Traçage et marquage d'une fermeture */
void closure_mark_and_trace(gc_t *gc, closure_t *closure) {
  env_mark_and_trace(gc, closure->env);
}



/** Marquer et tracer un cadre d'appel de fonction. */
static void frame_mark_and_trace(gc_t * gc, frame_t *frame) {
  frame_t * traced_frame = frame;
  int frame_num = 0; // pour compter les frames (debuggage)
  while(traced_frame!=NULL) { // on s'arrête récursivement en NULL (ou 0)
    frame_num++;
    if(gc->debug_gc) {
      printf("[GC]        Tracing frame #%d\n",frame_num);
    }
    // marquer/tracer l'environnement local
    env_mark_and_trace(gc, traced_frame->env);
    // et finalement marquer/tracer le cadre appelant
    traced_frame = traced_frame->caller_frame;
  }
}

  
/** Marquage/traçage depuis les racines de la machine virtuelle.
 * Il s'agit du point d'entrée pour la phase de marquage de l'algorithme de GC.
 */
void mark_and_trace_roots(vm_t *vm) {
  if(vm->gc->debug_gc) {
    printf("[GC]    Tracing roots\n");
    printf("[GC]      Tracing globals\n");
  }
  varray_mark_and_trace(vm->gc, vm->globs);
  if(vm->gc->debug_gc) {
    printf("[GC]      Tracing stack\n");
  }
  varray_mark_and_trace(vm->gc, vm->stack);

  if(vm->gc->debug_gc) {
    printf("[GC]      Tracing call frames\n");
  }
  frame_mark_and_trace(vm->gc, vm->frame);
}
