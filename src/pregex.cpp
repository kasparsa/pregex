#include "pregex.h"


PRegEx::PRegEx()
{
    group = 1;

    match_count = 0;

    verbose_level = 0;

    buf = NULL;
    fs = 0;

    match = NULL;
    col_cnt = 0;
    row_cnt = 0;

    balance_depth = 0;

    balance_index = 0;

    strcpy(balanced_symbol_to_find, " ");
    strcpy(balanced_symbol_initiator, " ");

    balance_can_proceed = 0;
    balance_related = 0;
}


PRegEx::~PRegEx()
{

}


void PRegEx::verbosity(int level)
{
    verbose_level = level;
}


void PRegEx::print_pattern( tpat *pat, unsigned long cnt )
{
    for ( unsigned long i = 0; i < cnt; i++ ) printf( "Pat[%lu]: matched=%lu b=%lu e=%lu exact=%lu zeroorone=%lu zeroormore=%lu oneormore=%lu balanced=%lu bopen=%lu bclose=%lu gs=%lu ge=%lu mark=%lu pat='%s'\n", i,pat[i].matched,pat[i].atbeginning, pat[i].atend, pat[i].exact, pat[i].zeroorone, pat[i].zeroormore, pat[i].oneormore, pat[i].balanced, pat[i].balanced_open, pat[i].balanced_close, pat[i].gs, pat[i].ge, pat[i].mark, pat[i].pat );
    printf("\n");
}


void PRegEx::print_match_debug( char *buf, unsigned long len, unsigned long *src )
{
    for( unsigned long i = 0; i < len; i++ ) {

        if ( src[i] ) printf( "\033[0;%lum\033[4m%c\033[0m", color(src[i]), buf[i] );

        else printf( "%c", buf[i] );
    }
}


void PRegEx::print_tokenized_groups( tmatch *match, unsigned long col_cnt )
{
    typedef char tcolfmt[255];

    unsigned long *col_width = (unsigned long *)malloc(col_cnt * sizeof(unsigned long));
    tcolfmt *col_fmt = (tcolfmt *)malloc(col_cnt*sizeof(tcolfmt));

    for ( unsigned long i = 0; i < col_cnt; i++ ) col_width[i] = 0;

    for( unsigned long y = 0; y < match_count; y++ ) {
        for( unsigned long x = 0; x < col_cnt; x++ ) {
            if ( strlen(match[y*col_cnt+x].str) > col_width[x] ) col_width[x] = strlen(match[y*col_cnt+x].str);
        }
    }

    for ( unsigned long i = 0; i < col_cnt; i++ ) {
        printf( "Column %lu width : %lu\n", i, col_width[i] );
        sprintf( (char*)&col_fmt[i], "['%%%lus'] ", col_width[i] );
    }

    printf("\n");
    for( unsigned long y = 0; y < match_count; y++ ) {
        for( unsigned long x = 0; x < col_cnt; x++ ) {
            printf( col_fmt[x], match[y*col_cnt+x].str );
        }
        printf("\n");
    }

    free(col_width);
    col_width = NULL;

    free(col_fmt);
    col_fmt = NULL;
}


long PRegEx::substring_found( char *str, char *substr )
{
    if ( strlen( str ) < strlen( substr ) ) return -1;

    unsigned long l = strlen( str ) - strlen( substr );

    for ( unsigned long i = 0; i <= l; i++ ) {

        if ( substring_match( i, str, substr ) ) return i;
    }

    return -1;
}


char *PRegEx::replace_substr( char *str, char *substr, unsigned long pos, unsigned long len )
{
    len -= 1;

    char *newstr = ( char* ) malloc( strlen( str ) + strlen( substr ) + 1 );

    char *a = ( char* ) malloc( pos + 1 );
    strncpy( a, str, pos );
    a[ pos ] = '\0';

    char *c = ( char* ) malloc( strlen( str ) - pos - len + 1);
    strncpy( c, str + pos + len + 1, strlen( str ) - pos - len );
    c[ strlen( str ) - pos - len ] = '\0';

    sprintf( newstr, "%s%s%s", a, substr, c );

    free(str);
    free( a );
    free( c );

    return newstr;
}


