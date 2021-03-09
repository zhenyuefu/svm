/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "bytecode.h"
#include "constants.h"


/** Lecture d'un entier dans le fichier de bytecode.
 * \param[in,out] f  le fichier de bytecode
 * \return l'entier parsé ou EOF à la fin du fichier
 */
int read_int(FILE *f) {
  // lire une ligne
  char buf[81];
  int pos = 0;

  while(1) {
    int ch = fgetc(f);
    if(ch == EOF) {
      return EOF;
    }
    if(ch==(int) ' ') { // sauter l'espace terminal
      break; // puis on sort de la boucle
    } else if ((ch >= (int) '0') && (ch <= (int) '9')) {
      // récupérer un chiffre
      buf[pos] = (char) ch;
      pos = pos + 1;
    } else {
      // un autre caractère (interdit)
      fprintf(stderr, "incorrect character '%c' in bytecode\n",ch);
      exit(EXIT_FAILURE);
    }
  }

  buf[pos] = (char) 0;   // terminaison

  if(pos==0) {
    // pas d'entier à parser
    return EOF;
  }
  
  int val = -1 ;
  int res = sscanf(buf,"%d",&val);
  if(res!=1) {
    // le scan devrait retourner un entier
    fprintf(stderr, "parse error in bytecode for input: %s\n",buf);
    exit(EXIT_FAILURE);
  }
  
  return val;
}


/** Lecture d'un fichier de byte-code.
 * \param[in,out] program le segment de code à charger
 * \param[in] filename le nom du fichier contenant le byte-code.
 * \return le tableau de bytecode
 */
void bytecode_read(program_t * program, const char *filename) {
  // ouverture du fichier
  FILE *f = fopen(filename, "r");  // open the file
  if(f==NULL) {
    fprintf(stderr,"cannot open bytecode file: %s\n",filename);
    exit(EXIT_FAILURE);
  }

  int magic = read_int(f);
  if(magic!=424242) {
    fprintf(stderr,"incorrect bytecode file (missing magic): %s\n",filename);
    exit(EXIT_FAILURE);
  }

  // taille en nombre d'entiers à lire ensuite
  int size = read_int(f);
  if(size<=0) {
    fprintf(stderr,"incorrect bytecode file (incorrect size %d): %s\n",size,filename);
    exit(EXIT_FAILURE);
  }

  program->bytecode = (int *) malloc(sizeof(int)*size);
  assert(program->bytecode!=NULL);
  program->size = size;

  int count;
  for(count=0;count<size;count++) {
    int next = read_int(f);
    if(next==EOF) {
      fprintf(stderr,"unexpected EOF in bytecode file: %s\n",filename);
      exit(EXIT_FAILURE);
    }
    program->bytecode[count]=next;
  }
  
}
  

/** Désallocation du segment de code. 
 * \param[in,out] program le segment de code à désallouer. */
void bytecode_destroy(program_t *program) {
  free(program->bytecode);
}

/** Affichage d'une instruction de bytecode au format assembleur.
 * \param[in] program le programme à afficher.
 * \param[in] pc le compteur de programme pour l'instruction à afficher.
 * \return le pc de la prochaine instruction.
 */
int bytecode_print_instr(program_t *program, unsigned int pc) {
  int opcode = program->bytecode[pc];
  switch(opcode) {
  case I_GALLOC : {
    printf("GALLOC\n");
    return pc+1;
  }
  case I_GSTORE: {
    printf("GSTORE %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  case I_GFETCH: {
    printf("GFETCH %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  case I_STORE: {
    printf("STORE %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  case I_FETCH: {
    printf("FETCH %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  case I_PUSH: {
    printf("PUSH ");
    switch(program->bytecode[pc+1]) {
    case T_BOOL:
      printf("BOOL ");
      if(program->bytecode[pc+2] == 0) {
        printf("FALSE\n");
      } else if(program->bytecode[pc+2] == 1) {
        printf("TRUE\n");
      } else {
        fprintf(stderr,"Error: wrong BOOL value '%d' for PUSH\n",program->bytecode[pc+2]);
        exit(EXIT_FAILURE);
      } 
      return pc+3;
    case T_INT:
      printf("INT %d\n", program->bytecode[pc+2]);
      return pc+3;
    case T_UNIT:
      printf("UNIT\n");
      return pc+2;
    case T_PRIM:
      printf("PRIM %d\n", program->bytecode[pc+2]);
      return pc+3;
    case T_FUN:
      printf("FUN %d\n", program->bytecode[pc+2]);
      return pc+3;
    default:
      fprintf(stderr,"Error: unknown type '%d' for PUSH\n",program->bytecode[pc+1]);
      exit(EXIT_FAILURE);
    }
    break;
  }
  case I_POP: {
    printf("POP\n");
    return pc+1;
  }
  case I_CALL: {
    printf("CALL %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  case I_RETURN: {
    printf("RETURN\n");
    return pc+1;
  }
  case I_JUMP: { 
    printf("JUMP %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  case I_JFALSE: { 
    printf("JFALSE %d\n", program->bytecode[pc+1]);
    return pc+2;
  }
  default:
    fprintf(stderr,"Error: unknown opcode '%d'\n",opcode);
    exit(EXIT_FAILURE);
  }
}

/** Affichage d'un segment de code (pour déboguage) 
 * \param[in] program le segment de code
 */
void bytecode_print(program_t *program) {
  int pc = 0;
  while(pc<program->size) {
    printf("%d: ",pc);
    pc = bytecode_print_instr(program,pc);
  }
}
