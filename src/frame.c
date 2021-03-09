/******
 * UPMC -- licence informatique
 * 2009/2010
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Bloc de pile (frame)
 *   implantation
 ******/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "frame.h"


/** Allocation d'un cadre d'appel de fonction.
 */
frame_t *frame_push(frame_t *frame,
                    env_t *env,
                    unsigned int sp,
                    unsigned int pc) {
  frame_t *res = (frame_t *) malloc(sizeof(frame_t));  
  assert(res!=NULL);

  res->sp = sp;
  res->env = env;
  res->pc = pc;                                               
  res->caller_frame = frame;  
  
  return res;
}

/** Destruction d'un cadre d'appel de fonction.
 */
frame_t *frame_pop(frame_t *frame) {
  frame_t *caller_frame = frame->caller_frame;
  
  free(frame); // puis détruire le cadre de pile
  
  return caller_frame;
}

/** Affichage d'un cadre d'appel de fonction (pour déboguage). */
void frame_print(frame_t *frame) {
  if(frame) {
    printf("Frame(pc=%d,sp=%d,env=",frame->pc,frame->sp);
    env_print(frame->env);
    printf(")\n<- ");
    frame_print(frame->caller_frame);
  } else { // last frame
    printf("END");
  }
}
 
