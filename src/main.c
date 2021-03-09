/* UPMC -- licence informatique
 * (C) 2009-2011 Equipe enseignante
 * LI223: Initiation à la Compilation et aux Machines Virtuelles
 *
 * Redistribution possible sous licence GPL v2.0 ou ultérieure
 */

/** \mainpage
 * Ceci est la documentation hypertexte du code source de la machine
 * virtuelle (VM) native utilisée dans le cadre du cours
 * LI223 : Initiation à la compilation et aux machines virtuelles
 *
 * La machine virtuelle est implémentée dans le langage C.
 * 
 * Le mode de navigation privilégié consiste à parcourir les fichiers
 * (en-têtes .h et modules .c) composant le projet.
 *
 * Une autre possibilité est de parcourir les structures de données
 * principales de la VM.
 *
 * Bonne navigation !
 */

/** \file main.c
 * Point d'entrée de la machine virtuelle native.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include "vm.h"

/** Petit mode d'emploi */
static void vm_help() {
  printf("Usage: svm [--help] [-d] [--vmdebug] [--gcdebug] [--gcfreq=FF] prog.bc\n");
  printf("   ==> run SVM with compiled program\n");
  printf("Options:\n");
  printf("   -h, --help    : print this help and exit\n");
  printf("   -d, --vmdebug : start the VM in debug mode\n");
  printf("   --gcdebug     : start the VM with Garbage Collector in debug mode\n");
  printf("   --gcfreq=FF   : GC frequency set to FF (positive integer)\n");
  printf("\n");
}

// quelques références en avant
int parse_debug_vm(int index, char *argv[]);
int parse_debug_gc(int index, char *argv[]);
int parse_gc_freq(int index, char *argv[]);

/** Point d'entrée de la machine virtuelle native.
 * \param[in] argc le nombre d'arguments sur la ligne de commande
 * \param[in] argv un tableau des arguments sur la ligne de commande
 */
int main(int argc, char *argv[]) {
  int debug_vm = 0;
  int debug_gc = 0;
  int gc_freq = 0;
  char freq[10];
  char *filename = NULL;
  int i;

  printf("SVM v4 (native)\n");
  printf("---------------\n");

  if(argc<=1) {
    fprintf(stderr,"Error: missing arguments\n");
    vm_help();
    exit(EXIT_FAILURE);
  }

  if(strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0) {
    vm_help();
    exit(EXIT_SUCCESS);
  }

  /* On commence par analyser la ligne de commande */
  for(i=1;i<argc;i++) {
    if(parse_debug_vm(i,argv)) {
      if(debug_vm) {
        fprintf(stderr,"option -v or --vmdebug set twice\n");
        exit(EXIT_FAILURE);
      } else {
        debug_vm = 1;
      }
    } else if(parse_debug_gc(i,argv)) {
      if(debug_gc) {
        fprintf(stderr,"option --gcdebug set twice\n");
        exit(EXIT_FAILURE);
      } else {
        debug_gc = 1;
      }
    } else {
      int freq = parse_gc_freq(i,argv);
      if(freq==0) {
        filename = argv[i];
        if(argc>i+1) {
          fprintf(stderr,"too many arguments\n");
          exit(EXIT_FAILURE);
        }
        break; // sort de la boucle
      } else {
        gc_freq = freq;
      }
    }
  }

  /* petit résumé des options */


  if(debug_vm) {
    printf("starting VM in debug mode\n");
  }
  if(debug_gc) {
    printf("debugging Garbage Collector\n");
  }
  if(gc_freq>0) {
    printf("GC frequency = %d\n",gc_freq);
  } else {
    gc_freq = DEFAULT_GC_FREQUENCY;
  }
  
  /* et maintenant on charge le bytecode */
  
  printf("loading bytecode file: %s\n",filename);

  program_t program;
  
  // on lit tout d'abord le fichier de bytecode
  bytecode_read(&program, filename);
  if(debug_vm) {
    printf("=== Loaded program:\n");
    bytecode_print(&program);
    printf("===================\n");
  }
 
  // Initialisation de la VM avec une fréquence de collection
  // initialisée à 3 pour pouvoir regarder le GC en action facilement.
  if(debug_vm) {
    printf("Initializing VM with GC frequency=%d\n",gc_freq);
  }
  vm_t * vm = init_vm(&program, debug_vm, debug_gc, gc_freq);                                          

  // puis on l'exécute
  printf("-------------------\n");
  if(debug_vm) {
    printf("=== Begin execution ====\n");
  }
  vm_execute(vm);                                  

  // et finalement on récupère la mémoire du bytecode
  bytecode_destroy(&program);

  if(debug_vm) {
    printf("=== Finish execution ====\n");
  }
  
  printf("-------------------\n");
  printf("VM stopping\n");

  // et c'est fini !
  return EXIT_SUCCESS;
}



/** Analyse de la ligne de commande (option --debugvm) */
int parse_debug_vm(int index, char *argv[]) {
  if(strcmp(argv[index],"-d")==0 || strcmp(argv[index],"--vmdebug")==0) {
    return 1;
  } else {
    return 0;
  }
}

/** Analyse de la ligne de commande (option --debuggc) */
int parse_debug_gc(int index, char *argv[]) {
  if(strcmp(argv[index],"--gcdebug")==0) {
    return 1;
  } else {
    return 0;
  }
}

/** Analyse de la ligne de commande (option --gc-freq) */
int parse_gc_freq(int index, char *argv[]) {
  char buf[11];
  if(strncmp(argv[index],"--gcfreq=",9)!=0) {
    return 0;
  }

  strncpy(buf, &(argv[index][9]),10);

  long val = strtol(buf,NULL,10);
  if(val==0 && errno==EINVAL) {
    fprintf(stderr,"Incorrect gc frequency: %s\n",buf);
    exit(EXIT_FAILURE);
  }

  if(val<=0) {
    fprintf(stderr,"GC frequency should be positive, given: %d\n",(int) val);
    exit(EXIT_FAILURE);
  }
  
  return (int) val;
} 
