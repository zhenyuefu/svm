/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "env.h"

value_t *env_search(env_t *env, unsigned int pos) {
  while(env) { // tant qu'il reste un environnement dans la chaîne
    // si l'élément est dans l'environnement courant
    if(pos < env->content->top)              
      // on le retourne
      return varray_at(env->content, pos);    
    // sinon, on va essayer dans le prochain environnement
    // (et on remarque que la recherche est "accélérée").
    pos -= env->content->top;                
    env = env->next;
  }
  printf("env_search: index outside environment (please report)");
  abort();
  return 0;
}


/** Accéder à une cellule de l'environnement.
 * Remarque : la référence peut conduire à traverser la pile
 * des environnements.
 * \param[in] env l'environnement concerné.
 * \param pos l'index de la cellule à accéder.
 * \return la cellule accédée 
 * Remarque : le programme s'arrêt si la cellule 
 * n'existe pas.
 */
value_t *env_fetch(env_t *env, unsigned int pos) {
  value_t * value = env_search(env,pos);
  return value;
}


void env_store(env_t *env, unsigned int pos, value_t *nvalue) {
  value_t * ovalue = env_search(env,pos);
  *ovalue = *nvalue; 
}


/** Affichage des environnements (pour déboguage). */
void env_print(env_t *env) {
  printf("<");
  while(env) {
    varray_print(env->content);
    env = env->next;
    if(env)
      printf("=>");
  }
  printf(">");
}
