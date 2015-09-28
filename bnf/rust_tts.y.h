#define _C(v)	v { $$ = v; }
#define _T(v)	v { $$ = v; }
tt_tok
 : _T(IDENT) | _T(STRING) | _T(LIFETIME) | _T(MACRO)
 | _T(DOC_COMMENT) | _T(SUPER_DOC_COMMENT)
 | _T(CHARLIT) | _T(INTEGER)
 | _T(FLOAT)
 | _C(',') | _C(';') | _C('_')
 | _T(RWD_self) | _T(RWD_super) | _T(RWD_mut) | _T(RWD_ref) | _T(RWD_let) | _T(RWD_where) | _T(RWD_pub) | _T(RWD_in) | _T(RWD_as)
 | _T(RWD_for ) | _T(RWD_while) | _T(RWD_loop) | _T(RWD_if) | _T(RWD_else) | _T(RWD_match) | _T(RWD_box) | _T(RWD_move)
 | _T(RWD_return) | _T(RWD_continue) | _T(RWD_break)
 | _T(RWD_impl) | _T(RWD_struct) | _T(RWD_enum) | _T(RWD_fn) | _T(RWD_type) | _T(RWD_static) | _T(RWD_const) | _T(RWD_trait) | _T(RWD_use)
 | _T(RWD_extern) | _T(RWD_crate) | _T(RWD_unsafe)
 | _C('/') | _T(SLASHEQUAL)
 | _C('%') | _T(PERCENTEQUAL)
 | _C('*') | _T(STAREQUAL)
 | _C('+') | _T(PLUSEQUAL)
 | _C('-') | _T(MINUSEQUAL) | _T(THINARROW)
 | _C('!') | _T(EXCLAMEQUAL)
 | _C('&') | _T(DOUBLEAMP)  | _T(AMPEQUAL)
 | _C('|') | _T(DOUBLEPIPE) | _T(PIPEEQUAL)
 | _C(':') | _T(DOUBLECOLON)
 | _C('^') | _T(CARETEQUAL)
 | _C('=') | _T(DOUBLEEQUAL) | _T(FATARROW)
 | _C('<') | _T(DOUBLELT)    | _T(LTEQUAL)   | _T(DOUBLELTEQUAL)
 | _C('>') | _T(DOUBLEGT)    | _T(GTEQUAL)   | _T(DOUBLEGTEQUAL)
 | _C('.') | _T(DOUBLEDOT)   | _T(TRIPLEDOT)
 | _C('$') | _C('@') | _C('?')
 | _C('#') | _T(HASHBANG)
 ;
#undef _
