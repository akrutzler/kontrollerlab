#include <avr/io.h>
#include "uart.h"
#include <avr/interrupt.h>

#include <avr/pgmspace.h>
#include <inttypes.h>


typedef uint8_t u08;
typedef uint16_t u16;


static volatile u08* allMems = (volatile u08*)0;


u08 UART_ReceiveByte()
{
	u16 character;
	character = uart_getc();
	while ( character & UART_NO_DATA )
		character = uart_getc();
	return character;
}


int main()
{
	u08 cmd, val;
	u16 pos, pos2;
	u08* ptr;
        DDRB = 'B';
        PORTB = 'B';
	DDRD=0xFF;
        PORTD=0xFF;
	uart_init( UART_BAUD_SELECT( 19200, F_CPU ) );
	sei();
	while(1)
	{
		cmd=UART_ReceiveByte();
		if (cmd=='R')
		{
			pos = UART_ReceiveByte();
			pos <<= 8;
			pos |= UART_ReceiveByte();
			val = allMems[ pos ];
			uart_putc( val );
		}
		else if (cmd=='W')
		{
			pos = UART_ReceiveByte();
			pos <<= 8;
			pos |= UART_ReceiveByte();
			val = UART_ReceiveByte();
			allMems[ pos ] = val;
			uart_putc( 'O' );
		}
	}
}