char *PRegEx::replace_abrv( char *str )
{
#define PAT_CNT 5

    const char *abrv[PAT_CNT] = { "A-Z", "a-z", "a-Z", "0-9", "\\n" };

    char *new_substr = NULL;

    for ( unsigned long i = 0; i < PAT_CNT; i++ ) {

        long pos = substring_found( str, (char*)abrv[i] );

        if ( pos != -1 ) {

            if ( strcmp( abrv[i], "A-Z" ) == 0 ) new_substr = (char*)uppercase;
            if ( strcmp( abrv[i], "a-z" ) == 0 ) new_substr = (char*)lowercase;
            if ( strcmp( abrv[i], "a-Z" ) == 0 ) new_substr = (char*)letters;
            if ( strcmp( abrv[i], "0-9" ) == 0 ) new_substr = (char*)digit;
            if ( strcmp( abrv[i], "\\n" ) == 0 ) new_substr = (char*)newline;

            str = replace_substr( str, new_substr, pos, strlen( abrv[i] ) );

        }
    }

    return str;
}


unsigned long PRegEx::pattern_str( unsigned long pos, char *patstr, tpat *pat, unsigned long index )
{
    unsigned long start = pos;

    while( subset_match( patstr[pos], (char*)patchar ) ) pos++;

    if ( pos > start ) {

        pat[index].pat = ( char* ) malloc( pos - start + 1 );
        strncpy( pat[index].pat, &patstr[start], pos - start );
        pat[index].pat[ pos - start ] = '\0';
        pat[index].exact = 1;
        pat[index].releasable = 1;

    }

    return pos;
}


unsigned long PRegEx::patter_esc( unsigned long pos, char *patstr, tpat *pat, unsigned long index )
{
    unsigned long nextpos = pos;

    if ( patstr[pos] == '\\') {
        nextpos++;
        if ( strlen( patstr) > pos + 1 ) {
            if( ( patstr[pos+1] == 'w' ) || ( patstr[pos+1] == 's' ) || ( patstr[pos+1] == 'n' ) || ( patstr[pos+1] == 'd' ) || ( patstr[pos+1] == 'h' ) || ( patstr[pos+1] == '(' ) || ( patstr[pos+1] == ')' ) || patstr[pos+1] == '*'  || patstr[pos+1] == '%' ) nextpos++;
            if ( patstr[pos+1] == 'w' ) pat[index].pat = (char*)wordsym;
            if ( patstr[pos+1] == 's' ) pat[index].pat = (char*)blank;
            if ( patstr[pos+1] == 'n' ) pat[index].pat = (char*)newline;
            if ( patstr[pos+1] == 'd' ) pat[index].pat = (char*)digit;
            if ( patstr[pos+1] == 'h' ) pat[index].pat = (char*)hex;
            if ( patstr[pos+1] == '%' ) {
                pat[index].pat = (char*)"";
                pat[index].balanced = 1;
                pat[index].balanced_open=index-1;
                pat[index-1].balanced = 1;
            }
            if ( patstr[pos+1] == '(' ) {
                pat[index].pat = (char*)"";
                pat[index].gs = 1;
            }
            if ( patstr[pos+1] == ')' ) {
                pat[index].pat = (char*)"";
                pat[index].ge = 1;
            }
            if ( patstr[pos+1] == '*' )  {
                if( strlen( patstr) > pos + 2 ) {
                    unsigned long j = 0;

                    pat[index].pat = (char*)malloc(strlen(symbol));
                    pat[index].releasable = 1;

                    for( unsigned long i = 0; i < strlen(symbol); i++ ) {
                        unsigned int gs_offset = 0;

                        if (pat[index-1].gs == 1) gs_offset = 2;

                        if(patstr[pos+2+gs_offset] == '\\') {
                            if(patstr[pos+3+gs_offset] == 'n') {
                                if( symbol[i] != '\n' ) pat[index].pat[j++] = symbol[i];
                            } else {
                                if( symbol[i] != patstr[pos+4] ) pat[index].pat[j++] = symbol[i];
                            }
                        } else {
                            if( symbol[i] != patstr[pos+2]  ) {
                                pat[index].pat[j] = symbol[i];
                                j++;
                            }
                        }
                    }
                    pat[index].pat[j] = '\0';
                    pat[index].zeroormore = 1;
                } else pat[index].pat = (char*)symbol;
            }

            if ( strlen( patstr) > pos + 2) {
                if ( ( patstr[pos+2] == '*' ) || ( patstr[pos+2] == '+' ) || ( patstr[pos+2] == '?' ) ) nextpos++;
                if ( patstr[pos+2] == '*' ) pat[index].zeroormore = 1;
                if ( patstr[pos+2] == '+' ) pat[index].oneormore = 1;
                if ( patstr[pos+2] == '?' ) pat[index].zeroorone = 1;
            }
        }

    }
    return nextpos;
}


