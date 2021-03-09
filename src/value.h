/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _VALUE_H_
#define _VALUE_H_

/** \file value.h
 *  Représentation des valeurs.
 *  
 *  Une valeur est soit :
 *  - rien  (c'est le cas de la valeur "vide" utilisée comme type unit, également pour la liste vide).
 *  - un entier
 *  - un booléen #t ou #f
 *  - une paire (car,cdr)
 *  - un numéro de primitive
 *  - une fermeture
 */

/* références en avant */
struct _vm;
struct _pair;
struct _env;

/** Structure pour les fermetures.
 */
typedef struct {
  int         pc;  /*!<  Compteur de programme pour le corps de la fermeture. */
  struct _env *env;  /*!<  Environnement lexical capturé par la fermeture. */
} closure_t;

/** Données associées à une valeur */
union _value_data {                       
  int             as_int;     /*!< si entier (T_INT), No de primitive (T_PRIM), ou booléen (T_BOOL) */
  struct _pair    *as_pair;    /*!< si c'est une paire (T_PAIR) */
  closure_t  as_closure; /*!< si c'est une fermeture (T_CLOSURE) */
};

/** Représentation d'une valeur.
 */
typedef struct {
  int             type;  /*!< le type de la valeur */
  union _value_data data; /*!< les données supplémentaires, en fonction du type. */
} value_t;


/** Représentation des paires car/cdr.
Les allocations/désallocations de paires sont gérées par
le garbage collector (GC).
 */
typedef struct _pair {
  value_t car;  /*!< premier élément de la paire. */
  value_t cdr;  /*!< second élément de la paire. */
  int gc_mark;   /*!< la valeur de la marque (0 ou 1). */
} pair_t;


/* 
 * Initialiseurs
 */

void value_fill_unit(value_t *value);
void value_fill_prim(value_t *value, int prim_number);
void value_fill_closure(value_t *value, closure_t closure);
void value_fill_int(value_t *value,  int num);
void value_fill_bool(value_t *value, int flag);
void value_fill_true(value_t *value);
void value_fill_false(value_t *value);
void value_fill_nil(value_t *value);

/*
 * Reconnaisseurs
 */

int value_is_pair(value_t *value);
int value_is_prim(value_t *value);
int value_is_closure(value_t *value);
int value_is_int(value_t *value);
int value_is_bool(value_t *value);

/*
 * Accesseurs
 */

int value_int_get(value_t *value);
int value_prim_get(value_t *value);
int value_is_true(value_t *value);
int value_is_false(value_t *value);
closure_t value_closure_get(value_t *value);
pair_t * value_pair_get(value_t *value);

/*
 * Manipulation des paires.
 */

value_t *value_get_car(value_t *value);
value_t *value_get_cdr(value_t *value);
void value_set_car(struct _vm * vm, value_t *value, value_t *car);
void value_set_cdr(struct _vm * vm, value_t *value, value_t *cdr);


/*
 * Fonction d'affichage
 */

void value_print(value_t *value);

#endif
