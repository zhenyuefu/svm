/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _FRAME_H_
#define _FRAME_H_

/** \file frame.h
 * Représentation des cadres d'appel (call frame ou encore blocs d'activation) de fonction.
 * Un cadre d'appel est associé à chaque appel de fonction (ou de fermeture).
 * Ce cadre contient :
 * - un environnement local pour les variables lexicales
 *   (potentiellement chaîné avec un environnement englobant)
 * - une zone de pile pour stocker les résultats intermédiaires des calculs.
 * - le cadre de pile de l'appelant (au premier appel : cadre de pile du top-niveau).
 * - le compteur de programme de l'appelant (pour pouvoir retourner au bon endroit).
 *
 * Remarque : il s'agit probablement de la stucture de donnée la plus importante de la VM.
 */

#include "env.h"

/** Structure des cadres de pile.
 */

typedef struct _frame {
  env_t *env;    /*!< l'environnement lexical du cadre d'appel. */         
  unsigned int sp;      /*!< le pointeur de pile */
  unsigned int pc;      /*!< le PC de l'appelant pour le retour de fonction */
  struct _frame *caller_frame; /*!< le cadre d'appel de l'appelant (ou cadre parent) */         
} frame_t;

/* Manipulation des cadres d'appel */

frame_t *frame_push(frame_t *caller_frame,
                    env_t *env,
                    unsigned int sp,
                    unsigned int pc);

frame_t *frame_pop(frame_t *frame);

void frame_print(frame_t *frame);


#endif
