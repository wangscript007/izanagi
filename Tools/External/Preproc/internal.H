/*
 *                          I N T E R N A L . H
 *      I n t e r n a l   D e f i n i t i o n s   f o r   C P P
 *
 * In general, definitions in this file should not be changed by implementor.
 *
 * 1984/05 - 1985/06    DECUS CPP / cpp.h
 *              Created and released on USENET          by Martin Minow
 * 1998/08              CPP Version 2.0 / internal.H
 *              Re-written and renamed from cpp.h       by Psycho
 * 1998/09              CPP Version 2.1 / internal.H
 *              Updated according C9X-1998/08 draft     by Psycho
 * 1998/11              CPP Version 2.2 / internal.H
 *              Slightly revised                        by Psycho
 */

#ifndef SYSTEM_H
    << Error, "system.H" must be included prior to "internal.H". >>
#endif

#ifndef EOS             /* This is predefined in Decus C    */
#define EOS             '\0'        /* End of string                */
#endif
#define CHAR_EOF        0           /* Returned by get() on eof     */
#define NULLST          ((char *) NULL)     /* Pointer to nowhere   */

#define VA_ARGS         (UCHARMAX + 1)      /* Signal of variable arguments */
#define DEF_PRAGMA      (-1 - VA_ARGS)      /* _Pragma() pseudo-macro       */
#define DEF_NOARGS      (-2 - VA_ARGS)      /* #define foo vs #define foo() */

/*
 * The following may need to change if the host system doesn't use ASCII.
 * These magic characters must be control characters which can't be used
 * in source file.
 */
#if     MODE != CON_FALSE
#define DEF_MAGIC       0x1C        /* Magic for #defines           */
#define ST_QUOTE        0x1D        /* Magic for stringizing        */
#define CAT             0x1E        /* Token concatenation delim.   */
#endif
#if     MODE == STANDARD
#define TOK_SEP         0x1F        /* Magic to wrap expanded macro */
#else
#if     COMMENT_INVISIBLE
#define COM_SEP         0x1F        /* Comment as token separator   */
#endif
#endif
#if     MODE >= STANDARD
#define RT_END          0x1B    /* Magic for macro rescan boundary  */
#endif
#define MAC_PARM        0x7F        /* Macro formals signal         */

/* Special character types  */
#define LET             1           /* Letter (alphabet and _)      */
#define DIG             2           /* Digit                        */
#define DOT             4           /* . might start a number       */
#define PUNC            8           /* Punctuators and operators    */
#define QUO             16          /* Both flavors of quotation ",'*/
#define SPA             32          /* White spaces                 */
#if     MBCHAR
#define MB1             64          /* The first byte of MBCHAR     */
#define MB2             ((char)128) /* The second byte of MBCHAR    */
#endif

/*
 * Codes for operators used in #if expression.
 * The value is stored in 'openum'.
 */
#define INV             0           /* Invalid, must be zero        */
#define OP_EOE          INV         /* End of expression            */
#define VAL             1           /* Value (operand)              */
#define OP_LPA          2           /* (    */
/* The following are unary.     */
#define FIRST_UNOP      OP_PLU      /* First unary operator         */
#define OP_PLU          3           /* + (ANSI Standard)            */
#define OP_NEG          4           /* -    */
#define OP_COM          5           /* ~    */
#define OP_NOT          6           /* !    */
#define LAST_UNOP       OP_NOT      /* Last unary operator          */
/* The following are binary.    */
#define FIRST_BINOP     OP_MUL      /* First binary operator        */
#define OP_MUL          7           /* *    */
#define OP_DIV          8           /* /    */
#define OP_MOD          9           /* %    */
#define OP_ADD          10          /* +    */
#define OP_SUB          11          /* -    */
#define OP_SL           12          /* <<   */
#define OP_SR           13          /* >>   */
#define OP_LT           14          /* <    */
#define OP_LE           15          /* <=   */
#define OP_GT           16          /* >    */
#define OP_GE           17          /* >=   */
#define OP_EQ           18          /* ==   */
#define OP_NE           19          /* !=   */
#define OP_AND          20          /* &    */
#define OP_XOR          21          /* ^    */
#define OP_OR           22          /* |    */
#define OP_ANA          23          /* &&   */
#define OP_ORO          24          /* ||   */
#define OP_QUE          25          /* ?    */
#define OP_COL          26          /* :    */
#define LAST_BINOP      OP_COL      /* Last binary operator         */
/* Parenthesis  */
#define OP_RPA          27          /* )    */
#define OP_END          28          /* End of expression marker     */
#define OP_FAIL         (OP_END + 1)    /* For error returns        */

