/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#ifndef _BYTECODE_H_
#define _BYTECODE_H_

/** \file bytecode.h
 * Représentation du code-octet (bytecode)
 * encodant les programmes compilés.
 *
 * Le jeu d'instruction proposé est le suivant :
 * - GALLOC : allouer une entrée dans l'environnement des variables globales.
 * - GSTORE ref : stocker le sommet de pile à la référence indiquée dans l'environnement global.
 * - GFETCH ref : placer en sommet de pile la valeur de la variable globale référencée.
 * - STORE ref : placer la valeur lexicale référencée en sommet de pile  (environnement local).
 * - FETCH ref : placer en sommet de pile la référence lexicale indiquée (environnement local).
 * - PUSH type val  : placer en sommet de pile une valeur immédiate du type indiqué.
 * - POP : dépiler le sommet de pile (la valeur est perdue)
 * - CALL : appeler la fonction ou la primitive stockée en sommet de pile (et dépiler).
 * - RETURN : retour de fonction
 * - JUMP pc : saut inconditionnel vers pc.
 * - JFALSE pc : saut vers pc à condition que le sommet de pile soit faux (et dépiler).
 */

/** Structure du segment de byte-code.
 */
typedef struct _program {
  int * bytecode;     /*!< le contenu du segment de byte-code. */ 
  unsigned int size;  /*!< la taille segment. */
} program_t;

/* Fonction de manipulations du bytecode */

void bytecode_read(program_t *program, const char *filename);
void bytecode_destroy(program_t *program);
void bytecode_print(program_t *program);
int bytecode_print_instr(program_t *program, unsigned int pc);

#endif
