/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "vm.h"
#include "constants.h"
#include "value.h"

/** Initialisation de la machine virtuelle.
 * \param[in] program le programme en bytecode à exécuter.
 * \param[in] debug_vm VM en mode debug (1) ou non (0)
 * \param[in] debug_gc GC en mode debug (1) ou non (0)
 * \param[in] collection_frequency la fréquence de ramassage de la mémoire 
 *  par le GC.
 * \return un état initial pour la VM.
 */
vm_t * init_vm(program_t *program, int debug_vm, int debug_gc, int collection_frequency,
               int argc, char **argv) {
  vm_t * vm  = (vm_t *) malloc(sizeof(vm_t));
  assert(vm!=NULL);

  vm->debug_vm = debug_vm;
  vm->program = program;
  // initialize globals
  vm->globs = varray_allocate(GLOBS_SIZE);
  varray_expandn(vm->globs,1);
  // put a unit at index 0
  value_t value;
  value_fill_unit(&value);
  varray_set_at(vm->globs,0,&value);
  // initialize stack
  vm->stack = varray_allocate(STACK_SIZE);
  
  // initial frame
  vm->frame = frame_push(NULL, // pas de call frame parente
                         NULL, // environnement local vide
                         0,  // début de pile ... au début
                         0); // commencer par la première instruction
  
  
  // initialize GC
  vm->gc = init_gc(debug_gc, collection_frequency);

  // command line args
  vm->argc = argc;
  vm->argv = argv;
  
  return vm;
}

/** Avancer à la prochaine instruction (ou partie d'instruction) en
 * incrémentant le compteur de programme de la call frame active.
 * \param[in,out] vm l'état de la machine virtuelle.
 * \return la prochaine instruction de bytecode.
 */
int vm_next(vm_t * vm) {
  int next = vm->frame->pc;
  vm->frame->pc = vm->frame->pc + 1;
  return vm->program->bytecode[next];
}


/** Exécution par la VM d'une instruction.
 * \param[in,out] vm l'état de la machine virtuelle
 * \param[in] instr le byte-code de l'instruction à exécuter
 */
void vm_execute_instr(vm_t *vm, int instr) {
  if(vm->debug_vm) {
    printf("=== Execute next intruction ===\n");
    printf(">>> ");bytecode_print_instr(vm->program, vm->frame->pc-1);printf("\n");
  }
  
  // en fonction de l'instruction à exécuter.
  switch(instr) {
    // allocation dans l'environnement global
  case I_GALLOC:                        
    varray_expandn(vm->globs, 1);
    break;
    
    // dépiler le sommet de pile et le placer au bon endroit dans l'environnement global
  case I_GSTORE: 
    varray_set_at(vm->globs, 
                  vm_next(vm), 
                  varray_pop(vm->stack));
    break;
    
    // empiler la valeur d'une variable globale
  case I_GFETCH:
    varray_push(vm->stack, 
                varray_at(vm->globs, vm_next(vm)));
    break;
    
    // dépiler le sommet de pile et le sauvegarder dans l'environnement local
  case I_STORE:
    env_store(vm->frame->env, 
              vm_next(vm), 
              varray_pop(vm->stack));
    break;
    
    // empiler la valeur d'une variable locale
  case I_FETCH:   
    varray_push(vm->stack,  // et on recopie
                env_fetch(vm->frame->env, vm_next(vm)));
    break;
      
    // empilement d'une valeur
  case I_PUSH: {
    value_t value;
    
    switch(vm_next(vm)) {
    case T_INT: // placer un entier
      value_fill_int(&value, vm_next(vm)); 
      break;
    case T_UNIT: // placer la valeur unit
      value_fill_unit(&value); 
      break;
    case T_FUN: { // placer une fermeture
	closure_t closure;
        closure.env = vm->frame->env; // on capture l'environnement courant
        closure.pc = vm_next(vm); // le PC de la fermeture est la prochaine information
        value_fill_closure(&value, closure);
      }
      break;
    case T_PRIM: // placer un numéro de primitive
      value_fill_prim(&value, vm_next(vm));
      break;
    case T_BOOL: // place un booléen
      value_fill_bool(&value, vm_next(vm));
      break;
    case T_CHAR: // place un caractère
      value_fill_char(&value, vm_next(vm));
      break;
    case T_PAIR: // placer une paire (on ne devrait pas avoir ce cas)
      printf("No immediate pair ! (please report)\n");
      exit(EXIT_FAILURE);
      break;
    case T_STRING: // placer une chaine (on ne devrait pas avoir ce cas)
      printf("No immediate string ! (please report)\n");
      break;
    default:
	printf("Unknown type: %d (in push)\n", vm->program->bytecode[vm->frame->pc-1]);
	exit(EXIT_FAILURE);
      }
    
    // empiler la valeur
    varray_push(vm->stack,&value);
  }
    break;
    
    // dépiler
  case I_POP: {
    value_t * val = varray_pop(vm->stack);
    if(varray_empty(vm->stack) && vm->frame->caller_frame == NULL && !value_is_unit(val)) {
      // on affiche les valeurs <<popée>> au top-niveau
      if(vm->debug_vm) {
        printf("DISPLAY> ");
      }
      value_print(stdout, val);
      printf("\n");
    }
  }
    break;
    
    // appeler une fermeture (fonction) ou une primitive
  case I_CALL: {
    // récupérer la fermeture ou la primitive
    value_t *fun = varray_pop(vm->stack);
    int nb_args = vm_next(vm);

    vm_call(vm, fun, nb_args);
  }
    break;

    // retour de fonction
  case I_RETURN: {             
    // la pile contient la valeur de retour au sommet [res ...]
    value_t *res = varray_pop(vm->stack);
    
    assert(vm->stack->top>=vm->frame->sp);  // il faut se déplacer dans le bon sens

    vm->stack->top = vm->frame->sp;
    varray_push(vm->stack, res);
    vm->frame = frame_pop(vm->frame);
  }
    break;
      
    // saut inconditionnel
  case I_JUMP:                          
    vm->frame->pc = vm->program->bytecode[vm->frame->pc];
    break;
    
    // si le sommet de pile est faux, alors on effectue le saut, 
    // sinon on dépile simplement
  case I_JFALSE: {
    value_t *c = varray_pop(vm->stack);
    if(value_is_bool(c) && value_is_false(c)) {
      vm->frame->pc = vm->program->bytecode[vm->frame->pc];
    } else {
      vm_next(vm);
    }
  }
    break;
      
  default:
    printf("Unknow opcode: %d\n", vm->program->bytecode[vm->frame->pc-1]);
    exit(EXIT_FAILURE);
  }

  if(vm->debug_vm) {
    printf("State:\n");
    printf("  PC = %d\n",vm->frame->pc);
    /* printf("  Globals = "); varray_print(vm->globs); */
    /* printf("\n"); */
    printf("  Stack = "); varray_stack_print(vm->stack);
    printf("\n");
    printf("  Frame = "); frame_print(vm->frame);
    printf("\n");
  }

}

