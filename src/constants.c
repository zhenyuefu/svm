/* fichier g�n�r� automatiquement : *ne pas �diter* */

/** Noms des opcodes */
const char *opcode_names[] = {"galloc", "push",   "gstore", "pop",
                              "jump",   "gfetch", "call",   "return",
                              "fetch",  "jfalse", "store",  "<unknown>"};

/** Noms des types */
const char *type_names[] = {"unit", "int",  "prim",     "fun",
                            "bool", "pair", "<unknown>"};

/** Noms des primitives */
const char *primitive_names[] = {"+",   "-",     "*",        "/",
                                 "=",   "list",  "cons",     "car",
                                 "cdr", "zero?", "<unknown>"};
