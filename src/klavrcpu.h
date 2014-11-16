/***************************************************************************
 *   Copyright (C) 2006 by Martin Strasser                                 *
 *   strasser  a t  cadmaniac  d o t  org                                  *
 *   Special thanks to Mario Boikov                                        *
 *   squeeze  a t  cadmaniac  d o t  org                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KLAVRCPU_H
#define KLAVRCPU_H

#include "klavrflash.h"

typedef unsigned char u08;
typedef unsigned int u16;
typedef unsigned int u32;

// DEFINE REGISTER NAMES:
#define SREG "SREG"

// DEFINE_FLAGS
#define SREG_I 0x80
#define SREG_T 0x40
#define SREG_H 0x20
#define SREG_S 0x10
#define SREG_V 0x08
#define SREG_N 0x04
#define SREG_Z 0x02
#define SREG_C 0x01

// DEFINE X Y AND Z REGISTERS
#define ZH 31
#define ZL 30
#define YH 29
#define YL 28
#define XH 27
#define XL 26

class KLDebugger;

/**
This is the class that executes instructions for the AVR.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLAVRCPU
{
public:
    KLAVRCPU(KLDebugger* parent, const QString& name);

    ~KLAVRCPU();
    
    typedef enum {
        STATE_READ_DATA = 0,
        STATE_WAIT_FOR_READ_DATA_INTERNAL = 1, // For indirect loads/stores only
        STATE_READ_DATA_INTERNAL = 2,          // For indirect loads/stores only
        STATE_WAIT_FOR_READ_DATA = 3,
        STATE_WRITE_BACK = 4,
        STATE_WAIT_FOR_WRITE_BACK = 5,
        STATE_CMD_FINISHED = 6} CPU_STATE;

    // A '//' after an execute command means that this command
    // is already implemented.
    void execute_ADC( u08 Rd, u08 Rr ); //
    void execute_ADD( u08 Rd, u08 Rr ); //
    void execute_ADIW( u08 Rd, u16 K ); //
    void execute_AND( u08 Rd, u08 Rr ); //
    void execute_ANDI( u08 Rd, u16 K ); //
    void execute_ASR( u08 Rd ); //
    void execute_BCLR( u08 s ); //
    void execute_BLD( u08 Rd, u08 b ); //
    void execute_BRBC( u08 s, u08 k ); //
    void execute_BRBS( u08 s, u08 k ); //
    void execute_BRCC( u08 k ); //
    void execute_BRCS( u08 k ); //
    void execute_BREAK(  ); //
    void execute_BREQ( u08 k ); //
    void execute_BRGE( u08 k ); //
    void execute_BRHC( u08 k ); //
    void execute_BRHS( u08 k ); //
    void execute_BRID( u08 k ); //
    void execute_BRIE( u08 k ); //
    void execute_BRLO( u08 k ); //
    void execute_BRLT( u08 k ); //
    void execute_BRMI( u08 k ); //
    void execute_BRNE( u08 k ); //
    void execute_BRPL( u08 k ); //
    void execute_BRSH( u08 k ); //
    void execute_BRTC( u08 k ); //
    void execute_BRTS( u08 k ); //
    void execute_BRVC( u08 k ); //
    void execute_BRVS( u08 k ); //
    void execute_BSET( u08 s ); //
    void execute_BST( u08 Rd, u08 b ); //
    void execute_CALL( u32 k ); //
    void execute_CBI( u08 A, u08 b ); //
    void execute_CBR( u08 Rd, u08 K ); //
    void execute_CLC(  ); //
    void execute_CLH(  ); //
    void execute_CLI(  ); //
    void execute_CLN(  ); //
    void execute_CLR( u08 Rd ); //
    void execute_CLS(  ); //
    void execute_CLT(  ); //
    void execute_CLV(  ); //
    void execute_CLZ(  ); //
    void execute_COM( u08 Rd ); //
    void execute_CP( u08 Rd, u08 Rr ); //
    void execute_CPC( u08 Rd, u08 Rr ); //
    void execute_CPI( u08 Rd, u08 K ); //
    void execute_CPSE( u08 Rd, u08 Rr ); //
    void execute_DEC( u08 Rd ); //
    void execute_EICALL(  );
    void execute_EIJMP(  );
    void execute_ELPM1(  );
    void execute_ELPM2( u08 Rd );
    void execute_ELPM3( u08 Rd );
    void execute_EOR( u08 Rd, u08 Rr ); //
    void execute_FMUL( u08 Rd, u08 Rr );
    void execute_FMULS( u08 Rd, u08 Rr );
    void execute_FMULSU( u08 Rd, u08 Rr );
    void execute_ICALL(  ); //
    void execute_IJMP(  ); //
    void execute_IN( u08 Rd, u08 A ); //
    void execute_INC( u08 Rd ); //
    void execute_JMP( u32 k ); //
    void execute_LDX1( u08 Rd ); //
    void execute_LDX2( u08 Rd ); //
    void execute_LDX3( u08 Rd ); //
    void execute_LDY1( u08 Rd ); //
    void execute_LDY2( u08 Rd ); //
    void execute_LDY3( u08 Rd ); //
    void execute_LDY4( u08 Rd, u08 q ); //
    void execute_LDZ1( u08 Rd ); //
    void execute_LDZ2( u08 Rd ); //
    void execute_LDZ3( u08 Rd ); //
    void execute_LDZ4( u08 Rd, u08 q ); //
    void execute_LDI( u08 Rd, u08 K ); //
    void execute_LDS( u08 Rd, u08 k ); //
    void execute_LPM1(  ); //
    void execute_LPM2( u08 Rd ); //
    void execute_LPM3( u08 Rd ); //
    void execute_LSL( u08 Rd ); //
    void execute_LSR( u08 Rd ); //
    void execute_MOV( u08 Rd, u08 Rr ); //
    void execute_MOVW( u08 Rd, u08 Rr ); //
    void execute_MUL( u08 Rd, u08 Rr ); //
    void execute_MULS( u08 Rd, u08 Rr ); //
    void execute_MULSU( u08 Rd, u08 Rr ); //
    void execute_NEG( u08 Rd ); //
    void execute_NOP(  ); //
    void execute_OR( u08 Rd, u08 Rr ); //
    void execute_ORI( u08 Rd, u08 K ); //
    void execute_OUT( u08 A, u08 Rr ); //
    void execute_POP( u08 Rd ); //
    void execute_PUSH( u08 Rd ); //
    void execute_RCALL( u16 k ); //
    void execute_RET(  ); //
    void execute_RETI(  ); //
    void execute_RJMP( u16 k ); //
    void execute_ROL( u08 Rd ); //
    void execute_ROR( u08 Rd ); //
    void execute_SBC( u08 Rd, u08 Rr ); //
    void execute_SBCI( u08 Rd, u08 K ); //
    void execute_SBI( u08 A, u08 b ); //
    void execute_SBIC( u08 A, u08 b ); //
    void execute_SBIS( u08 A, u08 b ); //
    void execute_SBIW( u08 Rd, u08 K ); //
    void execute_SBR( u08 Rd, u08 K ); //
    void execute_SBRC( u08 Rr, u08 b ); //
    void execute_SBRS( u08 Rr, u08 b ); //
    void execute_SEC(  ); //
    void execute_SEH(  ); //
    void execute_SEI(  ); //
    void execute_SEN(  ); //
    void execute_SER( u08 Rd ); //
    void execute_SES(  ); //
    void execute_SET(  ); //
    void execute_SEV(  ); //
    void execute_SEZ(  ); //
    void execute_SLEEP(  );
    void execute_SPM(  );
    void execute_STX1( u08 Rr ); //
    void execute_STX2( u08 Rr ); //
    void execute_STX3( u08 Rr ); //
    void execute_STY1( u08 Rr ); //
    void execute_STY2( u08 Rr ); //
    void execute_STY3( u08 Rr ); //
    void execute_STY4( u08 Rr, u08 q ); //
    void execute_STZ1( u08 Rr ); //
    void execute_STZ2( u08 Rr ); //
    void execute_STZ3( u08 Rr ); //
    void execute_STZ4( u08 Rr, u08 q ); //
    void execute_STS( u16 k, u08 Rd ); //
    void execute_SUB( u08 Rd, u08 Rr ); //
    void execute_SUBI( u08 Rd, u08 K ); //
    void execute_SWAP( u08 Rd ); //
    void execute_TST( u08 Rd ); //
    void execute_WDR(  );

    void execute( int location=-1 );

    /**
     * Sets or resets a bitmask in the variable in according to the value of val.
     * @param in The variable to be set.
     * @param val The boolean value that decides if the bits of the bitmask
     * are set or cleared.
     * @param bitmask The bitmask to be applied to in.
     */
    void setBit( unsigned char& in, bool val, unsigned int bitmask ) const
    { if (val) in |= bitmask; else in &= ~bitmask; }

    /**
     * Sets the program counter (PC) of the CPU.
     * Attention: The PC counts the bytes, not the words.
     * See the datasheets for details.
     * @param theValue The new value for the program counter.
     */
    void setProgramCounter(unsigned int theValue)
    { m_programCounter = theValue; }
    /**
     * @return Returns the program counter (PC). The PC
     * counts the bytes, not words or instructions.
     */
    unsigned int programCounter() const
    { return m_programCounter; }

    /**
     * @return Returns if the CPU is in an endless loop.
     * This is used to regulate the instruction rate.
     * If we didn't regulate it, KontrollerLab might consume
     * 100% CPU if we were in an endless loop, e.g., at the
     * end of a program when the CPU enters the final
     * \code{while(1);} loop.
     */
    bool endlessLoop() const
    { return m_endlessLoop; }

    /**
     * The call counter is used to know the current depth
     * of the program. If a subroutine is called, this counter
     * is increased by 1. If we return from a subroutine, this
     * counter is decreased by 1. For example, this counter
     * is used for the command "step over" in the debugger.
     * The call counter can be overwritten with this routine.
     * Handle with care!
     * @param theValue The new value for the call counter.
     */
    void setCallCounter(unsigned int theValue)
    { m_callCounter = theValue; }

    /**
     * See setCallCounter( unsigned int theValue ) for details
     * on the call counter.
     * @return The current value of the call counter is returned.
     */
    unsigned int callCounter() const
    { return m_callCounter; }