/** Appel de fonction : utilisé par l'instruction CALL et la primitive APPLY
 * \param[in,out] vm l'état de la machine virtuelle
 * \param[in] fun la fonction à appeler (primitive ou fermeture)
 * \param[in] nb_args le nombre d'arguments
 */
void vm_call(vm_t *vm, value_t *fun, int nb_args) {
  switch (fun->type) {
    // si c'est une fermeture
  case T_FUN: {
    int i;
    closure_t closure = value_closure_get(fun);
    env_t *env = gc_alloc_env(vm->gc, nb_args, closure.env);

    // recopier les arguments de la pile vers l'environnement local
    // de la fermeture
    for (i = 0; i < nb_args; i++) {
      varray_set_at(env->content, i, varray_top_at(vm->stack, i));
    }
    varray_popn(vm->stack, nb_args); // tout dépiler

    // empiler une nouvelle call frame.
    vm->frame = frame_push(vm->frame, env, vm->stack->top, vm->frame->pc);
    vm->frame->pc = closure.pc;
    break;
  }

    // Exécuter une primitive
  case T_PRIM: {
    // numéro de primitive encodée dans la valeur.
    int prim_num = value_prim_get(fun);
    // exécuter la primitive (aïe)
    execute_prim(vm, vm->stack, prim_num, nb_args);
    break;
  }

  default:
    printf("Unable to call: %d\n", fun->type);
    exit(EXIT_FAILURE);
  }
}

/** Moteur d'exécution de la machine virtuelle.
 * \param[in,out] vm l'état de la machine virtuelle
 */
void vm_execute(vm_t * vm) {
  unsigned int instr_counter = 0;

  if(vm->debug_vm) {
    printf("Initial state:\n");
    printf("  PC = %d\n",vm->frame->pc);
    printf("  Globals = "); varray_print(vm->globs);
    printf("\n");
    printf("  Stack = "); varray_stack_print(vm->stack);
    printf("\n");
    printf("  Frame = "); frame_print(vm->frame);
    printf("\n");
  }

  while(vm->frame->pc < vm->program->size) {   

    vm_execute_instr(vm, vm_next(vm));

    instr_counter = instr_counter + 1;
    
    if(instr_counter==vm->gc->collection_frequency) {
      // on force le GC
      gc_collect(vm);
      instr_counter = 0;
    }

  }

  // c'est fini
}
