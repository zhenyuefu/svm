/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "gc.h"
#include "value.h"
#include "varray.h"
#include "vm.h"
#include "constants.h"

/** \file prim.c
 * Implémentation des primitives
 ******/

/** Retourne l'élément neutre d'une primitive arithmétique. */
static int arith_neutral(int prim) {
  switch(prim) {
  case P_ADD:
  case P_SUB: return 0;
  case P_MUL:
  case P_DIV: return 1;
  default: {
    printf("unknow primitive: %d\n", prim);
    abort();
  }
  }
}

/** Application d'une primitive arithmétique. */
static int apply_arith_prim(int prim, int n1, int n2) {
  switch(prim) {
  case P_ADD: return (n1 + n2);
  case P_SUB: return (n1 - n2);
  case P_MUL: return (n1 * n2);
  case P_DIV: return (n1 / n2);
  default: {
    printf("unknow primitive: %d\n", prim);
    abort();
  }
  }
}

/** Application itérée d'un opérateur arithmétique binaire sur n opérandes
 * situés sur la zone de pile.
 * \param[in,out] stack la zone de pile concernée.
 * \param n le nombre d'opérandes (>=1).
 * \param op l'opérateur (primitive) à appliquer.
 */
void do_arith_prim(varray_t *stack, int n, int op) {
  int r;
  assert(n>0);
  // cas spécifique à un seul argument
  // (- n) == -n
  // (/ n) == 1/n
  if(n == 1) {
    r = apply_arith_prim(op, 
                         arith_neutral(op), 
                         value_int_get(varray_top(stack)));
  } else { // au moins deux arguments
    int i;
    r = value_int_get(varray_top(stack));
		
    for(i=1; i<n; i++) {
      r = apply_arith_prim(op, r, value_int_get(varray_top_at(stack, i)));
    }
    varray_popn(stack, n-1);
  }

  //varray_expandn(stack, 1);
  value_fill_int(varray_top(stack), r);
}


/** Primitive d'égalité
 * \param[in,out] stack la zone de pile concernée.
 */
void do_eq_prim(varray_t *stack) {
  int r = 0; // par défaut le résultat est faux
  
  // Tester si les types des arguments sont égaux
  if(varray_top_at(stack,0)->type 
     == varray_top_at(stack, 1)->type) {
    switch(varray_top_at(stack,0)->type) {
      // pour les "types entiers" (bool, int, char), on compare simplement la valeur
    case T_BOOL:
      r = (value_bool_get(varray_top_at(stack, 0)) ==
           value_bool_get(varray_top_at(stack, 1)));
      break;
    case T_INT:
      r = (value_int_get(varray_top_at(stack, 0))
           == value_int_get(varray_top_at(stack, 1)));
      break;
    case T_CHAR:
      r = (value_char_get(varray_top_at(stack, 0)) ==
           value_char_get(varray_top_at(stack, 1)));
      break;
    case T_STRING: // dans le cas des chaines par contre, il faut utiliser l'égalité structurelle
      r = strcmp(value_string_get(varray_top_at(stack, 0))->str,
                 value_string_get(varray_top_at(stack, 1))->str) == 0;
      break;
    case T_PAIR:  // pour les paires ce n'est pas encore implémenté
      printf("Implement me: compare two pair\n");
      abort();
      break;
      // pour le reste on ne peut comparer par égalité (?)
    default:
      printf("Unable to apply eq with type: %d\n", 
             varray_top_at(stack, 0)->type);
      abort();
    }
    
    // on dépile tous les arguments (moins 1).
    varray_popn(stack, 1);
    // et on place le résultat en sommet de pile
    value_fill_bool(varray_top_at(stack, 0), r);  
  }
  else { // sinon les types sont différents
    /* printf("Unable to apply eq with types: %d and %d\n",  */
    /*        varray_top_at(stack, 0)->type, */
    /*        varray_top_at(stack, 1)->type); */
    /* abort(); */
    varray_popn(stack, 1);
    value_fill_bool(varray_top_at(stack, 0), 0);
  }
}

/** Primitive de test à zéro
 * \param[in,out] stack la zone de pile concernée.
 */
