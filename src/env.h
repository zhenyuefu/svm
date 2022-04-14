/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _ENV_H_
#define _ENV_H_

#include "varray.h"

/** \file env.h
 * Représentation des environnements.
 * Les environnements manipulés par la VM native sont
 * chaînés les uns aux autres. Les références générées
 * par le compilateur pour les accès aux variables lexicales
 * se traduisent naturellement dans ce chaînage.
 */

/** Le GC a besoin de distinguer les différents
 * types de cellule (l'autre étant T_PAIR) */
#define T_ENV 999

/** Structure d'un environnement (variables locales).
 */
typedef struct _env {
  int gc_mark;  /*!< marque pour le GC */
  varray_t * content; /*!< contenu de l'environnement (tableau de cellules). */
  struct _env *next; /*!< environnement suivant (ou "englobant") dans la chaîne. */
} env_t;

/* 
 * Fonctions de manipulation des environnements.
 */

value_t *env_fetch(env_t *env, unsigned int pos);    
void env_store(env_t *env, unsigned int pos, value_t *nvalue); 
void env_print(env_t *env);                    

#endif