unsigned long PRegEx::pattern_allowed( unsigned long pos, char *patstr, tpat *pat, unsigned long index )
{
    char *tmppatstr = NULL;

    unsigned long start = pos;
    if ( patstr[pos] == '[') {
        pos++;
        start = pos;
        while( patstr[pos] != ']' )  pos++;
    }

    if ( pos > start ) {

        tmppatstr = ( char* ) malloc( pos - start + 1 );
        strncpy( tmppatstr, &patstr[start], pos - start );
        tmppatstr[ pos - start ] = '\0';

        pat[index].pat = replace_abrv( tmppatstr );
        pat[index].releasable = 1;

        pos++;

        if ( patstr[pos] == '*' ) {
            pat[index].zeroormore = 1;
            pos++;
        }
        if ( patstr[pos] == '+' ) {
            pat[index].oneormore = 1;
            pos++;
        }
        if ( patstr[pos] == '?' ) {
            pat[index].zeroorone = 1;
            pos++;
        }
    }

    return pos;
}


unsigned long PRegEx::pattern_ctrl( unsigned long pos, char *patstr, tpat *pat, unsigned long index )
{
    if( patstr[pos] == '^') {
        pat[index].pat = (char*)"";
        pat[index].atbeginning = 1;
        pos+=1;
    }
    if( patstr[pos] == '$') {
        pat[index].pat = (char*)"";
        pat[index].atend = 1;
        pos+=1;
    }

    return pos;
}


void PRegEx::clear_pattern( tpat *pat, unsigned long cnt )
{
    for ( unsigned long i = 0; i < cnt; i++ ) {
        pat[i].zeroorone = 0;
        pat[i].zeroormore = 0;
        pat[i].oneormore = 0;
        pat[i].balanced = 0;
        pat[i].exact = 0;
        pat[i].pat = NULL;
        pat[i].releasable = 0;
        pat[i].gs = 0;
        pat[i].ge = 0;
        pat[i].mark = 1;
        pat[i].atbeginning = 0;
        pat[i].atend = 0;
        pat[i].matched = 0;
        pat[i].balanced_open = 0;
        pat[i].balanced_close = 0;
    }
}


void PRegEx::release_pattern( tpat *pat, unsigned long cnt )
{
    for ( unsigned long i = 0; i < cnt; i++ ) {

        if ( pat[i].releasable ) {

            free(pat[i].pat);

            pat[i].pat = NULL;
        }

    }
}


unsigned long PRegEx::process_pattern( char *pattern, tpat *pat )
{
    unsigned long len = strlen( pattern );

    if ( verbose_level >= 1 )
    {
        printf("Pat='%s'\n",pattern);
        printf("Patchar='%s'\n",patchar);
    }

    unsigned long start = 0;
    unsigned long patcnt = 0;
    unsigned long groupval = 1;

    while( start < len ) {

        unsigned long end = pattern_ctrl( start, pattern, pat, patcnt );
        if ( end > start ) patcnt++;
        start = end;

        end = pattern_str( start, pattern, pat, patcnt );
        if ( end > start ) patcnt++;
        start = end;

        end = patter_esc( start, pattern, pat, patcnt );
        if ( end > start ) patcnt++;
        start = end;

        end = pattern_allowed( start, pattern, pat, patcnt );
        if ( end > start ) patcnt++;
        start = end;

    }


    for( unsigned long p = 0; p < patcnt; p++ ) {
        if ( pat[p].gs || pat[p].ge ) {
            if ( pat[p].gs ) {
                group++;
                groupval = group;
            }
            if ( pat[p].ge ) groupval = 1;

            for( unsigned long k = p; k < patcnt; k++ ) pat[k].mark = groupval;
        }
    }

    /*Begin: Balanced pattern marking */
    int uses_balabced = 0;
    for( unsigned long p = 0; p < patcnt; p++ ) {
        if ( pat[p].balanced ) {
            uses_balabced = 1;
            break;
        }
    }

    if ( uses_balabced ) {
        unsigned long p = 0;
        char open, close;
        unsigned long id_open, id_close;
        while( pat[p].balanced == 0 ) p++;
        p++;
        id_open = pat[p].balanced_open;
        open = pat[id_open].pat[0];

        if ( open == '(' ) close = ')';
        if ( open == '[' ) close = ']';
        if ( open == '{' ) close = '}';

        p = id_open;
        while ( ( p < patcnt ) && ( pat[p].pat[0] != close ) ) p++;
        id_close = p;

        for ( p = id_open; p <= id_close; p++ ) {
            pat[p].balanced_open = id_open;
            pat[p].balanced_close = id_close;
            pat[p].balanced = 1;
        }
        for( unsigned long p = 0; p < patcnt; p++ ) {

            if ( pat[p].balanced ) {
                open = pat[pat[p].balanced_open].pat[0];
            }
        }
    }
    /*End: Balanced pattern marking */

    if( verbose_level == 2 ) print_pattern( pat, patcnt );

    return patcnt;
}


