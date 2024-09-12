#include "pregex.h"


int main( int argc, char *argv[])
{
    if ( argc < 3 ) {

        printf("Usage ./parser file pattern\n");
        printf("Example ./parser ../testdata/testdata01.in \"\\n\\([A-Z]+\\);\\([A-Za-z ()/0-9]+\\);\\([0-9]+\\)\"\n");

        exit(0);
    }

    PRegEx RegEx;

    RegEx.init();

    RegEx.verbosity(2);

    RegEx.load(argv[1]);

    RegEx.regextract(argv[2]);

    RegEx.deinit();

    exit(0);
}