void do_zerop_prim(varray_t *stack) {
  int r = 0; // par défaut le résultat est faux
  
  if(varray_top_at(stack,0)->type == T_INT) {
      r = (value_int_get(varray_top_at(stack, 0))
           == 0);
      // on ne dépile rien et on remplace la tête
      // par le résultat
      value_fill_bool(varray_top_at(stack, 0), r);  
  } else {
    printf("Unable to apply `zerop` with type: %d\n", 
	   varray_top_at(stack, 0)->type);
    abort();
  }
}

/** Vérifie si une valeur est un entier
 * \param[in,out] stack la zone de pile concernée.
 */
void do_integerp_prim(varray_t *stack) {
  value_t *v = varray_top(stack);
  value_fill_bool(varray_top(stack), value_is_int(v));
}

/** Primitive de test lt
 * \param[in,out] stack la zone de pile concernée.
 */
void do_lt_prim(varray_t *stack) {
  int i1 = value_int_get(varray_top_at(stack, 0));
  int i2 = value_int_get(varray_top_at(stack, 1));
  varray_pop(stack);
  value_fill_bool(varray_top(stack), i1 < i2);
}

/** Primitive de construction
 * \param[in,out] vm l'état de la machine.
 * \param[in,out] stack la zone de pile concernée.
 */
void do_cons_prim(vm_t *vm, varray_t *stack) {
  // C'est facile mais il faut construire la paire
  // directement dans la zone de pile pour qu'elle soit
  // accessible à tout moment par le GC.

  // en sommet de pile on trouve les deux arguments: [car cdr ...]
  
  // on ajoute une place pour le résultat (la paire construite)
  varray_expandn(stack, 1);   
  // initialiser une paire vide
  value_fill_nil(varray_top(stack));   
  // on place le car
  value_set_car(vm,varray_top(stack), varray_top_at(stack, 1)); 
  // et le cdr
  value_set_cdr(vm,varray_top(stack), varray_top_at(stack, 2)); 
  // puis on copie le résultat -> [res cdr res ...]
  varray_set_top_at(stack, 2, varray_top(stack)); 
  // et finalement on enlève les deux premiers éléments -> [res ...]
  varray_popn(stack, 2);                                         
  
}

/** Construction de liste
 * \param[in,out] vm l'état de la machine.
 * \param[in,out] stack la zone de pile concernée.
 * \param n le nombre d'opérandes (>=1).
 */
void do_list_prim(vm_t * vm, varray_t *stack, int n) {
  int i;
  
  // ici la pile contient les éléments
  // de la liste avec le dernier élément au sommet [cadddXXXr .... cadddr caddr cadr car]
  // on ajoute deux éléments au sommet
  varray_expandn(stack, 2);       
  // le premier est le résultat
  value_fill_nil(varray_top_at(stack, 0));

  for(i=0; i<n; i++) { // on itère pour tous les éléments
    // copier le résultat (au sommet) sur le second
    varray_set_top_at(stack, 1,varray_top_at(stack, 0));
    // on reconstruit une liste au sommet
    value_fill_nil(varray_top(stack)); 
    // on va chercher le car en  n-i-1+2  et on le met au sommet
    value_set_car(vm,varray_top(stack), 
                  varray_top_at(stack, n - i + 1)); 
    // et le cdr est le reste de la construction (le résultat)
    value_set_cdr(vm,varray_top(stack), varray_top_at(stack, 1));

  }
  
  // on copie le résultat (au sommet) à l'index n - 1 + 2 
  varray_set_top_at(stack, n + 1,varray_top(stack));

  // on a donc sur la pile: [temp1 temp2 car cadr ....  res]   (temp1 en sommet de pile)
  // on dépile les n-1+2 premiers éléments et il reste donc sur la pile: [res]
  varray_popn(stack, n + 1); 
}

/** Premier élément d'une paire.
 * \param[in,out] stack la zone de pile concernée.
 */
void do_car_prim(varray_t *stack) {
  varray_set_top(stack, value_get_car(varray_top(stack)));  
}

/** Second élément d'une paire.
 * \param[in,out] stack la zone de pile concernée.
 */
void do_cdr_prim(varray_t *stack) {
  varray_set_top(stack, value_get_cdr(varray_top(stack)));  
}

/** Vérifie si une liste est vide
 * \param[in,out] stack la zone de pile concernée.
 */
void do_nullp_prim(varray_t *stack) {
  value_t *v = varray_top(stack);
  int res;
  if(value_is_pair(v) && !value_pair_get(v)) res = 1;
  else res = 0;
  value_fill_bool(varray_top(stack), res);
}

