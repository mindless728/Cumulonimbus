#include "utils.h"
#include "c_io.h"
#include "startup.h"
#include "x86arch.h"

/**
*	@return	Absolute value of num
*/
int abs(int num){
	if(num < 0){
		num *= -1;
	}
	return num;
}

/**
*
*	@param	x	Values to raise to y
*	@param	y	Power to raise x
*
*	@return	Returns x raised to the power of y
*/
int pow(int x, int y){
	if(y==0){
		return 1;
	}

	return x*pow(x, --y);
}

/**
*	Counts the number of characters in the null-terminated string
*
*	@param	str	String to count characters in
*	@return	The number of bytes in str
*/
int strlen(char* str){
	int i=0;
	while(str[i] != 0x00){
		i++;
	}
	return i;
}

/**
*	Sets size bytes of buf to value
*
*	@param	buf		Buffer to modify
*	@param	value	Value to set to buffer
*	@param	size	Size of buffer
*/
void* memset(void* buf, uint8_t value, int size){
	int i=0;

	if(size < 0x4){
		for(i=0; i<size; i++){
			((uint8_t*)buf)[i] = value;
		}
	}
	else{
		int loops = size>>2;	//Divide by four
		uint32_t val;
		((uint8_t*) &val)[0] = value;
		((uint8_t*) &val)[1] = value;
		((uint8_t*) &val)[2] = value;
		((uint8_t*) &val)[3] = value;

		for(i=0; i<loops; i++){
			((uint32_t*) buf)[i] = val;
		}

		loops = size &0x3;

		for(i=0; i<loops; i++){
			((uint8_t*)buf)[i] = value;
		}
	}
	return buf;
}


/**
*	Prints a full character map
*/
void console_char_chart(void){
	int i=0;
	c_printf(" 0 1 2 3 4 5 6 7 8 9 A B C D E F\n");

	for(i=0; i<=255; i++){
		if( (i%0x10) == 0 ){
			c_printf("\n%1x ", (i>>4));
		}

		c_putchar((char)i);

		c_putchar(' ');
	}
}


//int sprintf(char *str, const char** f){
//	char	*fmt = *f;
//	int	*ap;
//	char	buf[ 12 ];
//	char	ch;
//	int	leftadjust;
//	int	width;
//	int	len;
//	int	padchar;
//	int index = 0;

//	/*
//	** Get characters from the format string and process them
//	*/
//	ap = (int *)( f + 1 );
//	while( (ch = *fmt++) != '\0' ){
//		/*
//		** Is it the start of a format code?
//		*/
//		if( ch == '%' ){
//			/*
//			** Yes, get the padding and width options (if there).
//			** Alignment must come at the beginning, then fill,
//			** then width.
//			*/
//			leftadjust = 0;
//			padchar = ' ';
//			width = 0;
//			ch = *fmt++;
//			if( ch == '-' ){
//				leftadjust = 1;
//				ch = *fmt++;
//			}
//			if( ch == '0' ){
//				padchar = '0';
//				ch = *fmt++;
//			}
//			while( ch >= '0' && ch <= '9' ){
//				width *= 10;
//				width += ch - '0';
//				ch = *fmt++;
//			}

//			/*
//			** What data type do we have?
//			*/
//			switch( ch ){
//			case 'c':
//				// ch = *( (int *)ap )++;
//				ch = *ap++;
//				buf[ 0 ] = ch;
//				buf[ 1 ] = '\0';
//				padstr( x, y, buf, 1, width, leftadjust, padchar );
//				break;

//			case 'd':
//				// len = cvtdec( buf, *( (int *)ap )++ );
//				len = cvtdec( buf, *ap++ );
//				padstr( x, y, buf, len, width, leftadjust, padchar );
//				break;

//			case 's':
//				// str = *( (char **)ap )++;
//				str = (char *) (*ap++);
//				padstr( x, y, str, -1, width, leftadjust, padchar );
//				break;

//			case 'x':
//				// len = cvthex( buf, *( (int *)ap )++ );
//				len = cvthex( buf, *ap++ );
//				padstr( x, y, buf, len, width, leftadjust, padchar );
//				break;

//			case 'o':
//				// len = cvtoct( buf, *( (int *)ap )++ );
//				len = cvtoct( buf, *ap++ );
//				padstr( x, y, buf, len, width, leftadjust, padchar );
//				break;

//			}
//		}
//		else {
//			str[index++] = ch;
//		}
//	}
//	return index;
//}