protected:
    /**
     * Helper function.
     * @param adr The flash address to be examined.
     * @return An unsigned integer containing the next 16 bits of flash.
     */
    unsigned int get16BitCommandWordStartingAt( unsigned int adr );
    /**
     * Helper function.
     * @param adr The flash address to be examined.
     * @return An unsigned integer containing the next 32 bits of flash.
     */
    unsigned int get32BitCommandWordStartingAt( unsigned int adr );
    /**
     * There are some instructions which consist of two words
     * (=four bytes). If there is a two-word instruction
     * stored at the address adr, this routine returns true,
     * otherwise false.
     * @param adr The address in the program flash to be examined.
     * @return True if there is a two-word instruction at the
     * address adr, otherwise false.
     */
    bool isTwoWordInstruction( unsigned int adr );
    unsigned int m_callCounter;

    /**
     * This stores the state of the processor.
     * The possibilities are
     * - STATE_READ_DATA The processor tells the debugger which RAM locations to be read in.
     * - STATE_EXECUTE The processor executes the command.
     * - STATE_WRITE_BACK The processor writes back the data.
     */
    CPU_STATE m_state;
    bool m_endlessLoop;
    unsigned int m_programCounter;
    // A pointer to the parent debugger:
    KLDebugger* m_parent;
    QString m_name;
};

#endif
