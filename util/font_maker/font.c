/**
 * @author Benjamin Mayes
 * @desciption Turns a text file into a source file specifying a font that can
 * used by the VESA drivers.
 */

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv ) {
    FILE* font = fopen( argv[1], "r" );
    if( !font ) {
        perror( "fopen" );
        return 1;
    }

    int width;
    int height;
    fscanf( font, "%d %d", &width, &height );
    int num;
    printf( "void _init_font() {\n" );
    while( fscanf( font, "%d", &num ) != EOF ) {
        int i = 0;
        int j = 0;
        char c = 0;
        short output;
        for( i = 0; i < height; ++i ) {
            for( j = 0; j < width; ++j ) {
                do {
                    fscanf( font, "%c", &c );
                } while( c != '0' && c != '1' ); 
                if( c == '1' ) {
                   output = -1;
                } else {
                   output = 0;
                }   
                printf( "  font[%d][%d][%d] = 0x%x;\n", num, j, i, output&0xFFFF );
            }
        }
        printf( "\n" );
    }
    printf( "}\n" );


    fclose( font );
    return 0;
}
