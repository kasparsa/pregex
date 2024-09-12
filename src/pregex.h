#ifndef PREGEX_H
#define PREGEX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Declare global variables as extern
extern const char* patchar;
extern const char* symbol;
extern const char* uppercase;
extern const char* lowercase;
extern const char* digit;
extern const char* hex;
extern const char* letters;
extern const char* wordsym;
extern const char* delimiter;
extern const char* brackets;
extern const char* blank;
extern const char* newline;

typedef struct tmatch {
    unsigned long start;
    unsigned long end;
    char *str;
} tmatch;

class PRegEx {
private:
    char *buf;
    unsigned long fs;

    tmatch *match;
    unsigned long col_cnt;
    unsigned long row_cnt;

    int hk = 0;
public:
    PRegEx();
    ~PRegEx();

    void verbosity(int level);

    int verbose_level;

    unsigned long group;

    unsigned long match_count;

    unsigned long balance_depth;
    char balanced_symbol_initiator[2];
    char balanced_symbol_to_find[2];
    unsigned long balance_index;
    unsigned long balance_can_proceed;
    unsigned long balance_related;


    typedef struct tpat {
        unsigned long zeroorone;
        unsigned long zeroormore;
        unsigned long oneormore;
        unsigned long exact;
        char *pat;
        unsigned long releasable;
        unsigned long gs;
        unsigned long ge;
        unsigned long mark;
        unsigned long atbeginning;
        unsigned long atend;
        unsigned long matched;
        unsigned long balanced;
        unsigned long balanced_open;
        unsigned long balanced_close;
    } tpat;

    void print_tokenized_groups( tmatch *match, unsigned long col_cnt );
    void print_match_debug( char *buf, unsigned long len, unsigned long *src );

    long substring_found( char *str, char *substr );
    char *replace_substr( char *str, char *substr, unsigned long pos, unsigned long len );
    char *replace_abrv( char *str );

    unsigned long pattern_str( unsigned long pos, char *patstr, tpat *pat, unsigned long index );
    unsigned long patter_esc( unsigned long pos, char *patstr, tpat *pat, unsigned long index );
    unsigned long pattern_allowed( unsigned long pos, char *patstr, tpat *pat, unsigned long index );
    unsigned long pattern_ctrl( unsigned long pos, char *patstr, tpat *pat, unsigned long index );

    void clear_pattern( tpat *pat, unsigned long cnt );
    void release_pattern( tpat *pat, unsigned long cnt );
    void print_pattern( tpat *pat, unsigned long cnt );
    unsigned long process_pattern( char *pattern, tpat *pat );

    unsigned long subset_match( char c, char *set );
    unsigned long substring_match( unsigned long pos, const char *buf, char *str );
    unsigned long subset_substring_match( unsigned long i, unsigned long fs, char *buf, tpat *pat, unsigned long index);
    unsigned long find( unsigned long i, unsigned long fs, char *buf, unsigned long *src, int *found, tpat *pat, unsigned long index, unsigned long patcnt );
    void search( unsigned long fs, char *buf, unsigned long *src, tpat *pat, unsigned long patcnt );

    unsigned long color( unsigned long i );

    void copy_substring( char *buf, tmatch *list, unsigned long index );
    unsigned long get_enclosure_count( const char *buf, unsigned long fs, const char *enclosure );

    void tokenize_groups( char *buf, unsigned long fs, const unsigned long *src, tmatch *match, unsigned long col_cnt );

    void init();
    void load( const char *filename );
    void load( const char *str, unsigned int len );
    void regextract( char *regexpat );
    void deinit();

    unsigned long getrowcount();
    unsigned long getcolcount();
    char *getval( unsigned long row, unsigned long col );
    unsigned long getval_start( unsigned long row, unsigned long col );
    unsigned long getval_end( unsigned long row, unsigned long col );

    unsigned long getfs();
    char *getbuf();
};

#endif