unsigned long PRegEx::substring_match( unsigned long pos, const char *buf, char *str )
{
    unsigned long len = strlen(str);

    for( unsigned long j = 0; j < len; j++ ) if ( buf[pos+j] != str[j] ) return 0;

    return 1;
}


unsigned long PRegEx::subset_match( char c, char *set )
{
    unsigned long len = strlen( set );

    for ( unsigned long i = 0; i < len; i++ ) if ( c == set[ i ] ) return 1;

    return 0;
}


unsigned long PRegEx::subset_substring_match( unsigned long i, unsigned long fs, char *buf, tpat *pat, unsigned long index)
{
    bool match = subset_match( buf[i], pat[index].pat );

    /* For quantifiers "?" and "*" */
    if ( pat[index].zeroorone || pat[index].zeroormore ) {

        if ( match == 1 ) {
            if ( pat[index].zeroorone ) {
                return i + 1;
            }
            if ( pat[index].zeroormore ) {
                if ( pat[index].balanced) {

                    balance_depth = 1;
                    while ( ( i < fs ) && ( balance_depth != 0 ) ) {

                        if ( buf[i] == pat[pat[index].balanced_open].pat[0] ) balance_depth++;
                        if ( buf[i] == pat[pat[index].balanced_close].pat[0] ) balance_depth--;

                        i++;
                    }

                    return i-1;

                } else {
                    while( ( i < fs ) && ( subset_match( buf[i], pat[index].pat ) ) ) i++;
                    return i;
                }
            }
        } else {
            return i;
        }

    }

    /* For quantifier "+" */
    if ( pat[index].oneormore ) {

        if ( match == 1) {
            while( ( i < fs ) && ( subset_match( buf[i], pat[index].pat ) ) ) i++;
            return i;
        } else {
            return 0;       /* Pattern doesn't match */
        }

    }

    /* Without quantifier */
    return (match) ? i + 1 : 0;
}


unsigned long PRegEx::find( unsigned long i, unsigned long fs, char *buf, unsigned long *src, int *found, tpat *pat, unsigned long index, unsigned long patcnt )
{
    *found = 0;

    if ( pat[index].exact ) {

        if ( ( index > 0 ) && ( pat[index-1].atbeginning ) ) {

            if ( substring_match( i, buf, pat[index].pat ) ) {
                if ( i == 0 || buf[i-1] == '\n' ) {
                    unsigned long len = strlen( pat[index].pat );
                    for( unsigned long j = i; j < i+len; j++ ) src[j] = pat[index].mark;
                    i += len;
                    *found = 1;
                }
            }

        } else {

            if ( ( index < patcnt - 1 ) && ( pat[index + 1].atend ) ) {


                if ( substring_match( i, buf, pat[index].pat ) ) {
                    unsigned long len = strlen( pat[index].pat );
                    if ( buf[i+len ] == '\n' ) {
                        for( unsigned long j = i; j < i+len; j++ ) src[j] = pat[index].mark;
                        i += len;
                        *found = 1;
                    }
                }


            } else {
                if ( substring_match( i, buf, pat[index].pat ) ) {
                    unsigned long len = strlen( pat[index].pat );
                    for( unsigned long j = i; j < i+len; j++ ) src[j] = pat[index].mark;
                    i += len;
                    *found = 1;
                }
            }

        }


    } else {

        if ( ( index > 0 ) && ( pat[index-1].atbeginning ) ) {

            unsigned long j;
            if ( ( j = subset_substring_match( i, fs, buf, pat, index ) ) ) {
                if ( i == 0 || buf[i-1] == '\n' ) {
                    for(unsigned long k = i; k <= j; k++ ) src[k] = pat[index].mark;
                    i = j;
                    *found = 1;

                }
            }

        } else {
            if ( ( index < patcnt - 1 ) && ( pat[index + 1].atend ) ) {

                unsigned long j;
                if ( ( j = subset_substring_match( i, fs, buf, pat, index ) ) ) {
                    if ( buf[j] == '\n' ) {
                        for(unsigned long k = i; k <= j; k++ ) src[k] = pat[index].mark;
                        i = j;
                        *found = 1;

                    }
                }

            } else {

                unsigned long j;
                if ( ( j = subset_substring_match( i, fs, buf, pat, index ) ) ) {

                    for(unsigned long k = i; k < j; k++ ) src[k] = pat[index].mark;
                    i = j;
                    *found = 1;

                }
            }
        }
    }

    if ( *found == 0 && pat[index].zeroormore ) {
        *found = 1;
    }

    return i;
}