/** Vérifie si une liste est non vide
 * \param[in,out] stack la zone de pile concernée.
 */
void do_pairp_prim(varray_t *stack) {
  value_t *v = varray_top(stack);
  int res;
  if (value_is_pair(v) && value_pair_get(v)) res = 1;
  else res = 0;
  value_fill_bool(varray_top(stack), res);
}

/** Change le car d'une paire
 * \param[in,out] stack la zone de pile concernée.
 */
void do_setcar_prim(vm_t *vm, varray_t *stack) {
  value_t *v = varray_top(stack);
  value_set_car(vm, varray_top(stack), varray_top_at(stack, 1));
  value_fill_unit(varray_top_at(stack, 1));
  varray_pop(stack);
}

/** Change le cdr d'une paire
 * \param[in,out] stack la zone de pile concernée.
 */
void do_setcdr_prim(vm_t *vm, varray_t *stack) {
  value_t *v = varray_top(stack);
  value_set_cdr(vm, varray_top(stack), varray_top_at(stack, 1));
  value_fill_unit(varray_top_at(stack, 1));
  varray_pop(stack);
}

/** Renvoie le code ASCII d'un caractère
 * \param[in,out] stack la zone de pile concernée.
 */
void do_chartointeger_prim(varray_t *stack) {
  char c = value_char_get(varray_top(stack));
  value_fill_int(varray_top(stack), c);
}

/** Renvoie le charactère correspondant à un code ASCII
 * \param[in,out] stack la zone de pile concernée.
 */
void do_integertochar_prim(varray_t *stack) {
  int i = value_int_get(varray_top(stack));
  value_fill_char(varray_top(stack), i);
}

/** Vérifie si une valeur est un caractère
 * \param[in,out] stack la zone de pile concernée.
 */
void do_charp_prim(varray_t *stack) {
  value_t *v = varray_top(stack);
  value_fill_bool(varray_top(stack), value_is_char(v));
}

/** Construit une chaine de caractères
 * \param[in,out] gc le garbage collector
 * \param[in,out] stack la zone de pile concernée
 * \param[in] n la longeur de la chaine
 */
void do_string_prim(gc_t *gc, varray_t *stack, int n) {
  string_t *s = gc_alloc_string(gc, n);
  for(int i = 0; i < n; i++) {
    s->str[i] = value_char_get(varray_top_at(stack, i));
  }
  if(n == 0) varray_expandn(stack, 1);
  else varray_popn(stack, n-1);
  value_fill_string(varray_top(stack), s);
}

/** Optient la longeur de la chaine
 * \param[in,out] stack la zone de pile concernée
 */
void do_stringlength_prim(varray_t *stack) {
  string_t *s = value_string_get(varray_top(stack));
  value_fill_int(varray_top(stack), s->length);
}

/** Optient le n-ieme caractère de la chaîne
 * \param[in,out] stack la zone de pile concernée
 */
void do_stringref_prim(varray_t *stack) {
  string_t *s = value_string_get(varray_top_at(stack, 0));
  int n = value_int_get(varray_top_at(stack, 1));

  if(n >= s->length) {
    printf("string-length: index %d out of bounds", n);
    abort();
  }

  varray_popn(stack, 1);
  value_fill_char(varray_top(stack), s->str[n]);
}

/** Vérifie si une valeur est un chaine de caractères
 * \param[in,out] stack la zone de pile concernée.
 */
void do_stringp_prim(varray_t *stack) {
  value_t *v = varray_top(stack);
  value_fill_bool(varray_top(stack), value_is_string(v));
}

/** Converti un nombre en chaîne de caractères
 * \param[in,out] gc le garbage collector
 * \param[in,out] stack la zone de pile concernée
 */
void do_numbertostring_prim(gc_t *gc, varray_t *stack) {
  int n = value_int_get(varray_top(stack));
  int length = snprintf(NULL, 0, "%d", n);
  string_t *s = gc_alloc_string(gc, length+1);
  snprintf(s->str, length + 1, "%d", n);
  value_fill_string(varray_top(stack), s);
}

/** Affichage d'une valeur.
 * \param[in,out] stack la zone de pile concernée
 * \param[in] n nombre de paramètres
 */
