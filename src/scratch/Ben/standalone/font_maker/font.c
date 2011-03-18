/**
 * @author Benjamin Mayes
 * @desciption Turns a text file into a source file specifying a font that can
 * used by the VESA drivers.
 */

#include <stdio.h>
#include <stdlib.h>
int width;
int height;
FILE* font;

void print_char(int c) {
    int j,k,test=0; 
    printf( "//Character 0x%02x (dec %d)\n{", c,c );
    for( j = 0; j < height; ++j ) {
        printf( "{" );
        for( k = 0; k < width; ++k ) {
            char x;
            ++test;
            do {
                fscanf(font, "%c", &x );
            } while( x != '0' &&  x != '1' );
            if( x == '0' ) {
                c=0;
            } else {
                c=0xFFFF;
            }
            printf("0x%04x",c);
            if(k < width - 1) printf(", ");
        }
        printf ("}");
        if( j < height - 1 ) {
            printf( ", \n");
        } else {
            if( c < 255 ) {
                printf( "},//%d\n",test );
            } else {
                printf( "}//%d\n",test );
            }
        }
    }
}

void print_blank_char(int c) {
    int j,k; 
    printf( "//Character 0x%02x (dec %d)\n{", c,c );
    for( j = 0; j < height; ++j ) {
        printf( "{" );
        for( k = 0; k < width; ++k ) {
            printf("0x0000");
            if(k < width - 1) printf(", ");
        }
        printf ("}");
        if( j < height - 1 ) {
            printf( ", \n");
        } else {
            if( c < 255 ) {
                printf( "},\n" );
            } else {
                printf( "}\n" );
            }
        }
    }
}


int main( int argc, char** argv ) {
    font = fopen( argv[1], "r" );
    if( !font ) {
        perror( "fopen" );
        return 1;
    }
    int i = 0;
    int j = 0;
    int k = 0;
        
    printf( "/**\n * @author Benjamin Mayes and font_maker (written by Benjamin Mayes)\n * @description This is an automatically generated file that generates fonts \n *used by the kernel early on for debugging output.\n */\n\n");
    printf( "#include \"gs_font.h\"\n\n" );
    fscanf( font, "%d %d", &height, &width );
    int num;
    printf( "uint16_t font[128][%d][%d] = {\n", height, width );
    int last_num = 0;
    while( fscanf( font, "%d", &num ) != EOF ) {
        if( last_num < num - 1 ) {
            for( i = last_num; i < num; ++i ) {
                print_blank_char(i);
            }
        }
        last_num = num + 1;
        print_char(num);
    }

    ++num;
    for( i = num; i < 128; ++i ) {
        print_blank_char(i);
    }
        
    printf( "};\n" );


    fclose( font );
    return 0;
}