void PRegEx::search( unsigned long fs, char *buf, unsigned long *src, tpat *pat, unsigned long patcnt )
{
    int found = 0;
    unsigned long i = 0;

    balance_depth = 1;
    while( i < fs ) {

        found = 1;
        unsigned long start = i;

        for( unsigned long p = 0; p < patcnt; p++ ) {

            if ( pat[p].gs || pat[p].ge ) continue;

            if ( pat[p].atbeginning || pat[p].atend ) continue;

            if ( pat[p].balanced ) {

                if (strlen(pat[p].pat)==0) continue;                    /* balanced pattern controll character skipping */

            }

            if ( found ) {

                i = find( i, fs, buf, src, &found, pat, p, patcnt );

            }
        }

        if ( found == 0 ) {
            for ( unsigned long j = start; j <= i; j++ ) {
                src[j]=0;
                i = start;
            }
        } else match_count++;

        if( found == 0 ) i++;

    }
}


unsigned long PRegEx::color( unsigned long i )
{
    if ( i ==  1 ) return 89;
    if ( i ==  2 ) return 31;
    if ( i ==  3 ) return 32;
    if ( i ==  4 ) return 33;
    if ( i ==  5 ) return 34;
    if ( i ==  6 ) return 35;
    if ( i ==  7 ) return 36;
    if ( i ==  8 ) return 90;
    if ( i ==  9 ) return 91;
    if ( i == 10 ) return 92;
    if ( i == 11 ) return 93;
    if ( i == 12 ) return 94;
    if ( i == 13 ) return 95;
    if ( i == 14 ) return 96;
    if ( i == 15 ) return 97;
    if ( i > 15 ) return 31;

    return 89;
}


void PRegEx::copy_substring( char *buf, tmatch *list, unsigned long index )
{
    list[ index ].str = ( char* ) malloc( list[ index ].end - list[ index ].start + 1 );

    strncpy( list[ index ].str, &buf[ list[ index ].start], list[ index ].end - list[ index ].start );

    list[ index ].str[ list[ index ].end - list[ index ].start ] = '\0';
}


unsigned long PRegEx::get_enclosure_count( const char *buf, unsigned long fs, const char *enclosure )
{
    unsigned long depth = 0;
    unsigned long count = 0;

    for( unsigned long i = 0; i < fs; i++ ) {

        if ( buf[i] == enclosure[0] ) {
            if ( depth == 0 ) count++;
            depth++;
        }

        if ( buf[i] == enclosure[1] ) depth--;
    }

    return count;
}


void PRegEx::tokenize_groups( char *buf, unsigned long fs, const unsigned long *src, tmatch *match, unsigned long col_cnt )
{
    unsigned long i = 0;
    unsigned long grp = 2;
    unsigned long found = 0;
    unsigned long col = 0;
    unsigned long row = 0;

    for( unsigned long j = 0; j < group - 1; j++ ) {
        while( i < fs ) {
            if ( src[i] == grp ) {
                if( found == 0 ) {
                    found = 1;
                    match[row * col_cnt + col].start = i;
                }
            } else {
                if( found == 1 ) {
                    found = 0;
                    match[row * col_cnt + col].end = i;
                    copy_substring( buf, match, row * col_cnt + col );
                    row++;
                }
            }
            i++;
        }
        grp++;
        col++;
        row = 0;
        i = 0;
    }
}