void do_display_prim(varray_t *stack, int n) {
  if (n == 1) {
    value_print(stdout, varray_top(stack));
  } else if (n == 2) {
    value_print(value_port_get(varray_top_at(stack, 1)), varray_top(stack));
    varray_pop(stack);
  } else {
    printf("Incorrect number of arguments for display : should be 1 or 2, got %d\n", n);
    abort();
  }

  // Always push <unit> on top of the stack at the end
  value_fill_unit(varray_top(stack));
}

/** Ouverture d'un fichier en écriture
 * \param[in,out] stack la zone de pile concernée
 */
void do_openfile_prim(varray_t *stack, char *mode) {
  char *filename = value_string_get(varray_top(stack))->str;

  FILE *fd = fopen(filename, mode);

  if(!fd) {
    printf("Error opening file %s\n", filename);
    exit(1);
  }

  value_fill_port(varray_top(stack), fd);
}

/** Fermeture d'un fichier
 * \param[in,out] stack la zone de pile concernée
 */
void do_closeport_prim(varray_t *stack) {
  fclose(value_port_get(varray_top(stack)));
  value_fill_unit(varray_top(stack));
}

/** Lire un caractère depuis un fichier
 * \param[in,out] stack la zone de pile concernée
 */
void do_readchar_prim(varray_t *stack) {
  FILE *fd = value_port_get(varray_top(stack));
  char c = (char)fgetc(fd);
  value_fill_char(varray_top(stack), c);
}

char peek(FILE *fd, int n) {
  char buf[n];
  char res;
  for(int i = 0; i < n; i++) buf[i] = fgetc(fd);
  res = fgetc(fd);
  ungetc(res, fd);
  for(int i = n - 1; i >= 0; i--) ungetc(buf[i], fd);
  return res;
}

/** Lire un caractère depuis un fichier, dans déplacer la tète de lecture
 * \param[in,out] stack la zone de pile concernée
 * \param[n] le nombre d'arguments. Le deuxième argument (potenciel) est la position
 * du caractère à lire par rapport à la tète de lecture
 */
void do_peekchar_prim(varray_t *stack, int n) {
  FILE *fd = value_port_get(varray_top(stack));
  int pos;
  if(n == 1) pos = 0;
  else {
    pos = value_int_get(varray_top_at(stack, 1));
    varray_pop(stack);
  }
  value_fill_char(varray_top(stack), peek(fd, pos));
}

/** Vérifie si une valeur correspond au caractère EOF (-1)
 * \param[in,out] stack la zone de pile concernée
 */
void do_eofp_prim(varray_t *stack) {
  value_t *v = varray_top(stack);
  int iseof = 0;
  if (value_is_char(v) && value_char_get(v) == -1) iseof = 1;
  value_fill_bool(varray_top(stack), iseof);
}

/** Applique une fonction sur une liste d'arguments
 * \param[in,out] vm l'état de la vm
 * \param[in,out] stack la zone de pile concernée
 */
void do_apply_prim(vm_t *vm, varray_t *stack) {
  value_t *fun = varray_top(stack);
  value_t *args = varray_top_at(stack, 1);

  // fun | args | ...

  // Calculate the number of args
  int nb_args = 0;
  while(value_is_pair(args) && value_pair_get(args)) {
    nb_args++;
    args = &(value_pair_get(args)->cdr);
  }

  // expand the stack
  varray_expandn(stack, nb_args);

  // _ | ... | _ | fun | args | ...

  varray_set_top(stack, varray_top_at(stack, nb_args));
  varray_set_top_at(stack, 1, varray_top_at(stack, nb_args+1));

  // fun | args | _ | ... | _ | ...

  // put the args on the stack in the correct order
  args = varray_top_at(stack, 1);
  for(int i = 0; i < nb_args; i++) {
    varray_set_top_at(stack, i+2, &value_pair_get(args)->car);
    args = &value_pair_get(args)->cdr;
  }

  // fun | args | arg1 | ... | argn | ...

  fun = varray_top(stack);
  varray_popn(stack, 2);

  // arg1 | ... | argn | ...

  // Call the function
  vm_call(vm, fun, nb_args);

  // res | ...
}

/** Lève une erreur, et arrète le programme
 * \param[in,out] stack la zone de pile concernée
 */
void do_error_prim(varray_t *stack) {
  string_t *s = value_string_get(varray_top(stack));
  printf("error : %s\n", s->str);
  exit(1);
}

/** Arrète le programme avec un code d'erreur donné
 * \param[in,out] stack la zone de pile concernée
 */