/*
 * The following are operators used in macro definition only.
 */
#if     MODE != CON_FALSE
#define OP_STR          30          /* #    */
#define OP_CAT          31          /* ##   */
#define OP_ELL          32          /* ...  */
#endif

/*
 * The following are C source operators or punctuators,
 * not preprocessing operators.
 * Note: "sizeof", "defined" are read as identifier for convenience.
 */
#define OP_1            33  /* Any other single byte ops or puncs   */
                                    /* =, ., ;, [, ], {, }, ','     */
#define OP_2            34  /* Any other two bytes operators        */
                    /* &=, |=, ++, +=, --, -=, ->, %=, *=, /=, ^=,  */
#define OP_3            35  /* Three bytes operators :  <<=, >>=    */
/*
 * The following are operators spelled in digraphs.
 */
#if     MODE >= STANDARD
#define OP_LBRACE_D     0x40        /* <% i.e. {    */
#define OP_RBRACE_D     0x41        /* %> i.e. }    */
#define OP_LBRCK_D      0x42        /* <: i.e. [    */
#define OP_RBRCK_D      0x43        /* :> i.e. ]    */
#define OP_SHARP_D      0x44        /* %: i.e. #    */
#define OP_DSHARP_D     0x45        /* %:%: i.e. ## */
#define OP_DIGRAPH      0x40    /* (OP_*_D & OP_DIGRAPH) == 0x40    */
#endif

/*
 * The following are for lexical scanning only.
 */
/* Token types          */
#define NO_TOKEN        0
#define NAM             65          /* Identifier (name, keyword)   */
#define NUM             66          /* Preprocessing number         */
#define STR             67          /* Character string literal     */
#define CHR             69          /* Integer character constant   */
#if     MODE >= STANDARD
#define WSTR            68          /* Wide string literal          */
#define WCHR            70          /* Wide character constant      */
#endif
#define OPE             71          /* Operator or punctuator       */
#define SPE             72          /* Unknown token (@ or others)  */
#define SEP             73          /* Token separator or magics    */

/*
 * The following are values of 'debug' variable which is set by the
 * arguments of #pragma __debug directive.
 */
#define PATH            1
#define TOKEN           2
#define EXPAND          4
#define IF              8
#define EXPRESSION      16
#define GETC            32
#define MEMORY          64

/*
 * The DEFBUF structure stores information about #defined macros.
 * Note that DEFBUF->parmnames is parameter names catenated with commas,
 * which is saved for the check of redefinition for Standard mode.
 * 'parmnames' and 'repl' are allocated to the area succeding to name.
 */
typedef struct defbuf {
        struct defbuf * link;       /* Pointer to next def in chain */
        short           nargs;      /* Number of parameters         */
#if     MODE == STANDARD
        char *          parmnames;  /* -> Parameter names catenated by ','  */
#endif
        char *          repl;       /* Pointer to replacement text  */
        char            name[1];    /* Macro name                   */
} DEFBUF;

/*
 * The FILEINFO structure stores information about open files and macros
 * being expanded.
 */
typedef struct fileinfo {
        char *          bptr;       /* Current pointer into buffer  */
        long            line;       /* Current line number of file  */
        FILE *          fp;         /* Source file if non-null      */
        long            pos;        /* Position next to #include    */
        struct fileinfo *   parent; /* Link to includer             */
        struct ifinfo *     initif; /* Initial ifstack (return there on EOF)*/
        char *          filename;   /* File/macro name              */
        char *          buffer;     /* Buffer of current input line */
} FILEINFO;

/*
 * IFINFO stores information of conditional compilation.
 */
typedef struct ifinfo {
        int             stat;       /* State of compilation         */
        long            ifline;     /* Line #if started             */
        long            elseline;   /* Line #else started           */
} IFINFO;

