/* fichier g�n�r� automatiquement : *ne pas �diter* */

/* Constantes pour les opcodes */

/** opcode LABEL */
#define I_LABEL -1

/** opcode GALLOC */
#define I_GALLOC 0

/** opcode PUSH */
#define I_PUSH 1

/** opcode GSTORE */
#define I_GSTORE 2

/** opcode POP */
#define I_POP 3

/** opcode JUMP */
#define I_JUMP 4

/** opcode GFETCH */
#define I_GFETCH 5

/** opcode CALL */
#define I_CALL 6

/** opcode RETURN */
#define I_RETURN 7

/** opcode FETCH */
#define I_FETCH 8

/** opcode JFALSE */
#define I_JFALSE 9

/** opcode STORE */
#define I_STORE 10

/** Noms des opcodes */
extern const char *opcode_names[];

/* Constantes pour les types */

/** type UNIT */
#define T_UNIT 0

/** type INT */
#define T_INT 1

/** type PRIM */
#define T_PRIM 2

/** type FUN */
#define T_FUN 3

/** type BOOL */
#define T_BOOL 4

/** type PAIR */
#define T_PAIR 5

/** Noms des types */
extern const char *type_names[];

/* Constantes pour les primitives */

/** primitive + */
#define P_ADD 0

/** primitive - */
#define P_SUB 1

/** primitive * */
#define P_MUL 2

/** primitive / */
#define P_DIV 3

/** primitive = */
#define P_EQ 4

/** primitive list */
#define P_LIST 5

/** primitive cons */
#define P_CONS 6

/** primitive car */
#define P_CAR 7

/** primitive cdr */
#define P_CDR 8

/** primitive zero? */
#define P_ZEROP 9

/** Noms des primitive */
extern const char *primitive_names[];