void do_exit_prim(varray_t *stack) {
  int i = value_int_get(varray_top(stack));
  exit(i);
}

/** Renvoie les arguments de la ligne de commandes, dans une liste
 * \param[in] vm l'état actuel de la VM
 * \param[in,out] stack la zone de pile concernée
 */
void do_currentcmdargs_prim(vm_t *vm, varray_t *stack) {
  varray_expandn(stack, 1);
  value_fill_nil(varray_top(stack));

  for(int i = vm->argc-1; i >= 0; i--) {
    char *str = vm->argv[i];
    string_t *s = gc_alloc_string(vm->gc, strlen(str));
    strcpy(s->str, str);

    pair_t *pair = gc_alloc_pair(vm);
    varray_expandn(stack, 1);
    value_fill_string(varray_top(stack), s);
    do_cons_prim(vm, stack);
  }
}

/** Exécution d'une primitive.
 * Les arguments sont sur la pile du cadre d'appel (frame) courant
 * (premier argument au sommet, etc.).
 * \param[in,out] stack la zone de pile.
 * \param prim le numéro de la primitive à invoquer.
 * \param n le nombre d'arguments à dépiler.
 */
void execute_prim(vm_t *vm, varray_t *stack, int prim, int n) {
  switch(prim) {
    
    // Les fonctions arithmétiques sont traitées d'un coup.
  case P_ADD: 
  case P_SUB: 
  case P_MUL: 
  case P_DIV: 
    do_arith_prim(stack, n, prim); break;
    
    // égalité
  case P_EQ:
    do_eq_prim(stack); break;
    break;

    // <
  case P_LT:
    do_lt_prim(stack); break;
    break;

    // Construire une nouvelle paire.
  case P_CONS:
    do_cons_prim(vm,stack); break;
    break;
    
    // On construit une liste, donc une suite de paires:
  case P_LIST: 
    do_list_prim(vm,stack,n); break;

    // car 
  case P_CAR:
    do_car_prim(stack); break;

    // cdr
  case P_CDR:
    do_cdr_prim(stack); break;

    // null?
  case P_NULLP:
    do_nullp_prim(stack); break;
    // pair?
  case P_PAIRP:
    do_pairp_prim(stack); break;

  case P_SETCAR:
    do_setcar_prim(vm, stack); break;
  case P_SETCDR:
    do_setcdr_prim(vm, stack); break;

  case P_ZEROP:
    do_zerop_prim(stack); break;
  case P_INTEGERP:
    do_integerp_prim(stack); break;

    // char->integer
  case P_CHARTOINTEGER:
    do_chartointeger_prim(stack); break;
    // integer->char
  case P_INTEGERTOCHAR:
    do_integertochar_prim(stack); break;
  case P_CHARP:
    do_charp_prim(stack); break;

    // Make a string
  case P_STRING:
    do_string_prim(vm->gc, stack, n); break;
    // String length
  case P_STRINGLENGTH:
    do_stringlength_prim(stack); break;
  case P_STRINGREF:
    do_stringref_prim(stack); break;
  case P_NUMBERTOSTRING:
    do_numbertostring_prim(vm->gc, stack); break;
  case P_STRINGP:
    do_stringp_prim(stack); break;

    // Display a value
  case P_DISPLAY:
    do_display_prim(stack, n); break;

    // Open an output port
  case P_OPENOUTPUTFILE:
    do_openfile_prim(stack, "w"); break;
    // Open an input port
  case P_OPENINPUTFILE:
    do_openfile_prim(stack, "r"); break;
    // Close an input/output port
  case P_CLOSEINPUTPORT:
  case P_CLOSEOUTPUTPORT:
    do_closeport_prim(stack); break;

    // Read a character
  case P_READCHAR:
    do_readchar_prim(stack); break;
    // Peek on a character
  case P_PEEKCHAR:
    do_peekchar_prim(stack, n); break;
    // Vérifier si un caractère est eof
  case P_EOFP:
    do_eofp_prim(stack); break;

  case P_APPLY:
    do_apply_prim(vm, stack); break;

  case P_ERROR:
    do_error_prim(stack); break;
  case P_EXIT:
    do_exit_prim(stack); break;

  case P_CURRENTCMDARGS:
    do_currentcmdargs_prim(vm, stack); break;

  default:
    printf("unknow primitive: %d with %d args\n", prim, n);
    abort();
  }
}