/*
 * These bits are set in IFINFO.stat
 */
#define WAS_COMPILING   1           /* TRUE if compile set at entry */
#define ELSE_SEEN       2           /* TRUE when #else processed    */
#define TRUE_SEEN       4           /* TRUE when #if TRUE processed */

#define compiling       ifstack[0].stat

/* VAL_SIGN structure stores information about evaluated number.    */
typedef struct val_sign {
        expr_t          val;        /* Value                        */
        int             sign;       /* Signed, unsigned, error      */
} VAL_SIGN;

/* Values of VAL_SIGN.sign. */
#define SIGNED          1
#define UNSIGNED        0
#define VAL_ERROR       (-1)

/* Value of macro_line on macro call error.         */
#define MACRO_ERROR     (-1L)

/* Values of mkdep. */
#define MD_MKDEP        1   /* Output source file dependency line   */
#define MD_SYSHEADER    2   /* Print also system-header names       */
#define MD_FILE         4   /* Output to the file named *.d         */

#if     MODE == POST_STANDARD
/* Values of insert_sep (flag of insertion of token separator).     */
#define NO_SEP          0   /* No separator is inserted             */
#define INSERT_SEP      1   /* Next get() shall insert a separator  */
#define INSERTED_SEP    2   /* Last get() has Inserted a separator  */
#endif

#if     OK_SIZE
/*
 * Define bits for the basic types and their adjectives.
 */
#define T_CHAR          1
#define T_INT           2
#define T_FLOAT         4
#define T_DOUBLE        8
#define T_LONGDOUBLE    16
#define T_SHORT         32
#define T_LONG          64
#define T_LONGLONG      128
#define T_SIGNED        256
#define T_UNSIGNED      512
#define T_PTR           1024        /* Pointer to data objects      */
#define T_FPTR          2048        /* Pointer to functions         */
/*
 * The SIZES structure is used to store the values for #if sizeof.
 */
typedef struct sizes {
        int             bits;       /* If this bit is set,          */
        int             size;       /* this is the datum size value */
        int             psize;      /* this is the pointer size     */
} SIZES;
#endif

#define str_eq(s1, s2)  (strcmp(s1, s2) == 0)

/*
 * Error codes.  VMS uses system definitions.
 * Decus C codes are defined in stdio.h.
 * Others are cooked to order.
 */
#if     HOST_SYSTEM == SYS_VMS
#include                <ssdef.h>
#include                <stsdef.h>
#define IO_SUCCESS      (SS$_NORMAL | STS$M_INHIB_MSG)
#define IO_ERROR        SS$_ABORT
#endif
/*
 * Note: IO_SUCCESS and IO_ERROR are defined in the Decus C stdio.h file.
 */
#ifndef IO_SUCCESS
#define IO_SUCCESS      0
#endif
#ifndef IO_ERROR
#define IO_ERROR        (errno ? errno : 1)
#endif

/*
 * Externs
 */
