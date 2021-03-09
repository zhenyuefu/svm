/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "varray.h"


/** Allocation d'un tableau de valeurs.
 * Remarque : les tableaux de valeurs ne sont pas directement gérés
 * par le GC, donc on trouve l'allocateur ici.
 * \param initial_capacity la capacité (taille allouée) initiale pour le tableau
 * (de taille effective 0 au début).
 * \return un pointeur vers la structure allouée pour le tableau de valeurs.
 */
varray_t *varray_allocate(unsigned int initial_capacity) {
  varray_t *res = (varray_t *) malloc(sizeof(varray_t));

  assert(initial_capacity >= 0);

  res->content  = (value_t *) malloc(sizeof(value_t) * initial_capacity);  
  assert(res->content!=NULL);
  res->capacity = initial_capacity;                                 
  res->top      = 0; // le marqueur top à 0 indique qu'il n'y a aucun élément (taille 0)

  return res;
}


/** Extension de la taille allouée d'un tableau de valeurs.
 * \param[in,out] varray le tableau de valeurs à étendre.
 * \param n le nombre de valeurs à allouer en supplément.
 * Remarque : cette fonction étend la taille du tableau en
 * déplacement simplement son 'top'. Si ce dernier se retrouve
 * au delà de la taille allouée (capacité), alors cette dernière
 * est augmentée en conséquence.
 */
void varray_expandn(varray_t *varray, unsigned int n) {

  // déplacer le 'top'
  varray->top += n;                                    

  // si le nouveau 'top' est au delà de la capacité
  // alors on augmente cette dernière
  if(varray->top > varray->capacity) {             
    do {
      // de façon habituelle, pour ne pas réallouer
      // trop souvent, on multiplie par 2 la capacité.
      varray->capacity *= 2;                           
    } while(varray->capacity < varray->top);       
    // et on répète tant que la capacité n'est pas suffisante
    // pour englober le nouveau top
    
    // la réallocation suit
    varray->content 
      = (value_t *) realloc(varray->content, 
                            sizeof(value_t) * varray->capacity);
    assert(varray->content!=NULL);
  }
}

/** Dépilement d'éléments d'un tableau de valeur.
 * Cette fonction est utilisée lorsque le tableau de valeurs est utilisé en
 * tant que structure de pile.
 * Remarque : la taille allouée du tableau n'est pas modifiée, et les éléments
 * dépilés ne sont pas désalloués. On déplace simplement le 'top' du tableau.
 * \param[in,out] varray le tableau des valeurs à dépiler.
 * \param n le nombre de valeurs à dépiler.
 */
void varray_popn(varray_t *varray, unsigned int n) {
  // précondition : on ne veut pas que top devienne strictement négatif.
  assert(n <= varray->top);
  varray->top -= n;
}

/** Accéder à la n-ième valeur d'un tableau de valeurs. 
 * Remarque : cette fonction effectue un accès de type tableau.
 * \param[in] varray le tableau de valeurs considéré.
 * \param n l'index de la valeur accédée (0 pour le premier élément).
 * \return la n-ième valeur du tableau.
 */
value_t *varray_at(varray_t *varray, unsigned int n) {
  // précondition : l'index doit exister
  assert(n<varray->top);
  return &varray->content[n];
}

/** Modifier la n-ième case du tableau.
 * Remarque : cette fonction effectue un accès de type tableau.
 * \param[in,out] varray le tableau de valeurs considéré.
 * \param n l'index de la valeur accédée (0 pour le premier élément).
 * \param[in] value la nouvelle valeur à placer dans le tableau.
 */
void varray_set_at(varray_t *varray, unsigned int n, value_t *value) {
  // précondition : l'index doit exister
  assert(n<varray->top);
  *(varray_at(varray,n)) = *value;
}

/** Accéder à la n-ième valeur à partir du sommet de la pile. 
 * \param[in] varray le tableau de valeurs considéré comme une pile.
 * \param n le déplacement entre le sommet (top) et la valeur accédée (0 pour top).
 * \return la valeur située à "distance" n du sommet de la pile.
 */
value_t *varray_top_at(varray_t *varray, unsigned int n) {
  // précondition : l'index doit exister
  assert(n < varray->top);
  return &varray->content[varray->top - n - 1];
}

/** Accédera sommet de la pile. 
 * \param[in] varray le tableau de valeurs considéré comme une pile.
 * \return le sommet de la pile.
 */
value_t *varray_top(varray_t *varray) {
  return varray_top_at(varray,0);
}

/** Modifier la n-ième case à partir du sommet de la pile.
 * \param[in,out] varray le tableau de valeurs considéré comme une pile.
 * \param n l'index de la valeur accédée (0 pour le top).
 * \param[in] value la nouvelle valeur à placer dans la pile.
 */
void varray_set_top_at(varray_t *varray, unsigned int n, value_t *value) {
  *(varray_top_at(varray,n)) = *value;
}

/** Modifier le sommet de la pile.
 * \param[in,out] varray le tableau de valeurs considéré comme une pile.
 * \param[in] value la nouvelle valeur à placer en sommet de pile.
 */
void varray_set_top(varray_t *varray, value_t *value) {
  *(varray_top_at(varray,0)) = *value;
}

/** Empiler une valeur en sommet de pile.
 * \param[in,out] varray le tableau de valeurs considéré comme une pile.
 * \param[in] value la nouvelle valeur à empiler.
 */
void varray_push(varray_t *varray, value_t *value) {
  varray_expandn(varray,1);
  varray_set_top(varray,value);
}

/** Dépiler la valeur en sommet de pile.
 * \param[in,out] varray le tableau de valeurs considéré comme une pile.
 * \return la valeur dépilée.
 */
value_t *varray_pop(varray_t *varray) {
  value_t *value = varray_top(varray);
  varray_popn(varray,1);
  return value;
}

/** Retourner la taille du tableau.
 */
int varray_size(varray_t *varray) {
  return varray->top;
}

/** Tester si le tableau est vide.
 * \return 1 si le tableau est vide, 0 sinon
 */
int varray_empty(varray_t *varray) {
  return varray->top == 0;
}

/** Destruction d'un tableau de valeurs.
 * Remarque : les valeurs contenues ne sont pas désallouées.
 * \param[in,out] varray le tableau à désallouer.
 */
void varray_destroy(varray_t *varray) {
  free(varray->content);
  free(varray);
}

/** Affichage d'un tableau de valeurs sur la sortie standard (pour débogage). 
 * \param[in] varray le tableau à afficher.
 */
void varray_print(varray_t *varray) {
  int i;
  printf("[");
  for(i=0; i<varray_size(varray); i++) {
    if((i > 0) && i<varray_size(varray)) {
      printf(" ");
    }
    value_print(varray_at(varray, i));
  }
  printf("]");
}

/** Affichage d'un tableau de valeurs sur la sortie standard (pour débogage).
 * Cette variante interprète le tableau sous forme de pile.
 * \param[in] varray le tableau à afficher.
 */
void varray_stack_print(varray_t *varray) {
  int i;
  printf("[");
  if(!varray_empty(varray)) {
    value_print(varray_top(varray));
    printf("|");
  }
  for(i=1; i<varray_size(varray); i++) {
    if((i > 1) && i<varray_size(varray)) {
      printf(" ");
    }
    value_print(varray_top_at(varray, i));
  }
  printf("]");
}
