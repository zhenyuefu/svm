/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "value.h"

/** Préparation d'une valeur Unit.
 * \param[in,out] value la valeur à préparer.
 */
void value_fill_unit(value_t *value) {
  value->type = T_UNIT;
  value->data.as_int = -1;
}

/** Préparation d'une valeur contenant une référence à une primitive.
 * \param[in,out] value la valeur à préparer.
 */
void value_fill_prim(value_t *value, int prim_number) {
  value->type = T_PRIM;
  value->data.as_int = prim_number;
}

/** Préparation d'une valeur contenant une fermeture.
 * \param[in,out] cell la cellule à préparer.
 * \param func_pc le compteur de programme de la fonction "enfermée".
 * \param[in] frame la fenêtre capturée par la fermeture.
 */
void value_fill_closure(value_t *value, closure_t closure) {
  value->type = T_FUN;
  value->data.as_closure = closure;
}

/** Préparation d'une valeur de type entier.
 * \param[in,out] value la valeur à préparer
 * \param value la valeur entière de la valeur.
 */
void value_fill_int(value_t *value,  int num) {
  value->type = T_INT;
  value->data.as_int = num;
}

/** Préparation d'une valeur de type booléen.
 * \param[in,out] value la valeur à préparer
 * \param value la valeur booléenne de la valeur.
 */
void value_fill_bool(value_t *value,  int flag) {
  value->type = T_BOOL;
  value->data.as_int = flag;
}

/** Préparation de la valeur #t */
void value_fill_true(value_t *value) {
  value_fill_bool(value, 1);
}

/** Préparation de la valeur #f */
void value_fill_false(value_t *value) {
  value_fill_bool(value, 0);
}

/** Préparation d'une valeur de type paire.
 * Remarque : La valeur est initialisée comme paire vide (ou liste vide).
 * \param[in,out] value la valeur à préparer
 * \param[in] pair la paire à associer à la valeur
 */
void value_fill_nil(value_t *value) {
  value->type = T_PAIR;
  value->data.as_pair  = NULL;
}

/** Tester si la valeur est de type paire. */
int value_is_pair(value_t *value)    { return value->type == T_PAIR; }

/** Tester si la valeur est de type primitive. */
int value_is_prim(value_t *value)    { return value->type == T_PRIM; }

/** Tester si la valeur est de type fermeture. */
int value_is_closure(value_t *value) { return value->type == T_FUN; }

/** Tester si la valeur est de type entier. */
int value_is_int(value_t *value)     { return value->type == T_INT; }

/** Tester si la valeur est de type booléen. */
int value_is_bool(value_t *value)    { return value->type == T_BOOL; }

/** Tester si la valeur est la paire vide */
int value_is_nil(value_t *value)  { 
  assert(value->type == T_PAIR); 
  return value->data.as_pair == NULL;
}

/** Tester si la valeur est #t */
int value_is_true(value_t *value)  { 
  assert(value->type == T_BOOL); 
  return value->data.as_int != 0;
}

/** Tester si la valeur est #f */
int value_is_false(value_t *value)  { 
  assert(value->type == T_BOOL); 
  return value->data.as_int == 0;
}

/** Récupérer la valeur entière */
int value_int_get(value_t *value) {
  assert(value->type == T_INT);
  return value->data.as_int;
}

/** Récupérer le numéro de primitive */
int value_prim_get(value_t *value) {
  assert(value->type == T_PRIM);
  return value->data.as_int;
}

/** Récupérer la fermeture */
closure_t value_closure_get(value_t *value) {
  assert(value->type == T_FUN);
  return value->data.as_closure;
}

/** Récupérer la paire */
pair_t * value_pair_get(value_t *value) {
  assert(value->type == T_PAIR);
  return value->data.as_pair;
}


/** Récupérer le car (premier élément) d'une valeur de type paire.
 * \return un pointeur sur la valeur du car.
 */
value_t *value_get_car(value_t *value) {
  // Précondition: la valeur est une paire, et elle n'est pas vide.
  assert(value_is_pair(value) && value->data.as_pair);

  return &(value->data.as_pair->car);
}

/** Récupérer le cdr (second élément) d'une valeur de type paire.
 * \return un pointeur sur la valeur du cdr.
 */
value_t *value_get_cdr(value_t *value) {
  // Précondition: la valeur est une paire, et elle n'est pas vide
  assert(value_is_pair(value) && value->data.as_pair);

  return &(value->data.as_pair->cdr);
}

extern pair_t * gc_alloc_pair(struct _vm *vm);

/** Pour une valeur de type paire, assigner le car (premier élément).
 * \param[in,out] value la valeur à modifier.
 * \param[in] car le nouveau car pour la valeur.
 */  
void value_set_car(struct _vm *vm, value_t *value, value_t *car) {
  // Précondition: la valeur est une paire
  assert(value_is_pair(value));
  pair_t *pair = value->data.as_pair;

  // si on n'a pas encore alloué de couple, on le fait maintenant
  if(pair==NULL)  {
    pair = (pair_t *) gc_alloc_pair(vm);
    value->data.as_pair = pair; 
  }
  
  pair->car = *car;
}

/** Pour une valeur de type liste (paire), assigner le cdr (second élément).
 * \param[in,out] value la valeur à modifier.
 * \param[in] cdr le cdr pour la valeur.
 */ 
void value_set_cdr(struct _vm *vm, value_t *value, value_t *cdr) {
  assert(value_is_pair(value));
  pair_t *pair = value->data.as_pair;

  // si on n'a pas encore alloué de couple, on le fait maintenant
  if(pair==NULL)  {
    pair = (pair_t *) gc_alloc_pair(vm);
    value->data.as_pair = pair; 
  }
  
  pair->cdr = *cdr;
}



/** Fonction interne d'affichage de valeur.
 * \param[in] value la valeur à afficher.
 * \param[in] in_cdr si 1 (true) alors on est dans un cdr, 0 (false) sinon
 */
static void value_print_intern(value_t *value, int in_cdr) {
  if(value->type == T_PAIR) {

    if(!in_cdr) printf("(");
    
    if(value->data.as_pair) {
      if(in_cdr) printf(" ");
      value_print_intern(&(value->data.as_pair->car), 0);
      value_print_intern(&(value->data.as_pair->cdr), 1); // dans un cdr
    }

    if(!in_cdr) printf(")");

  } else {
    if(in_cdr) printf(". ");

    switch(value->type) {
    case T_UNIT: 
      printf("<unit>"); 
      break;
    case T_PRIM: 
      printf("Primitive[%d]", value->data.as_int);
      break;
    case T_FUN: 
      printf("Closure@%d", value->data.as_closure.pc);
      printf(" - ");
      env_print(value->data.as_closure.env);
      printf(">");
      break;
    case T_INT:  
      printf("%d", value->data.as_int); 
      break;
    case T_BOOL: 
      printf(value->data.as_int ? "#t" : "#f"); 
      break;
    case T_PAIR: // déjà traité
      break;
    }
  }
}

/** Affichage d'une valeur (pour débogage).
 * \param[in] value la valeur à afficher.
 */
void value_print(value_t *value) {
  value_print_intern(value, 0); // pas dans un cdr au début
}