extern D09  int     cflag;          /* -C option (keep comments)    */
extern      int     eflag;          /* -E option (ignore errors)    */
extern D09  int     iflag;      /* -i option (no-output of included file)   */
extern D09  int     lflag;          /* -L option (output source)    */
extern D09  int     pflag;          /* -P option (no #line output)  */
extern      int     qflag;          /* -Q option (error to cpp.err) */
#if     MODE != CON_FALSE && OK_TRIGRAPHS
extern D09  int     tflag;          /* -3 option (toggle trigraphs) */
#endif
#if     MODE >= STANDARD
#if     OK_DIGRAPHS
extern D09  int     digraphs;       /* -2 option (toggle digraphs)  */
#endif
extern D09  int     stdc_val;       /* Value of __STDC__            */
extern      long    stdc_ver;       /* Value of __STDC_VERSION__    */
extern      long    cplus;          /* Value of __cplusplus for C++ */
extern      int     stdc2;          /* cplus || stdc_ver >= 199901L */
extern      int     slenmin;        /* Least maxmum of string len.  */
extern      int     idlenmin;       /* Least maximum of ident len.  */
extern      int     nmacparsmin;    /* Least maximum of num of pars.*/
extern      int     exp_nestmin;    /* Least maximum of expr nest   */
extern      int     blk_nestmin;    /* Least maximum of block nest  */
extern      int     inc_nestmin;    /* Least maximum of include nest*/
extern      int     nmacromin;      /* Least maximum of num of macro*/
extern      long    line_limit;     /* Maximum source line number   */
extern D09  int     has_pragma;     /* '_Pragma()' has been expanded*/
#endif
extern D09  int     std_line_prefix;    /* #line in C source style  */
extern D09  int     warn_level;     /* Level of warning             */
extern      int     errors;         /* Error counter                */
extern D09  long    line;           /* Current source line number   */
extern D09  int     wrongline;      /* Force #line to compiler      */
extern D09  int     keepcomments;   /* Don't remove comments        */
extern      int     include_nest;   /* Nesting level of #include    */
extern D09  int     no_output;      /* Don't output included file   */
extern D09  int     in_directive;   /* In process of #directive     */
extern D09  int     in_define;      /* In #define line              */
extern D09  long    macro_line;     /* Line number of macro call*/
extern D09  int     openum;         /* Number of operator or punct. */
extern D09  IFINFO *    ifptr;      /* -> current ifstack item      */
extern D09  FILEINFO *  infile;     /* Current input file or macro  */
#if     MODE == POST_STANDARD
extern D09  int     insert_sep;     /* Inserted token separator flag*/
#endif
#if     OK_MAKE
extern      int     mkdep;          /* Output source file dependency*/
#endif
extern D09  char *  workp;          /* Free space in work[]         */
extern D09  char * const    work_end;       /* End of work[] buffer */
extern D09  char    identifier[ IDMAX + IDMAX/8];   /* Lastly scanned name  */
extern const char   type[];         /* Character classifier         */
extern IFINFO       ifstack[];      /* Information of #if nesting   */
extern char         work[];
        /* Temporary buffer for directive line and macro expansion  */

#if     OK_SIZE
extern SIZES        size_table[];   /* For #if sizeof sizes         */
#endif
#if     DEBUG || DEBUG_EVAL
extern D09  int     debug;          /* Debug class                  */
#endif
#if     MODE < STANDARD
extern D09  long    in_asm;         /* In #asm - #endasm block      */
#endif

#if     PROTO   /* prototype declarations   */

/* main.c   */
extern void     curfile( void);
extern int      openfile( const char * filename, int local);
extern void     unpredefine( int clearall);
extern void     undef_a_predef( const char * name);
/* control.c    */
extern int      control( int newlines);
extern DEFBUF * lookid( const char * name);
extern DEFBUF * install( const char * name, int nargs, const char * parmnames
        , const char * repl);
extern int      undefine( const char * name);
extern void     dumpadef( const char * why, const DEFBUF * dp, int newdef);
extern void     dumpdef( void);
/* eval.c   */
extern expr_t   eval( void);
extern VAL_SIGN *   evalnum( const char * nump);
/* expand.c */
extern DEFBUF * is_macro( char ** cp);
extern char *   expand( DEFBUF * defp, char * out, char * out_end);
/* support.c    */
extern int      get_unexpandable( int c, int diag);
extern void     skipnl( void);
extern int      skipws( void);
extern int      scantoken( int c, char ** out_pp, char * out_end);
extern char *   scanquote( int delim, char * out, char * out_end, int diag);
extern int      get( void);
#if     MODE != CON_FALSE && OK_TRIGRAPHS
extern int      trigraph( char * in);
#endif
#if     MODE == POST_STANDARD && OK_DIGRAPHS
extern int      cnv_digraph( char * in);
#endif
extern void     unget( void);
extern FILEINFO *   ungetstring( const char * text, const char * name);
extern char *   savestring( const char * text);
extern FILEINFO *   getfile( const char * name, size_t bufsize);
extern char *   (xmalloc)( size_t size);
extern char *   (xrealloc)( char * ptr, size_t size);
#if     COMMENT_INVISIBLE
extern void     put_line( char * out, FILE * fp);
#endif
extern void     cfatal( const char * format, const char * arg1, long arg2
        , const char * arg3);
extern void     cerror( const char * format, const char * arg1, long arg2
        , const char * arg3);
extern void     cwarn( const char * format, const char * arg1, long arg2
        , const char * arg3);