void PRegEx::init()
{
    group = 1;

    match_count = 0;

    balance_depth = 0;

    balance_can_proceed = 0;

    balance_related = 0;

    balance_index = 0;

    strcpy(balanced_symbol_to_find, " ");
}


void PRegEx::load( const char *filename )
{
    FILE *f = fopen( filename, "r" );

    if ( f == NULL ) {
        printf( "Error opening file %s\n", filename );
        exit( 255 );
    }

    fseek( f, 0, SEEK_END );

    fs = ftell( f );

    rewind( f );

    buf = ( char* ) malloc( fs );

    size_t s = fread( buf, fs, 1, f );
    if ( s < 1 ) {
        printf( "Not entire file have been red %s %lu\n", filename, s );
        exit( 255 );
    }

    fclose( f );
}


void PRegEx::load(const char* str, unsigned int len)
{
    buf = ( char* ) malloc( len );

    fs = len;

    memcpy( buf, str, len );
}


void PRegEx::regextract( char *regexpat )
{
    unsigned long *src = ( unsigned long* ) calloc( 1, fs * sizeof(unsigned long) );

    tpat pat[65536];

    clear_pattern( pat, 65536 );

    unsigned long patcnt = process_pattern( regexpat, pat );

    search( fs, buf, src, pat, patcnt );

    if (verbose_level == 2 ) print_match_debug( buf, fs, src );

    if (verbose_level >= 1 ) printf("groups: %lu\n", group  - 1);
    if (verbose_level >= 1 ) printf("match count: %lu\n", match_count);

    if ( ( group > 1 ) && ( match_count > 0 ) )  {

        col_cnt = group - 1;
        row_cnt = match_count;

        match = ( tmatch* ) calloc( col_cnt * match_count, sizeof( tmatch ) );

        if (verbose_level >= 1 ) printf("Tokenizing..\n");
        tokenize_groups( buf, fs, src, match, col_cnt );
        if (verbose_level >= 1 ) printf("Done\n");

        if (verbose_level == 2 ) print_tokenized_groups( match, col_cnt );
    }

    release_pattern( pat, patcnt );

    free(src);
}


void PRegEx::deinit()
{
    for ( unsigned long i = 0; i < row_cnt; i++ ) {
        for ( unsigned long j = 0; j < col_cnt; j++ ) {
            free( match[i*col_cnt+j].str );
        }
    }

    if ( row_cnt ) {

        free(match);

        match = NULL;
    }

    free(buf);

    buf = NULL;

    col_cnt = 0;
    row_cnt = 0;

    fs = 0;
}

unsigned long PRegEx::getrowcount()
{
    return row_cnt;
}


unsigned long PRegEx::getcolcount()
{
    return col_cnt;
}


unsigned long PRegEx::getfs()
{
    return fs;
}


char *PRegEx::getbuf()
{
    return buf;
}


char *PRegEx::getval( unsigned long row, unsigned long col )
{
    if ( row > row_cnt ) {
        printf( "ERROR: row %lu out of bounds. Max row count %lu\n", row, row_cnt );
        exit(255);
    }

    if ( col > col_cnt ) {
        printf( "ERROR: col %lu out of bounds. Max col count %lu\n", col, col_cnt );
        exit(255);
    }

    if ( match == NULL ) {
        printf( "ERROR match array NULL\n" );
        exit(255);
    }

    return match[ row * col_cnt + col ].str;
}


unsigned long PRegEx::getval_start( unsigned long row, unsigned long col )
{
    if ( row > row_cnt ) {
        printf( "ERROR: row %lu out of bounds. Max row count %lu\n", row, row_cnt );
        exit(255);
    }

    if ( col > col_cnt ) {
        printf( "ERROR: col %lu out of bounds. Max col count %lu\n", col, col_cnt );
        exit(255);
    }

    if ( match == NULL ) {
        printf( "ERROR match array NULL\n" );
        exit(255);
    }

    return match[ row * col_cnt + col ].start;
}


unsigned long PRegEx::getval_end( unsigned long row, unsigned long col )
{
    if ( row > row_cnt ) {
        printf( "ERROR: row %lu out of bounds. Max row count %lu\n", row, row_cnt );
        exit(255);
    }

    if ( col > col_cnt ) {
        printf( "ERROR: col %lu out of bounds. Max col count %lu\n", col, col_cnt );
        exit(255);
    }

    if ( match == NULL ) {
        printf( "ERROR match array NULL\n" );
        exit(255);
    }

    return match[ row * col_cnt + col ].end;
}