/* system.c */
extern void     dooptions( int argc, char ** argv, char ** in_pp
        , char ** out_pp);
extern int      reopen_stdout( const char * filename);
extern void     setstdin( char * cp);
extern void     put_start_file( char * filename);
extern void     sharp( void);
extern void     putfname( char * filename);
extern int      getredirection( int argc, char ** argv);
#if     OK_MAKE
extern void     put_depend( const char * filename);
#endif
extern int      doinclude( void);
#if     MODE >= STANDARD
extern void     dopragma( void);
#else
extern void     doold( void);
extern void     put_asm( void);
#endif
extern void     put_source( const char * src);

#if     DEBUG
/* support.c    */
extern void     dumpstring( const char * why, const char * text);
extern void     dumpunget( const char * why);
/* system.c     */
extern void     print_heap( void);
#endif

#else           /* old style function declarations  */

/* main.c   */
extern void     curfile();          /* Print source file name       */
extern int      openfile();         /* Open a source file           */
extern void     unpredefine();      /* Undefine predefined macros   */
extern void     undef_a_predef();   /* Remove a non-standard predef */
/* control.c    */
extern int      control();          /* Process #directive lines     */
extern DEFBUF * lookid();           /* Look for a #define'd thing   */
extern DEFBUF * install();          /* Install to symbol table      */
extern int      undefine();         /* Delete from symbol table     */
extern void     dumpadef();         /* Dump a specific macro def    */
extern void     dumpdef();          /* Dump current macro defs      */
/* eval.c   */
extern expr_t   eval();             /* Evaluate #if expression      */
extern VAL_SIGN *   evalnum();      /* Evaluate preprocessing number*/
/* expand.c */
extern DEFBUF * is_macro();         /* The sequence is a macro call?*/
extern char *   expand();           /* Expand a macro completely    */
/* support.c    */
extern int      get_unexpandable(); /* Get next unexpandable token  */
extern void     skipnl();           /* Skip to the end of the line  */
extern int      skipws();           /* Skip over white-spaces       */
extern int      scantoken();        /* Get the next token           */
extern char *   scanquote();        /* Scan a quoted literal        */
extern int      get();              /* Get the next char from input */
#if     MODE != CON_FALSE && OK_TRIGRAPHS
extern int      trigraph();         /* Do trigraph replacement      */
#endif
#if     MODE == POST_STANDARD && OK_DIGRAPHS
extern int      cnv_digraph();      /* Convert digraphs to usual tok*/
#endif
extern void     unget();            /* Push back the char to input  */
extern FILEINFO *   ungetstring();  /* Push back the string to input*/
extern char *   savestring();       /* Stuff string in malloc mem.  */
extern FILEINFO *   getfile();      /* New FILEINFO initialization  */
extern char *   (xmalloc)();        /* Get memory or die            */
extern char *   (xrealloc)();       /* Reallocate memory or die     */
#if     COMMENT_INVISIBLE
extern void     put_line();         /* Put out a logical line       */
#endif
extern void     cfatal();           /* Print a fatal error and exit */
extern void     cerror();           /* Print an error message       */
extern void     cwarn();            /* Print a warning message      */
/* system.c */
extern void     dooptions();        /* Process command line args    */
extern int      reopen_stdout();    /* Connect fp to stdout         */
extern void     setstdin();         /* Get name of stdin            */
extern void     put_start_file();   /* Output the source file name  */
extern void     sharp();            /* Output # line number         */
extern void     putfname();         /* Output the include file name */
extern int      getredirection();   /* Redirect standard paths      */
#if     OK_MAKE
extern void     put_depend();       /* Output source dependency line*/
#endif
extern int      doinclude();        /* Process #include directive   */
#if     MODE >= STANDARD
extern void     dopragma();         /* Process #pragma directive    */
#else
extern void     doold();            /* Process older directives     */
extern void     put_asm();          /* Putout an asm code line      */
#endif
extern void     put_source();       /* Output source line           */

#if     DEBUG
/* support.c    */
extern void     dumpstring();       /* Dump text readably           */
extern void     dumpunget();        /* Dump all ungotten junk       */
/* system.c     */
extern void     print_heap();       /* Print blocks of heap memory  */
#endif

#endif          /* PROTO    */

