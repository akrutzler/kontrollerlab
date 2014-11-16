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
#include "klavrcpu.h"
#include "kldebugger.h"
#include "klavrflash.h"


KLAVRCPU::KLAVRCPU( KLDebugger* parent, const QString& name )
{
    m_parent = parent;
    m_name = name;
    m_state = STATE_READ_DATA;
    m_endlessLoop = false;
    m_callCounter = 0;
}


KLAVRCPU::~KLAVRCPU()
{
}


/**
 * \fn KLAVRCPU::execute_ADC( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0001 11rd dddd rrrr
 *
 * Adds two registers and the contents of the C Flag and places the result in 
 * the destination register Rd. 
 */
void KLAVRCPU::execute_ADC( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned char result = rd + rr + (sreg & SREG_C ? 1 : 0);
        setBit( sreg, ((rd&8) && (rr&8)) ||
                      ((rr&8) && (!(result&8))) ||
                      ((!(result&8)) && (rd&8)), SREG_H );
        setBit( sreg, (rd&0x80) && (rr&0x80) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (!(rr&0x80)) && (result&0x80), SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, (rd&0x80) && (rr&0x80) ||
                      (rr&0x80) && (!(result&0x80)) ||
                      (!(result&0x80)) && (rd&0x80), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_ADD( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 11rd dddd rrrr
 *
 * Adds two registers without the C Flag and places the result in the 
 * destination register Rd. 
 */
void KLAVRCPU::execute_ADD( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned char result = rd + rr;
        setBit( sreg, ((rd&8) && (rr&8)) ||
                ((rr&8) && (!(result&8))) ||
                        ((!(result&8)) && (rd&8)), SREG_H );
        setBit( sreg, (rd&0x80) && (rr&0x80) && (!(result&0x80)) ||
                (!(rd&0x80)) && (!(rr&0x80)) && (result&0x80), SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, (rd&0x80) && (rr&0x80) ||
                (rr&0x80) && (!(result&0x80)) ||
                        (!(result&0x80)) && (rd&0x80), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_ADIW( u08 Rd, u16 K )
 *
 * Bitmask of this operation:
 * 1001 0110 KKdd KKKK
 *
 * Adds an immediate value (0 - 63) to a register pair and places the result 
 * in the register pair. This instruction operates on the upper four 
 * register pairs, and is well suited for operations on the pointer registers. This 
 * instruction is not available in all devices. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_ADIW( u08 Rd, u16 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rd+1, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rdL = m_parent->readFromPCRam( Rd );
        unsigned char rdH = m_parent->readFromPCRam( Rd+1 );

        unsigned int result = rdH*256 + rdL + K;
        setBit( sreg, (!(rdH&0x80)) && (result&0x8000), SREG_V );
        setBit( sreg, result&0x8000, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, (!(result&0x8000)) && (rdH&0x80), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        rdL = result & 0xff;
        rdH = (result >> 8) & 0xff;
        m_parent->writeMemoryCell( Rd, rdL, Rd+1, rdH, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_AND( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0010 00rd dddd rrrr
 *
 * Performs the logical AND between the contents of register Rd and 
 * register Rr and places the result in the destination register Rd. 
 */
void KLAVRCPU::execute_AND( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned char result = rd & rr;
        setBit( sreg, false, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_ANDI( u08 Rd, u16 K )
 *
 * Bitmask of this operation:
 * 0111 KKKK dddd KKKK
 *
 * Performs the logical AND between the contents of register Rd and a 
 * constant and places the result in the destination register Rd. 
 */
void KLAVRCPU::execute_ANDI( u08 Rd, u16 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd & K;
        setBit( sreg, false, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_ASR( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0101
 *
 * Shifts all bits in Rd one place to the right. Bit 7 is held constant. Bit 0 
 * is loaded into the C Flag of the SREG. This operation effectively divides a 
 * signed value by two without changing its sign. The Carry Flag can be used to 
 * round the result. 
 */
void KLAVRCPU::execute_ASR( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd >> 1;
        result &= 0x7F;
        result |= rd & 0x80;
        setBit( sreg, rd & 0x1, SREG_C );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_C)), SREG_V );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_BCLR( u08 s )
 *
 * Bitmask of this operation:
 * 1001 0100 1sss 1000
 *
 * Clears a single Flag in SREG. 
 */
void KLAVRCPU::execute_BCLR( u08 s )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        setBit( sreg, false, 1 << s );
        m_parent->writeMemoryCell( m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_BLD( u08 Rd, u08 b )
 *
 * Bitmask of this operation:
 * 1111 100d dddd 0bbb
 *
 * Copies the T Flag in the SREG (Status Register) to bit b in register Rd. 
 */
void KLAVRCPU::execute_BLD( u08 Rd, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd;
        setBit(result, sreg & SREG_T, 1 << b);
        m_parent->writeMemoryCell( Rd, result );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_BRBC( u08 s, u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk ksss
 *
 * Conditional relative branch. Tests a single bit in SREG and branches 
 * relatively to PC if the bit is cleared. This instruction branches relatively to PC 
 * in either direction (PC - 63 <= destination <= PC + 64). The parameter k is 
 * the offset from PC and is represented in two s complement form. 
 */
void KLAVRCPU::execute_BRBC( u08 s, u08 k )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        m_state = STATE_CMD_FINISHED;
        int k_ = k;
        if (k_>63) k_=k_-128;
        // Check if branch needed:
        if ((sreg & (1<<s)))
            k_ = 0;
        m_programCounter += k_*2 + 2;
    }
}


/**
 * \fn KLAVRCPU::execute_BRBS( u08 s, u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk ksss
 *
 * Conditional relative branch. Tests a single bit in SREG and branches 
 * relatively to PC if the bit is set. This instruction branches relatively to PC in 
 * either direction (PC - 63 <= destination <= PC + 64). The parameter k is the 
 * offset from PC and is represented in two s complement form. 
 */
void KLAVRCPU::execute_BRBS( u08 s, u08 k )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        m_state = STATE_CMD_FINISHED;
        int k_ = k;
        if (k_>63) k_=k_-128;
        // Check if branch needed:
        if (!(sreg & (1<<s)))
            k_ = 0;
        // if ( m_programCounter == 0x1b2 )
        //     qDebug("H");
        m_programCounter += k_*2 + 2;
    }
}


/**
 * \fn KLAVRCPU::execute_BRCC( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k000
 *
 * Conditional relative branch. Tests the Carry Flag (C) and branches 
 * relatively to PC if C is cleared. This instruction branches relatively to PC in 
 * either direction (PC - 63 <= destination <= PC + 64). The parameter k is the 
 * offset from PC and is represented in two s complement form. (Equivalent to 
 * instruction BRBC 0,k). 
 */
void KLAVRCPU::execute_BRCC( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRCS( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k000
 *
 * Conditional relative branch. Tests the Carry Flag (C) and branches 
 * relatively to PC if C is set. This instruction branches relatively to PC in either 
 * direction (PC - 63 <= destination <= PC + 64). The parameter k is the offset from PC 
 * and is represented in two s complement form. (Equivalent to instruction 
 * BRBS 0,k). 
 */
void KLAVRCPU::execute_BRCS( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BREAK(  )
 *
 * Bitmask of this operation:
 * 1001 0101 1001 1000
 *
 * The BREAK instruction is used by the On-chip Debug system, and is 
 * normally not used in the application software. When the BREAK instruction is 
 * executed, the AVR CPU is set in the Stopped Mode. This gives the On-chip Debugger 
 * access to internal resources. If any Lock bits are set, or either the JTAGEN or 
 * OCDEN Fuses are unprogrammed, the CPU will treat the BREAK instruction as a 
 * NOP and will not enter the Stopped mode. This instruction is not available 
 * in all devices. Refer to the device specific instruction set summary. 
 */
void KLAVRCPU::execute_BREAK(  )
{
    /// @TODO QA Check this function!
    qWarning("FOUND A BREAK COMMAND: IGNORED %s:%d", __FILE__, __LINE__);
    m_state = STATE_CMD_FINISHED;
    m_programCounter += 2;
}


/**
 * \fn KLAVRCPU::execute_BREQ( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k001
 *
 * Conditional relative branch. Tests the Zero Flag (Z) and branches 
 * relatively to PC if Z is set. If the instruction is executed immediately after any of 
 * the instructions CP, CPI, SUB or SUBI, the branch will occur if and only if 
 * the unsigned or signed binary number represented in Rd was equal to the 
 * unsigned or signed binary number represented in Rr. This instruction branches 
 * relatively to PC in either direction (PC - 63 <= destination <= PC + 64). The 
 * parameter k is the offset from PC and is represented in two s complement form. 
 * (Equivalent to instruction BRBS 1,k). 
 */
void KLAVRCPU::execute_BREQ( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRGE( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k100
 *
 * Conditional relative branch. Tests the Signed Flag (S) and branches 
 * relatively to PC if S is cleared. If the instruction is executed immediately after 
 * any of the instructions CP, CPI, SUB or SUBI, the branch will occur if and 
 * only if the signed binary number represented in Rd was greater than or equal 
 * to the signed binary number represented in Rr. This instruction branches 
 * relatively to PC in either direction (PC - 63 <= destination <= PC + 64). The 
 * parameter k is the offset from PC and is represented in two s complement form. 
 * (Equivalent to instruction BRBC 4,k). 
 */
void KLAVRCPU::execute_BRGE( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRHC( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k101
 *
 * Conditional relative branch. Tests the Half Carry Flag (H) and 
 * branches relatively to PC if H is cleared. This instruction branches relatively 
 * to PC in either direction (PC - 63 <= destination <= PC + 64). The parameter k 
 * is the offset from PC and is represented in two s complement form. 
 * (Equivalent to instruction BRBC 5,k). 
 */
void KLAVRCPU::execute_BRHC( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRHS( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k101
 *
 * Conditional relative branch. Tests the Half Carry Flag (H) and 
 * branches relatively to PC if H is set. This instruction branches relatively to PC 
 * in either direction (PC - 63 <= destination <= PC + 64). The parameter k is 
 * the offset from PC and is represented in two s complement form. (Equivalent 
 * to instruction BRBS 5,k). 
 */
void KLAVRCPU::execute_BRHS( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRID( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k111
 *
 * Conditional relative branch. Tests the Global Interrupt Flag (I) and 
 * branches relatively to PC if I is cleared. This instruction branches relatively 
 * to PC in either direction (PC - 63 <= destination <= PC + 64). The parameter k 
 * is the offset from PC and is represented in two s complement form. 
 * (Equivalent to instruction BRBC 7,k). 
 */
void KLAVRCPU::execute_BRID( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRIE( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k111
 *
 * Conditional relative branch. Tests the Global Interrupt Flag (I) and 
 * branches relatively to PC if I is set. This instruction branches relatively to PC 
 * in either direction (PC - 63 <= destination <= PC + 64). The parameter k is 
 * the offset from PC and is represented in two s complement form. (Equivalent 
 * to instruction BRBS 7,k). 
 */
void KLAVRCPU::execute_BRIE( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRLO( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k000
 *
 * Conditional relative branch. Tests the Carry Flag (C) and branches 
 * relatively to PC if C is set. If the instruction is executed immediately after any of 
 * the instructions CP, CPI, SUB or SUBI, the branch will occur if and only if 
 * the unsigned binary number represented in Rd was smaller than the unsigned 
 * binary number represented in Rr. This instruction branches relatively to PC 
 * in either direction (PC - 63 <= destination <= PC + 64). The parameter k is 
 * the offset from PC and is represented in two s complement form. (Equivalent 
 * to instruction BRBS 0,k). 
 */
void KLAVRCPU::execute_BRLO( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRLT( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k100
 *
 * Conditional relative branch. Tests the Signed Flag (S) and branches 
 * relatively to PC if S is set. If the instruction is executed immediately after any of 
 * the instructions CP, CPI, SUB or SUBI, the branch will occur if and only if 
 * the signed binary number represented in Rd was less than the signed binary 
 * number represented in Rr. This instruction branches relatively to PC in 
 * either direction (PC - 63 <= destination <= PC + 64). The parameter k is the 
 * offset from PC and is represented in two s complement form. (Equivalent to 
 * instruction BRBS 4,k). 
 */
void KLAVRCPU::execute_BRLT( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRMI( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k010
 *
 * Conditional relative branch. Tests the Negative Flag (N) and branches 
 * relatively to PC if N is set. This instruction branches relatively to PC in either 
 * direction (PC - 63 <= destination <= PC + 64). The parameter k is the offset from PC 
 * and is represented in two s complement form. (Equivalent to instruction 
 * BRBS 2,k). 
 */
void KLAVRCPU::execute_BRMI( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRNE( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k001
 *
 * Conditional relative branch. Tests the Zero Flag (Z) and branches 
 * relatively to PC if Z is cleared. If the instruction is executed immediately after 
 * any of the instructions CP, CPI, SUB or SUBI, the branch will occur if and 
 * only if the unsigned or signed binary number represented in Rd was not equal 
 * to the unsigned or signed binary number represented in Rr. This 
 * instruction branches relatively to PC in either direction (PC - 63 <= destination <= 
 * PC + 64). The parameter k is the offset from PC and is represented in two s 
 * complement form. (Equivalent to instruction BRBC 1,k). 
 */
void KLAVRCPU::execute_BRNE( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRPL( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k010
 *
 * Conditional relative branch. Tests the Negative Flag (N) and branches 
 * relatively to PC if N is cleared. This instruction branches relatively to PC in 
 * either direction (PC - 63 <= destination <= PC + 64). The parameter k is the 
 * offset from PC and is represented in two s complement form. (Equivalent to 
 * instruction BRBC 2,k). 
 */
void KLAVRCPU::execute_BRPL( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRSH( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k000
 *
 * Conditional relative branch. Tests the Carry Flag (C) and branches 
 * relatively to PC if C is cleared. If the instruction is executed immediately after 
 * execution of any of the instructions CP, CPI, SUB or SUBI the branch will occur if 
 * and only if the unsigned binary number represented in Rd was greater than or 
 * equal to the unsigned binary number represented in Rr. This instruction 
 * branches relatively to PC in either direction (PC - 63 <= destination <= PC + 64). 
 * The parameter k is the offset from PC and is represented in two s complement 
 * form. (Equivalent to instruction BRBC 0,k). 
 */
void KLAVRCPU::execute_BRSH( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRTC( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k110
 *
 * Conditional relative branch. Tests the T Flag and branches relatively 
 * to PC if T is cleared. This instruction branches relatively to PC in either 
 * direction (PC - 63 <= destination <= PC + 64). The parameter k is the offset from PC 
 * and is represented in two s complement form. (Equivalent to instruction 
 * BRBC 6,k). 
 */
void KLAVRCPU::execute_BRTC( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRTS( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k110
 *
 * Conditional relative branch. Tests the T Flag and branches relatively 
 * to PC if T is set. This instruction branches relatively to PC in either 
 * direction (PC - 63 <= destination <= PC + 64). The parameter k is the offset from PC 
 * and is represented in two s complement form. (Equivalent to instruction 
 * BRBS 6,k). 
 */
void KLAVRCPU::execute_BRTS( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRVC( u08 k )
 *
 * Bitmask of this operation:
 * 1111 01kk kkkk k011
 *
 * Conditional relative branch. Tests the Overflow Flag (V) and branches 
 * relatively to PC if V is cleared. This instruction branches relatively to PC in 
 * either direction (PC - 63 <= destination <= PC + 64). The parameter k is the 
 * offset from PC and is represented in two s complement form. (Equivalent to 
 * instruction BRBC 3,k). 
 */
void KLAVRCPU::execute_BRVC( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BRVS( u08 k )
 *
 * Bitmask of this operation:
 * 1111 00kk kkkk k011
 *
 * Conditional relative branch. Tests the Overflow Flag (V) and branches 
 * relatively to PC if V is set. This instruction branches relatively to PC in either 
 * direction (PC - 63 <= destination <= PC + 64). The parameter k is the offset from PC 
 * and is represented in two s complement form. (Equivalent to instruction 
 * BRBS 3,k). 
 */
void KLAVRCPU::execute_BRVS( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BRBC OR BRBS! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_BSET( u08 s )
 *
 * Bitmask of this operation:
 * 1001 0100 0sss 1000
 *
 * Sets a single Flag or bit in SREG. 
 */
void KLAVRCPU::execute_BSET( u08 s )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        setBit( sreg, true, 1 << s );
        m_parent->writeMemoryCell( m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_BST( u08 Rd, u08 b )
 *
 * Bitmask of this operation:
 * 1111 101d dddd 0bbb
 *
 * Stores bit b from Rd to the T Flag in SREG (Status Register). 
 */
void KLAVRCPU::execute_BST( u08 Rd, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        setBit(sreg, rd & (1<<b), SREG_T);
        m_parent->writeMemoryCell( m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CALL( u08 k )
 *
 * Bitmask of this operation:
 * 1001 010k kkkk 111k kkkk kkkk kkkk kkkk
 *
 * Calls to a subroutine within the entire Program memory. The return 
 * address (to the instruction after the CALL) will be stored onto the Stack. (See 
 * also RCALL). The Stack Pointer uses a post-decrement scheme during CALL. 
 * This instruction is not available in all devices. Refer to the device 
 * specific instruction set summary. 
 */
void KLAVRCPU::execute_CALL( u32 k )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA;
        m_callCounter++;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;
        
        /// @TODO: Check this - 2 for more than 128KB flash!
        m_parent->writeMemoryCell( sp, ((m_programCounter+4) >> 8) & 0xff,
                                   sp-1, (m_programCounter+4) & 0xff,
                                   m_parent->spl(), (sp-2) & 0xff,
                                   m_parent->sph(), ((sp-2)>>8) & 0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter = k*2;
    }
}


/**
 * \fn KLAVRCPU::execute_CBI( u08 A, u08 b )
 *
 * Bitmask of this operation:
 * 1001 1000 AAAA Abbb
 *
 * Clears a specified bit in an I/O Register. This instruction operates on 
 * the lower 32 I/O Registers - addresses 0-31. 
 */
void KLAVRCPU::execute_CBI( u08 A, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( A, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char reg = m_parent->readFromPCRam( A );
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        setBit( reg, false, 1 << b );
        m_parent->writeMemoryCell( A, reg, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CBR( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 0111 KKKK dddd KKKK
 *
 * Clears the specified bits in register Rd. Performs the logical AND 
 * between the contents of register Rd and the complement of the constant mask K. 
 * The result will be placed in register Rd. 
 */
void KLAVRCPU::execute_CBR( u08, u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY ANDI! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLC(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1000 1000
 *
 * Clears the Carry Flag (C) in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLC( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLH(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1101 1000
 *
 * Clears the Half Carry Flag (H) in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLH( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLI(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1111 1000
 *
 * Clears the Global Interrupt Flag (I) in SREG (Status Register). The 
 * interrupts will be immediately disabled. No interrupt will be executed after the 
 * CLI instruction, even if it occurs simultaneously with the CLI 
 * instruction. 
 */
void KLAVRCPU::execute_CLI( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLN(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1010 1000
 *
 * Clears the Negative Flag (N) in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLN( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLR( u08 Rd )
 *
 * Bitmask of this operation:
 * 0010 01dd dddd dddd
 *
 * Clears a register. This instruction performs an Exclusive OR between a 
 * register and itself. This will clear all bits in the register. 
 */
void KLAVRCPU::execute_CLR( u08 Rd )
{
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        sreg &= ~( SREG_S | SREG_V | SREG_N );
        sreg |= SREG_Z;
        m_parent->writeMemoryCell( Rd, 0, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CLS(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1100 1000
 *
 * Clears the Signed Flag (S) in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLS( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLT(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1110 1000
 *
 * Clears the T Flag in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLT( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLV(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1011 1000
 *
 * Clears the Overflow Flag (V) in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLV( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_CLZ(  )
 *
 * Bitmask of this operation:
 * 1001 0100 1001 1000
 *
 * Clears the Zero Flag (Z) in SREG (Status Register). 
 */
void KLAVRCPU::execute_CLZ( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_COM( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0000
 *
 * This instruction performs a One s Complement of register Rd. 
 */
void KLAVRCPU::execute_COM( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = 0xff - rd;
        setBit( sreg, false, SREG_V );
        setBit( sreg, true, SREG_C );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CP( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0001 01rd dddd rrrr
 *
 * This instruction performs a compare between two registers Rd and Rr. 
 * None of the registers are changed. All conditional branches can be used 
 * after this instruction. 
 */
void KLAVRCPU::execute_CP( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        // qDebug("CP:  %d == %d", rd, rr);
        unsigned char result = rd - rr;
        setBit( sreg, (result == 0), SREG_Z );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, ((!(rd&8)) && (rr&8)) ||
                ((rr&8) && (result&8)) ||
                        ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(rr&0x80)) && (!(result&0x80)) ||
                (!(rd&0x80)) && (rr&0x80) && (result&0x80), SREG_V );
        setBit( sreg, ((!(rd&0x80)) && (rr&0x80)) ||
                ((rr&0x80) && (result&0x80)) ||
                        ((result&0x80) && (!(rd&0x80))), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CPC( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 01rd dddd rrrr
 *
 * This instruction performs a compare between two registers Rd and Rr and 
 * also takes into account the previous carry. None of the registers are 
 * changed. All conditional branches can be used after this instruction. 
 */
void KLAVRCPU::execute_CPC( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        // if ( !( sreg & SREG_C ) )
        //     qDebug("Carry");
        // qDebug("CPC: %d == %d", rd, rr);
        unsigned char result = rd - rr;
        result -= (unsigned char) (sreg & SREG_C ? 1 : 0);
        setBit( sreg, (result == 0) && (sreg & SREG_Z), SREG_Z );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, ((!(rd&8)) && (rr&8)) ||
                      ((rr&8) && (result&8)) ||
                      ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(rr&0x80)) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (rr&0x80) && (result&0x80), SREG_V );
        setBit( sreg, ((!(rd&0x80)) && (rr&0x80)) ||
                      ((rr&0x80) && (result&0x80)) ||
                      ((result&0x80) && (!(rd&0x80))), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CPI( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 0011 KKKK dddd KKKK
 *
 * This instruction performs a compare between register Rd and a 
 * constant. The register is not changed. All conditional branches can be used after 
 * this instruction. 
 */
void KLAVRCPU::execute_CPI( u08 Rd, u08 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd - K;
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, ((!(rd&8)) && (K&8)) ||
                      ((K&8) && (result&8)) ||
                      ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(K&0x80)) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (K&0x80) && (result&0x80), SREG_V );
        setBit( sreg, ((!(rd&0x80)) && (K&0x80)) ||
                      ((K&0x80) && (result&0x80)) ||
                      ((result&0x80) && (!(rd&0x80))), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_CPSE( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0001 00rd dddd rrrr
 *
 * This instruction performs a compare between two registers Rd and Rr, 
 * and skips the next instruction if Rd = Rr. 
 */
void KLAVRCPU::execute_CPSE( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        if ( rd == rr )
        {
            if ( isTwoWordInstruction( m_programCounter+2 ) )
                m_programCounter += 6;
            else
                m_programCounter += 4;
        }
        else
            m_programCounter+=2;
        m_state = STATE_CMD_FINISHED;
    }
}


/**
 * \fn KLAVRCPU::execute_DEC( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 1010
 *
 * Subtracts one -1- from the contents of register Rd and places the result 
 * in the destination register Rd. The C Flag in SREG is not affected by the 
 * operation, thus allowing the DEC instruction to be used on a loop counter in 
 * multiple- precision computations. When operating on unsigned values, only BREQ 
 * and BRNE branches can be expected to perform consistently. When operating 
 * on two s complement values, all signed branches are available. 
 */
void KLAVRCPU::execute_DEC( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd - 1;
        setBit( sreg, result == 0x7f, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_EICALL(  )
 *
 * Bitmask of this operation:
 * 1001 0101 0001 1001
 *
 * Indirect call of a subroutine pointed to by the Z (16 bits) Pointer 
 * Register in the Register File and the EIND Register in the I/O space. This 
 * instruction allows for indirect calls to the entire Program memory space. The Stack 
 * Pointer uses a post-decrement scheme during EICALL. This instruction is not 
 * implemented for devices with 2 bytes PC, see ICALL. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_EICALL(  )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
    m_callCounter++;
}


/**
 * \fn KLAVRCPU::execute_EIJMP(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0001 1001
 *
 * Indirect jump to the address pointed to by the Z (16 bits) Pointer 
 * Register in the Register File and the EIND Register in the I/O space. This 
 * instruction allows for indirect jumps to the entire Program memory space. This 
 * instruction is not available in all devices. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_EIJMP(  )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_ELPM1(  )
 *
 * Bitmask of this operation:
 * 1001 0101 1101 1000
 *
 * Loads one byte pointed to by the Z-register and the RAMPZ Register in the 
 * I/O space, and places this byte in the destination register Rd. This 
 * instruction features a 100% space effective constant initialization or constant 
 * data fetch. The Program memory is organized in 16-bit words while the 
 * Z-pointer is a byte address. Thus, the least significant bit of the Z-pointer 
 * selects either low byte (ZLSB = 0) or high byte (ZLSB = 1). This instruction can 
 * address the entire Program memory space. The Z-pointer Register can either be 
 * left unchanged by the operation, or it can be incremented. The 
 * incrementation applies to the entire 24-bit concatenation of the RAMPZ and Z-pointer 
 * Registers. Devices with Self-Programming capability can use the ELPM 
 * instruction to read the Fuse and Lock bit value. Refer to the device documentation 
 * for a detailed description. This instruction is not available in all 
 * devices. Refer to the device specific instruction set summary. 
 */
void KLAVRCPU::execute_ELPM1(  )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_ELPM2( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0110
 *
 *  
 */
void KLAVRCPU::execute_ELPM2( u08 Rd )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_ELPM3( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0111
 *
 *  
 */
void KLAVRCPU::execute_ELPM3( u08 Rd )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_EOR( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0010 01rd dddd rrrr
 *
 * Performs the logical EOR between the contents of register Rd and 
 * register Rr and places the result in the destination register Rd. 
 */
void KLAVRCPU::execute_EOR( u08 Rd, u08 Rr )
{
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char result = m_parent->readFromPCRam( Rd ) ^
                               m_parent->readFromPCRam( Rr );
        sreg &= ~( SREG_V );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, result & 0x80, SREG_N );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_FMUL( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 0011 0ddd 1rrr
 *
 * This instruction performs 8-bit × 8-bit -> 16-bit unsigned 
 * multiplication and shifts the result one bit left. 
 */
void KLAVRCPU::execute_FMUL( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_FMULS( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 0011 1ddd 0rrr
 *
 * This instruction performs 8-bit × 8-bit -> 16-bit signed 
 * multiplication and shifts the result one bit left. 
 */
void KLAVRCPU::execute_FMULS( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_FMULSU( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 0011 1ddd 1rrr
 *
 * This instruction performs 8-bit × 8-bit -> 16-bit signed 
 * multiplication and shifts the result one bit left. 
 */
void KLAVRCPU::execute_FMULSU( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_ICALL(  )
 *
 * Bitmask of this operation:
 * 1001 0101 0000 1001
 *
 * Indirect call of a subroutine pointed to by the Z (16 bits) Pointer 
 * Register in the Register File. The Z-pointer Register is 16 bits wide and allows 
 * call to a subroutine within the lowest 64K words (128K bytes) section in the 
 * Program memory space. The Stack Pointer uses a post-decrement scheme during 
 * ICALL. This instruction is not available in all devices. Refer to the device 
 * specific instruction set summary. 
 */
void KLAVRCPU::execute_ICALL(  )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->zh(), m_parent->zl(), m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA;
        m_callCounter++;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char zl = m_parent->readFromPCRam( m_parent->zl() );
        unsigned char zh = m_parent->readFromPCRam( m_parent->zh() );
        unsigned int z = zl + zh*256;
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;
        
        /// @TODO: Check this - 2 for more than 128KB flash!
        unsigned int spAfter = sp - 2;
        
        m_parent->writeMemoryCell( sp-1, (m_programCounter+2) & 0xff,
                                   sp,  ((m_programCounter+2) & 0xff00) >> 8,
                                   m_parent->spl(), spAfter & 0xff,
                                   m_parent->sph(), (spAfter & 0xff00) >> 8 );
        
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        
        m_programCounter = z*2;
    }
}


/**
 * \fn KLAVRCPU::execute_IJMP(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0000 1001
 *
 * Indirect jump to the address pointed to by the Z (16 bits) Pointer 
 * Register in the Register File. The Z-pointer Register is 16 bits wide and allows 
 * jump within the lowest 64K words (128K bytes) section of Program memory. 
 * This instruction is not available in all devices. Refer to the device 
 * specific instruction set summary. 
 */
void KLAVRCPU::execute_IJMP(  )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->zh(), m_parent->zl() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char zl = m_parent->readFromPCRam( m_parent->zl() );
        unsigned char zh = m_parent->readFromPCRam( m_parent->zh() );
        unsigned int z = zl + zh*256;
        
        /// @TODO: Check this - 2 for more than 128KB flash!
        
        m_state = STATE_CMD_FINISHED;
        m_programCounter = z*2;
    }
}


/**
 * \fn KLAVRCPU::execute_IN( u08 Rd, u08 A )
 *
 * Bitmask of this operation:
 * 1011 0AAd dddd AAAA
 *
 * Loads data from the I/O Space (Ports, Timers, Configuration Registers 
 * etc.) into register Rd in the Register File. 
 */
void KLAVRCPU::execute_IN( u08 Rd, u08 A )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( A );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( A ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_INC( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0011
 *
 * Adds one -1- to the contents of register Rd and places the result in the 
 * destination register Rd. The C Flag in SREG is not affected by the operation, thus 
 * allowing the INC instruction to be used on a loop counter in multiple- precision 
 * computations. When operating on unsigned numbers, only BREQ and BRNE branches can be 
 * expected to perform consistently. When operating on two s complement values, 
 * all signed branches are available. 
 */
void KLAVRCPU::execute_INC( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd + 1;
        setBit( sreg, result == 0x80, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_JMP( u32 k )
 *
 * Bitmask of this operation:
 * 1001 010k kkkk 110k kkkk kkkk kkkk kkkk
 *
 * Jump to an address within the entire 4M (words) Program memory. See also 
 * RJMP. This instruction is not available in all devices. Refer to the device 
 * specific instruction set summary. 
 */
void KLAVRCPU::execute_JMP( u32 k )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        m_programCounter=k*2;
        m_state = STATE_CMD_FINISHED;
    }
}


/**
 * \fn KLAVRCPU::execute_LDX1( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 1100
 *
 * Loads one byte indirect from the data space to a register. For parts with 
 * SRAM, the data space consists of the Register File, I/O memory and internal 
 * SRAM (and external SRAM if applicable). For parts without SRAM, the data 
 * space consists of the Register File only. The EEPROM has a separate address 
 * space. The data location is pointed to by the X (16 bits) Pointer Register in the 
 * Register File. Memory access is limited to the current data segment of 64K bytes. 
 * To access another data segment in devices with more than 64K bytes data 
 * space, the RAMPX in register in the I/O area has to be changed. The X-pointer 
 * Register can either be left unchanged by the operation, or it can be 
 * post-incremented or pre-decremented. These features are especially suited for 
 * accessing arrays, tables, and Stack Pointer usage of the X-pointer Register. 
 * Note that only the low byte of the X-pointer is updated in devices with no more 
 * than 256 bytes data space. For such devices, the high byte of the pointer is 
 * not used by this instruction and can be used for other purposes. The RAMPX 
 * Register in the I/O area is updated in parts with more than 64K bytes data space or 
 * more than 64K bytes Program memory, and the increment/ decrement is added to 
 * the entire 24-bit address on such devices. Not all variants of this 
 * instruction is available in all devices. Refer to the device specific instruction 
 * set summary. 
 */
void KLAVRCPU::execute_LDX1( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->xl(), m_parent->xh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                m_parent->readFromPCRam( m_parent->xh() ) * 256;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                m_parent->readFromPCRam( m_parent->xh() ) * 256;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDX2( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 1101
 *
 *  
 */
void KLAVRCPU::execute_LDX2( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->xl(), m_parent->xh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                m_parent->readFromPCRam( m_parent->xh() ) * 256;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                m_parent->readFromPCRam( m_parent->xh() ) * 256;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ),
                                   m_parent->xh(), ((adr+1)&0xff00)>>8,
                                   m_parent->xl(), (adr+1)&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDX3( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 1110
 *
 *  
 */
void KLAVRCPU::execute_LDX3( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->xl(), m_parent->xh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                m_parent->readFromPCRam( m_parent->xh() ) * 256;
        adr--;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                m_parent->readFromPCRam( m_parent->xh() ) * 256;
        adr--;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ),
                                   m_parent->xh(), (adr&0xff00)>>8,
                                   m_parent->xl(), adr&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDY1( u08 Rd )
 *
 * Bitmask of this operation:
 * 1000 000d dddd 1000
 *
 * Loads one byte indirect with or without displacement from the data 
 * space to a register. For parts with SRAM, the data space consists of the 
 * Register File, I/O memory and internal SRAM (and external SRAM if applicable). 
 * For parts without SRAM, the data space consists of the Register File only. 
 * The EEPROM has a separate address space. The data location is pointed to by 
 * the Y (16 bits) Pointer Register in the Register File. Memory access is 
 * limited to the current data segment of 64K bytes. To access another data segment 
 * in devices with more than 64K bytes data space, the RAMPY in register in the 
 * I/O area has to be changed. The Y-pointer Register can either be left 
 * unchanged by the operation, or it can be post-incremented or pre-decremented. 
 * These features are especially suited for accessing arrays, tables, and 
 * Stack Pointer usage of the Y-pointer Register. Note that only the low byte of 
 * the Y-pointer is updated in devices with no more than 256 bytes data space. 
 * For such devices, the high byte of the pointer is not used by this 
 * instruction and can be used for other purposes. The RAMPY Register in the I/O area is 
 * updated in parts with more than 64K bytes data space or more than 64K bytes 
 * Program memory, and the increment/ decrement/displacement is added to the 
 * entire 24-bit address on such devices. Not all variants of this instruction is 
 * available in all devices. Refer to the device specific instruction set summary. 
 */
void KLAVRCPU::execute_LDY1( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDY2( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 1001
 *
 *  
 */
void KLAVRCPU::execute_LDY2( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ),
                                   m_parent->yh(), ((adr+1)&0xff00)>>8,
                                   m_parent->yl(), (adr+1)&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDY3( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 1010
 *
 *  
 */
void KLAVRCPU::execute_LDY3( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        adr--;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        adr--;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ),
                                   m_parent->yh(), (adr&0xff00)>>8,
                                   m_parent->yl(), adr&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDY4( u08 Rd, u08 q )
 *
 * Bitmask of this operation:
 * 10q0 qq0d dddd 1qqq
 *
 *  
 */
void KLAVRCPU::execute_LDY4( u08 Rd, u08 q )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                m_parent->readFromPCRam( m_parent->yh() ) * 256 + q;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                m_parent->readFromPCRam( m_parent->yh() ) * 256 + q;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDZ1( u08 Rd )
 *
 * Bitmask of this operation:
 * 1000 000d dddd 0000
 *
 * Loads one byte indirect with or without displacement from the data 
 * space to a register. For parts with SRAM, the data space consists of the 
 * Register File, I/O memory and internal SRAM (and external SRAM if applicable). 
 * For parts without SRAM, the data space consists of the Register File only. 
 * The EEPROM has a separate address space. The data location is pointed to by 
 * the Z (16 bits) Pointer Register in the Register File. Memory access is 
 * limited to the current data segment of 64K bytes. To access another data segment 
 * in devices with more than 64K bytes data space, the RAMPZ in register in the 
 * I/O area has to be changed. The Z-pointer Register can either be left 
 * unchanged by the operation, or it can be post-incremented or pre-decremented. 
 * These features are especially suited for Stack Pointer usage of the 
 * Z-pointer Register, however because the Z-pointer Register can be used for 
 * indirect subroutine calls, indirect jumps and table lookup, it is often more 
 * convenient to use the X or Y-pointer as a dedicated Stack Pointer. Note that only the 
 * low byte of the Z-pointer is updated in devices with no more than 256 bytes 
 * data space. For such devices, the high byte of the pointer is not used by this 
 * instruction and can be used for other purposes. The RAMPZ Register in the I/O area is 
 * updated in parts with more than 64K bytes data space or more than 64K bytes 
 * Program memory, and the increment/decrement/displacement is added to the 
 * entire 24-bit address on such devices. Not all variants of this instruction is 
 * available in all devices. Refer to the device specific instruction set summary. 
 * For using the Z-pointer for table lookup in Program memory see the LPM and 
 * ELPM instructions. 
 */
void KLAVRCPU::execute_LDZ1( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDZ2( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0001
 *
 *  
 */
void KLAVRCPU::execute_LDZ2( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ),
                                   m_parent->zh(), ((adr+1)&0xff00)>>8,
                                   m_parent->zl(), (adr+1)&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDZ3( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0010
 *
 *  
 */
void KLAVRCPU::execute_LDZ3( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        adr--;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        adr--;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ),
                                   m_parent->zh(), (adr&0xff00)>>8,
                                   m_parent->zl(), adr&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDZ4( u08 Rd, u08 q )
 *
 * Bitmask of this operation:
 * 10q0 qq0d dddd 0qqq
 *
 *  
 */
void KLAVRCPU::execute_LDZ4( u08 Rd, u08 q )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        // This ensures the RAM is valid:
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256 + q;
        m_parent->readMemoryCell( adr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256 + q;
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDI( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 1110 KKKK dddd KKKK
 *
 * Loads an 8 bit constant directly to register 16 to 31. 
 */
void KLAVRCPU::execute_LDI( u08 Rd, u08 K )
{
    if ( m_state == STATE_READ_DATA )
    {
        m_parent->writeMemoryCell( Rd, K );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LDS( u08 Rd, u08 k )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0000 kkkk kkkk kkkk kkkk
 *
 * Loads one byte from the data space to a register. For parts with SRAM, the 
 * data space consists of the Register File, I/O memory and internal SRAM (and 
 * external SRAM if applicable). For parts without SRAM, the data space consists of 
 * the register file only. The EEPROM has a separate address space. A 16-bit 
 * address must be supplied. Memory access is limited to the current data segment 
 * of 64K bytes. The LDS instruction uses the RAMPD Register to access memory 
 * above 64K bytes. To access another data segment in devices with more than 64K 
 * bytes data space, the RAMPD in register in the I/O area has to be changed. This 
 * instruction is not available in all devices. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_LDS( u08 Rd, u08 k )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( k );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( k ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=4;
    }
}


/**
 * \fn KLAVRCPU::execute_LPM1(  )
 *
 * Bitmask of this operation:
 * 1001 0101 1100 1000
 *
 * Loads one byte pointed to by the Z-register into the destination 
 * register Rd. This instruction features a 100% space effective constant 
 * initialization or constant data fetch. The Program memory is organized in 16-bit words 
 * while the Z-pointer is a byte address. Thus, the least significant bit of the 
 * Z-pointer selects either low byte (ZLSB = 0) or high byte (ZLSB = 1). This 
 * instruction can address the first 64K bytes (32K words) of Program memory. The 
 * Z-pointer Register can either be left unchanged by the operation, or it can be 
 * incremented. The incrementation does not apply to the RAMPZ Register. Devices with 
 * Self-Programming capability can use the LPM instruction to read the Fuse and Lock bit 
 * values. Refer to the device documentation for a detailed description. Not all 
 * variants of the LPM instruction are available in all devices. Refer to the device 
 * specific instruction set summary. The LPM instruction is not implemented at all 
 * in the AT90S1200 device. 
 */
void KLAVRCPU::execute_LPM1(  )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        // Read Z register
        m_parent->readMemoryCell( ZH, ZL );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( ZH ) * 256 +
                           m_parent->readFromPCRam( ZL );
        m_parent->writeMemoryCell( 0, m_parent->flash().read( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LPM2( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0100
 *
 *  
 */
void KLAVRCPU::execute_LPM2( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        // Read Z register
        m_parent->readMemoryCell( ZH, ZL );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( ZH ) * 256 +
                m_parent->readFromPCRam( ZL );
        m_parent->writeMemoryCell( Rd, m_parent->flash().read( adr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LPM3( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 0101
 *
 *  
 */
void KLAVRCPU::execute_LPM3( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        // Read Z register
        m_parent->readMemoryCell( ZH, ZL );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( ZH ) * 256 +
                m_parent->readFromPCRam( ZL );
        unsigned char zhAfter = ((adr+1)>>8) & 0xFF;
        unsigned char zlAfter = (adr+1) & 0xFF;
        m_parent->writeMemoryCell( Rd, m_parent->flash().read( adr ), ZH, zhAfter, ZL, zlAfter );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LSL( u08 Rd )
 *
 * Bitmask of this operation:
 * 0000 11dd dddd dddd
 *
 * Shifts all bits in Rd one place to the left. Bit 0 is cleared. Bit 7 is 
 * loaded into the C Flag of the SREG. This operation effectively multiplies 
 * signed and unsigned values by two. 
 */
void KLAVRCPU::execute_LSL( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = (rd << 1);
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, rd & 0x80, SREG_C );
        setBit( sreg, rd & 0x8, SREG_H );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_C)), SREG_V );
        setBit( sreg, (result == 0), SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_LSR( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0110
 *
 * Shifts all bits in Rd one place to the right. Bit 7 is cleared. Bit 0 is 
 * loaded into the C Flag of the SREG. This operation effectively divides an 
 * unsigned value by two. The C Flag can be used to round the result. 
 */
void KLAVRCPU::execute_LSR( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = (rd >> 1);
        setBit( sreg, false, SREG_N );
        setBit( sreg, rd & 1, SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_C)), SREG_V );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        setBit( sreg, (result == 0), SREG_Z );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_MOV( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0010 11rd dddd rrrr
 *
 * This instruction makes a copy of one register into another. The source 
 * register Rr is left unchanged, while the destination register Rd is loaded with a 
 * copy of Rr. 
 */
void KLAVRCPU::execute_MOV( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_MOVW( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 0001 dddd rrrr
 *
 * This instruction makes a copy of one register pair into another 
 * register pair. The source register pair Rr+1:Rr is left unchanged, while the 
 * destination register pair Rd+1:Rd is loaded with a copy of Rr + 1:Rr. This 
 * instruction is not available in all devices. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_MOVW( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, Rr+1 );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( Rr ),
                                   Rd+1, m_parent->readFromPCRam( Rr+1 ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_MUL( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 11rd dddd rrrr
 *
 * This instruction performs 8-bit × 8-bit -> 16-bit unsigned 
 * multiplication. This instruction performs 8-bit × 8-bit -> 16-bit unsigned 
 * multiplication. 
 */
void KLAVRCPU::execute_MUL( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned int result = ((unsigned int) rd) * ((unsigned int)rr);
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, result & 0x8000, SREG_C );
        m_parent->writeMemoryCell( 0, result & 0xff, 1, (result >> 8) & 0xff, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_MULS( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 0010 dddd rrrr
 *
 * This instruction performs 8-bit × 8-bit -> 16-bit signed 
 * multiplication. The multiplicand Rd and the multiplier Rr are two registers containing 
 * signed numbers. The 16-bit signed product is placed in R1 (high byte) and R0 
 * (low byte). This instruction is not available in all devices. Refer to the 
 * device specific instruction set summary. 
 */
void KLAVRCPU::execute_MULS( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        char rd = m_parent->readFromPCRam( Rd );
        char rr = m_parent->readFromPCRam( Rr );
        int result = ((int) rd) * ((int)rr);
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, result & 0x8000, SREG_C );
        m_parent->writeMemoryCell( 0, result & 0xff, 1, (result >> 8) & 0xff, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_MULSU( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 0011 0ddd 0rrr
 *
 * This instruction performs 8-bit × 8-bit -> 16-bit multiplication of a 
 * signed and an unsigned number. The multiplicand Rd and the multiplier Rr are 
 * two registers. The multiplicand Rd is a signed number, and the multiplier 
 * Rr is unsigned. The 16-bit signed product is placed in R1 (high byte) and R0 
 * (low byte). This instruction is not available in all devices. Refer to the 
 * device specific instruction set summary. 
 */
void KLAVRCPU::execute_MULSU( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        int result = ((int) rd) * ((unsigned int)rr);
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, result & 0x8000, SREG_C );
        m_parent->writeMemoryCell( 0, result & 0xff, 1, (result >> 8) & 0xff, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_NEG( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0001
 *
 * Replaces the contents of register Rd with its two s complement; the 
 * value $80 is left unchanged. 
 */
void KLAVRCPU::execute_NEG( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = 0x00 - rd;
        setBit( sreg, (result&0x8) || (rd&0x8), SREG_H );
        setBit( sreg, result == 0x80, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, result, SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_NOP(  )
 *
 * Bitmask of this operation:
 * 0000 0000 0000 0000
 *
 * This instruction performs a single cycle No Operation. 
 */
void KLAVRCPU::execute_NOP(  )
{
    /// @TODO QA Check this function!
    m_state = STATE_CMD_FINISHED;
    m_programCounter += 2;
}


/**
 * \fn KLAVRCPU::execute_OR( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0010 10rd dddd rrrr
 *
 * Performs the logical OR between the contents of register Rd and 
 * register Rr and places the result in the destination register Rd. 
 */
void KLAVRCPU::execute_OR( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned char result = rd | rr;
        setBit( sreg, false, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_ORI( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 0110 KKKK dddd KKKK
 *
 * Performs the logical OR between the contents of register Rd and a 
 * constant and places the result in the destination register Rd. 
 */
void KLAVRCPU::execute_ORI( u08 Rd, u08 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = rd | K;
        setBit( sreg, false, SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_OUT( u08 A, u08 Rr )
 *
 * Bitmask of this operation:
 * 1011 1AAr rrrr AAAA
 *
 * Stores data from register Rr in the Register File to I/O Space (Ports, 
 * Timers, Configuration Registers etc.). 
 */
void KLAVRCPU::execute_OUT( u08 A, u08 Rr )
{
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        m_parent->writeMemoryCell( A, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_POP( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 000d dddd 1111
 *
 * This instruction loads register Rd with a byte from the STACK. The Stack 
 * Pointer is pre-incremented by 1 before the POP. This instruction is not 
 * available in all devices. Refer to the device specific instruction set summary. 
 */
void KLAVRCPU::execute_POP( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;

        m_parent->readMemoryCell( sp+1 );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;

        m_parent->writeMemoryCell( Rd, m_parent->readFromPCRam( sp+1 ),
                                   m_parent->spl(), (sp+1) & 0xff,
                                   m_parent->sph(), ((sp+1)>>8) & 0xff );
        // qDebug("POP (%d) <- %d (%d), SP NOW: %d", Rd, m_parent->readFromPCRam( sp+1 ), sp+1, sp+1);
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter += 2;
    }
}


/**
 * \fn KLAVRCPU::execute_PUSH( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 001d dddd 1111
 *
 * This instruction stores the contents of register Rd on the STACK. The 
 * Stack Pointer is post-decremented by 1 after the PUSH. This instruction is 
 * not available in all devices. Refer to the device specific instruction set 
 * summary. 
 */
void KLAVRCPU::execute_PUSH( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;
        
        /// @TODO: Check this - 2 for more than 128KB flash!
        m_parent->writeMemoryCell( sp, m_parent->readFromPCRam( Rd ),
                                   m_parent->spl(), (sp-1) & 0xff,
                                   m_parent->sph(), ((sp-1)>>8) & 0xff );
        // qDebug("PUSH (%d) <- %d (%d), SP NOW: %d", sp, m_parent->readFromPCRam( Rd ), Rd, sp-1);
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter += 2;
    }
}


/**
 * \fn KLAVRCPU::execute_RCALL( u16 k )
 *
 * Bitmask of this operation:
 * 1101 kkkk kkkk kkkk
 *
 * Relative call to an address within PC - 2K + 1 and PC + 2K (words). The 
 * return address (the instruction after the RCALL) is stored onto the Stack. 
 * (See also CALL). In the assembler, labels are used instead of relative 
 * operands. For AVR microcontrollers with Program memory not exceeding 4K words 
 * (8K bytes) this instruction can address the entire memory from every 
 * address location. The Stack Pointer uses a post-decrement scheme during 
 * RCALL. 
 */
void KLAVRCPU::execute_RCALL( u16 k_ )
{
        /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA;
        m_callCounter++;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;
        
        /// @TODO: Check this - 2 for more than 128KB flash!
        m_parent->writeMemoryCell( sp, ((m_programCounter+2) >> 8) & 0xff,
                                   sp-1, (m_programCounter+2) & 0xff,
                                   m_parent->spl(), (sp-2) & 0xff,
                                   m_parent->sph(), ((sp-2)>>8) & 0xff );
        // qDebug("Wrote (%d) <- %d, (%d) <- %d, SP now: %d",
        //        sp, ((m_programCounter+2) >> 8) & 0xff,
        //        sp-1, (m_programCounter+2) & 0xff, sp-2);
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        int k = k_;
        if ( k > 2048 ) k = k - 4096;
        m_programCounter += ((int) k)*2+2;
    }
}


/**
 * \fn KLAVRCPU::execute_RET(  )
 *
 * Bitmask of this operation:
 * 1001 0101 0000 1000
 *
 * Returns from subroutine. The return address is loaded from the STACK. 
 * The Stack Pointer uses a pre-increment scheme during RET. 
 */
void KLAVRCPU::execute_RET(  )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;

        m_parent->readMemoryCell( sp+1, sp+2 );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;
        
        /// @TODO: Check this +2 for more than 128KB flash!
        m_programCounter = m_parent->readFromPCRam( sp+1 ) + m_parent->readFromPCRam( sp+2 )*256;
        m_parent->writeMemoryCell( m_parent->spl(), (sp+2) & 0xff,
                                   m_parent->sph(), ((sp+2)>>8) & 0xff );
        // qDebug("Read (%d) = %d, (%d) = %d, SP now: %d",
        //        sp+1, m_parent->readFromPCRam( sp+1 ),
        //        sp+2, m_parent->readFromPCRam( sp+2 ),
        //        sp+2);
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        if ( m_callCounter>0 )
            m_callCounter--;
    }
}


/**
 * \fn KLAVRCPU::execute_RETI(  )
 *
 * Bitmask of this operation:
 * 1001 0101 0001 1000
 *
 * Returns from interrupt. The return address is loaded from the STACK and 
 * the Global Interrupt Flag is set. Note that the Status Register is not 
 * automatically stored when entering an interrupt routine, and it is not restored when 
 * returning from an interrupt routine. This must be handled by the application 
 * program. The Stack Pointer uses a pre-increment scheme during RETI. 
 */
void KLAVRCPU::execute_RETI(  )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( m_parent->sph(), m_parent->spl() );
        m_state = STATE_WAIT_FOR_READ_DATA_INTERNAL;
    }
    else if ( m_state == STATE_READ_DATA_INTERNAL )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;

        m_parent->readMemoryCell( sp+1, sp+2, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
        unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
        unsigned int sp = spl + sph*256;
        
        /// @TODO: Check this +2 for more than 128KB flash!
        m_programCounter = m_parent->readFromPCRam( sp+1 ) + m_parent->readFromPCRam( sp+2 )*256;
        m_parent->writeMemoryCell( m_parent->spl(), (sp+2) & 0xff,
                                   m_parent->sph(), ((sp+2)>>8) & 0xff,
                                   m_parent->sreg(), m_parent->readFromPCRam( m_parent->sreg() ) | SREG_I );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        if ( m_callCounter>0 )
            m_callCounter--;
    }
}


/**
 * \fn KLAVRCPU::execute_RJMP( u16 k )
 *
 * Bitmask of this operation:
 * 1100 kkkk kkkk kkkk
 *
 * Relative jump to an address within PC - 2K +1 and PC + 2K (words). In the 
 * assembler, labels are used instead of relative operands. For AVR 
 * microcontrollers with Program memory not exceeding 4K words (8K bytes) this instruction 
 * can address the entire memory from every address location. 
 */
void KLAVRCPU::execute_RJMP( u16 k_ )
{
    m_state = STATE_CMD_FINISHED;
    int k = k_;
    if ( k > 2048 ) k = k - 4096;
    if ( k == -1 )
        m_endlessLoop=true;
    m_programCounter += ((int) k)*2+2;
    // qDebug("finished RJMP");
}


/**
 * \fn KLAVRCPU::execute_ROL( u08 Rd )
 *
 * Bitmask of this operation:
 * 0001 11dd dddd dddd
 *
 * Shifts all bits in Rd one place to the left. The C Flag is shifted into bit 0 
 * of Rd. Bit 7 is shifted into the C Flag. This operation, combined with LSL, 
 * effectively multiplies multi-byte signed and unsigned values by two. 
 */
void KLAVRCPU::execute_ROL( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = (rd << 1) | (sreg & SREG_C ? 1 : 0);
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, rd & 0x80, SREG_C );
        setBit( sreg, rd & 0x8, SREG_H );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_C)), SREG_V );
        setBit( sreg, (result == 0), SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_ROR( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0111
 *
 * Shifts all bits in Rd one place to the right. The C Flag is shifted into bit 
 * 7 of Rd. Bit 0 is shifted into the C Flag. This operation, combined with 
 * ASR, effectively divides multi-byte signed values by two. Combined with 
 * LSR it effectively divides multibyte unsigned values by two. The Carry 
 * Flag can be used to round the result. 
 */
void KLAVRCPU::execute_ROR( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = (rd >> 1) | (sreg & SREG_C ? 0x80 : 0);
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, rd & 1, SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_C)), SREG_V );
        setBit( sreg, (result == 0), SREG_Z );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SBC( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0000 10rd dddd rrrr
 *
 * Subtracts two registers and subtracts with the C Flag and places the 
 * result in the destination register Rd. 
 */
void KLAVRCPU::execute_SBC( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned char result = rd - rr - (sreg & SREG_C ? 1 : 0);
        setBit( sreg, ((!(rd&8)) && (rr&8)) ||
                      ((rr&8) && (result&8)) ||
                      ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(rr&0x80)) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (rr&0x80) && (result&0x80), SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, (result == 0) && (sreg & SREG_Z), SREG_Z );
        setBit( sreg, (!(rd&0x80)) && (rr&0x80) ||
                      (rr&0x80) && (result&0x80) ||
                      (result&0x80) && (!(rd&0x80)), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SBCI( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 0100 KKKK dddd KKKK
 *
 * Subtracts a constant from a register and subtracts with the C Flag and 
 * places the result in the destination register Rd. 
 */
void KLAVRCPU::execute_SBCI( u08 Rd, u08 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = K;
        unsigned char result = rd - rr;
        result -= (unsigned char) (sreg & SREG_C ? 1 : 0);
        setBit( sreg, ((!(rd&8)) && (rr&8)) ||
                      ((rr&8) && (result&8)) ||
                      ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(rr&0x80)) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (rr&0x80) && (result&0x80), SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, (result == 0) && (sreg & SREG_Z), SREG_Z );
        setBit( sreg, (!(rd&0x80)) && (rr&0x80) ||
                      (rr&0x80) && (result&0x80) ||
                      (result&0x80) && (!(rd&0x80)), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SBI( u08 A, u08 b )
 *
 * Bitmask of this operation:
 * 1001 1010 AAAA Abbb
 *
 * Sets a specified bit in an I/O Register. This instruction operates on 
 * the lower 32 I/O Registers - addresses 0-31. 
 */
void KLAVRCPU::execute_SBI( u08 A, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( A, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char reg = m_parent->readFromPCRam( A );
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        setBit( reg, true, 1 << b );
        m_parent->writeMemoryCell( A, reg, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SBIC( u08 A, u08 b )
 *
 * Bitmask of this operation:
 * 1001 1001 AAAA Abbb
 *
 * This instruction tests a single bit in an I/O Register and skips the next 
 * instruction if the bit is cleared. This instruction operates on the lower 32 I/O 
 * Registers - addresses 0-31. 
 */
void KLAVRCPU::execute_SBIC( u08 A, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( A + 0x20 );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char rr = m_parent->readFromPCRam( A + 0x20 );
        if (!( rr & (1<<b) ))
        {
            if ( isTwoWordInstruction( m_programCounter+2 ) )
                m_programCounter += 6;
            else
                m_programCounter += 4;
        }
        else
            m_programCounter+=2;
        m_state = STATE_CMD_FINISHED;
    }
}


/**
 * \fn KLAVRCPU::execute_SBIS( u08 A, u08 b )
 *
 * Bitmask of this operation:
 * 1001 1011 AAAA Abbb
 *
 * This instruction tests a single bit in an I/O Register and skips the next 
 * instruction if the bit is set. This instruction operates on the lower 32 I/O 
 * Registers - addresses 0-31. 
 */
void KLAVRCPU::execute_SBIS( u08 A, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( A + 0x20 );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char rr = m_parent->readFromPCRam( A + 0x20 );
        if ( rr & (1<<b) )
        {
            if ( isTwoWordInstruction( m_programCounter+2 ) )
                m_programCounter += 6;
            else
                m_programCounter += 4;
        }
        else
            m_programCounter+=2;
        m_state = STATE_CMD_FINISHED;
    }
}


/**
 * \fn KLAVRCPU::execute_SBIW( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 1001 0111 KKdd KKKK
 *
 * Subtracts an immediate value (0-63) from a register pair and places the 
 * result in the register pair. This instruction operates on the upper four 
 * register pairs, and is well suited for operations on the Pointer Registers. This 
 * instruction is not available in all devices. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_SBIW( u08 Rd, u08 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rd+1, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rdL = m_parent->readFromPCRam( Rd );
        unsigned char rdH = m_parent->readFromPCRam( Rd+1 );

        unsigned int result = rdH*256 + rdL - K;
        setBit( sreg, (rdH&0x80) && (!(result&0x8000)), SREG_V );
        setBit( sreg, result&0x8000, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, (result&0x8000) && (!(rdH&0x80)), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        rdL = result & 0xff;
        rdH = (result >> 8) & 0xff;
        m_parent->writeMemoryCell( Rd, rdL, Rd+1, rdH, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SBR( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 0110 KKKK dddd KKKK
 *
 * Sets specified bits in register Rd. Performs the logical ORI between 
 * the contents of register Rd and a constant mask K and places the result in the 
 * destination register Rd. 
 */
void KLAVRCPU::execute_SBR( u08, u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY ORI! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SBRC( u08 Rr, u08 b )
 *
 * Bitmask of this operation:
 * 1111 110r rrrr 0bbb
 *
 * This instruction tests a single bit in a register and skips the next 
 * instruction if the bit is cleared. 
 */
void KLAVRCPU::execute_SBRC( u08 Rr, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char rr = m_parent->readFromPCRam( Rr );
        if (!( rr & (1<<b) ))
        {
            if ( isTwoWordInstruction( m_programCounter+2 ) )
                m_programCounter += 6;
            else
                m_programCounter += 4;
        }
        else
            m_programCounter+=2;
        m_state = STATE_CMD_FINISHED;
    }
}


/**
 * \fn KLAVRCPU::execute_SBRS( u08 Rr, u08 b )
 *
 * Bitmask of this operation:
 * 1111 111r rrrr 0bbb
 *
 * This instruction tests a single bit in a register and skips the next 
 * instruction if the bit is set. 
 */
void KLAVRCPU::execute_SBRS( u08 Rr, u08 b )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char rr = m_parent->readFromPCRam( Rr );
        if ( rr & (1<<b) )
        {
            if ( isTwoWordInstruction( m_programCounter+2 ) )
                m_programCounter += 6;
            else
                m_programCounter += 4;
        }
        else
            m_programCounter+=2;
        m_state = STATE_CMD_FINISHED;
    }
}


/**
 * \fn KLAVRCPU::execute_SEC(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0000 1000
 *
 * Sets the Carry Flag (C) in SREG (Status Register). 
 */
void KLAVRCPU::execute_SEC( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SEH(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0101 1000
 *
 * Sets the Half Carry (H) in SREG (Status Register). 
 */
void KLAVRCPU::execute_SEH( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SEI(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0111 1000
 *
 * Sets the Global Interrupt Flag (I) in SREG (Status Register). The 
 * instruction following SEI will be executed before any pending interrupts. 
 */
void KLAVRCPU::execute_SEI( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SEN(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0010 1000
 *
 * Sets the Negative Flag (N) in SREG (Status Register). 
 */
void KLAVRCPU::execute_SEN( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SER( u08 Rd )
 *
 * Bitmask of this operation:
 * 1110 1111 dddd 1111
 *
 * Loads $FF directly to register Rd. 
 */
void KLAVRCPU::execute_SER( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY LDI! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SES(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0100 1000
 *
 * Sets the Signed Flag (S) in SREG (Status Register). 
 */
void KLAVRCPU::execute_SES( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SET(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0110 1000
 *
 * Sets the T Flag in SREG (Status Register). 
 */
void KLAVRCPU::execute_SET( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SEV(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0011 1000
 *
 * Sets the Overflow Flag (V) in SREG (Status Register). 
 */
void KLAVRCPU::execute_SEV( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SEZ(  )
 *
 * Bitmask of this operation:
 * 1001 0100 0001 1000
 *
 * Sets the Zero Flag (Z) in SREG (Status Register). 
 */
void KLAVRCPU::execute_SEZ( )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY BSET OR BCLR! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_SLEEP(  )
 *
 * Bitmask of this operation:
 * 1001 0101 1000 1000
 *
 * This instruction sets the circuit in sleep mode defined by the MCU 
 * Control Register. 
 */
void KLAVRCPU::execute_SLEEP(  )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_SPM(  )
 *
 * Bitmask of this operation:
 * 1001 0101 1110 1000
 *
 * SPM can be used to erase a page in the Program memory, to write a page in the 
 * Program memory (that is already erased), and to set Boot Loader Lock bits. In 
 * some devices, the Program memory can be written one word at a time, in other 
 * devices an entire page can be programmed simultaneously after first filling a 
 * temporary page buffer. In all cases, the Program memory must be erased one page at a 
 * time. When erasing the Program memory, the RAMPZ and Z-register are used as 
 * page address. When writing the Program memory, the RAMPZ and Z-register are 
 * used as page or word address, and the R1:R0 register pair is used as data(1). 
 * When setting the Boot Loader Lock bits, the R1:R0 register pair is used as 
 * data. Refer to the device documentation for detailed description of SPM 
 * usage. This instruction can address the entire Program memory. This 
 * instruction is not available in all devices. Refer to the device specific 
 * instruction set summary. 
 */
void KLAVRCPU::execute_SPM(  )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * \fn KLAVRCPU::execute_STX1( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 1100
 *
 * Stores one byte indirect from a register to data space. For parts with 
 * SRAM, the data space consists of the Register File, I/O memory and internal 
 * SRAM (and external SRAM if applicable). For parts without SRAM, the data 
 * space consists of the Register File only. The EEPROM has a separate address 
 * space. The data location is pointed to by the X (16 bits) Pointer Register in the 
 * Register File. Memory access is limited to the current data segment of 64K bytes. 
 * To access another data segment in devices with more than 64K bytes data 
 * space, the RAMPX in register in the I/O area has to be changed. The X-pointer 
 * Register can either be left unchanged by the operation, or it can be 
 * post-incremented or pre-decremented. These features are especially suited for 
 * accessing arrays, tables, and Stack Pointer usage of the X-pointer Register. 
 * Note that only the low byte of the X-pointer is updated in devices with no more 
 * than 256 bytes data space. For such devices, the high byte of the pointer is 
 * not used by this instruction and can be used for other purposes. The RAMPX 
 * Register in the I/O area is updated in parts with more than 64K bytes data space or 
 * more than 64K bytes Program memory, and the increment/ decrement is added to 
 * the entire 24-bit address on such devices. Not all variants of this 
 * instruction is available in all devices. Refer to the device specific instruction 
 * set summary. 
 */
void KLAVRCPU::execute_STX1( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->xl(), m_parent->xh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                           m_parent->readFromPCRam( m_parent->xh() ) * 256;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STX2( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 1101
 *
 *  
 */
void KLAVRCPU::execute_STX2( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->xl(), m_parent->xh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                           m_parent->readFromPCRam( m_parent->xh() ) * 256;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ),
                                   m_parent->xh(), ((adr+1)&0xff00)>>8,
                                   m_parent->xl(), (adr+1)&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STX3( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 1110
 *
 *  
 */
void KLAVRCPU::execute_STX3( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->xl(), m_parent->xh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->xl() ) +
                           m_parent->readFromPCRam( m_parent->xh() ) * 256;
        adr--;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ),
                                   m_parent->xh(), (adr&0xff00)>>8,
                                   m_parent->xl(), adr&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STY1( u08 Rr )
 *
 * Bitmask of this operation:
 * 1000 001r rrrr 1000
 *
 * Stores one byte indirect with or without displacement from a register 
 * to data space. For parts with SRAM, the data space consists of the Register 
 * File, I/O memory and internal SRAM (and external SRAM if applicable). For 
 * parts without SRAM, the data space consists of the Register File only. The 
 * EEPROM has a separate address space. The data location is pointed to by the Y (16 
 * bits) Pointer Register in the Register File. Memory access is limited to the 
 * current data segment of 64K bytes. To access another data segment in devices 
 * with more than 64K bytes data space, the RAMPY in register in the I/O area has 
 * to be changed. The Y-pointer Register can either be left unchanged by the 
 * operation, or it can be post-incremented or pre-decremented. These features are 
 * especially suited for accessing arrays, tables, and Stack Pointer usage of the 
 * Y-pointer Register. Note that only the low byte of the Y-pointer is updated in 
 * devices with no more than 256 bytes data space. For such devices, the high byte of 
 * the pointer is not used by this instruction and can be used for other 
 * purposes. The RAMPY Register in the I/O area is updated in parts with more than 64K 
 * bytes data space or more than 64K bytes Program memory, and the increment/ 
 * decrement/displacement is added to the entire 24-bit address on such devices. Not all variants 
 * of this instruction is available in all devices. Refer to the device 
 * specific instruction set summary. 
 */
void KLAVRCPU::execute_STY1( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STY2( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 1001
 *
 *  
 */
void KLAVRCPU::execute_STY2( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ),
                                   m_parent->yh(), ((adr+1)&0xff00)>>8,
                                   m_parent->yl(), (adr+1)&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STY3( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 1010
 *
 *  
 */
void KLAVRCPU::execute_STY3( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256;
        adr--;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ),
                                   m_parent->yh(), (adr&0xff00)>>8,
                                   m_parent->yl(), adr&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STY4( u08 Rr, u08 q )
 *
 * Bitmask of this operation:
 * 10q0 qq1r rrrr 1qqq
 *
 *  
 */
void KLAVRCPU::execute_STY4( u08 Rr, u08 q )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->yl(), m_parent->yh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->yl() ) +
                           m_parent->readFromPCRam( m_parent->yh() ) * 256 + q;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STZ1( u08 Rr )
 *
 * Bitmask of this operation:
 * 1000 001r rrrr 0000
 *
 * Stores one byte indirect with or without displacement from a register 
 * to data space. For parts with SRAM, the data space consists of the Register 
 * File, I/O memory and internal SRAM (and external SRAM if applicable). For 
 * parts without SRAM, the data space consists of the Register File only. The 
 * EEPROM has a separate address space. The data location is pointed to by the Z (16 
 * bits) Pointer Register in the Register File. Memory access is limited to the 
 * current data segment of 64K bytes. To access another data segment in devices 
 * with more than 64K bytes data space, the RAMPZ in register in the I/O area has 
 * to be changed. The Z-pointer Register can either be left unchanged by the 
 * operation, or it can be post-incremented or pre-decremented. These features are 
 * especially suited for Stack Pointer usage of the Z-pointer Register, however 
 * because the Z-pointer Register can be used for indirect subroutine calls, 
 * indirect jumps and table lookup, it is often more convenient to use the X or 
 * Y-pointer as a dedicated Stack Pointer. Note that only the low byte of the 
 * Z-pointer is updated in devices with no more than 256 bytes data space. For such 
 * devices, the high byte of the pointer is not used by this instruction and can be 
 * used for other purposes. The RAMPZ Register in the I/O area is updated in 
 * parts with more than 64K bytes data space or more than 64K bytes Program 
 * memory, and the increment/decrement/displacement is added to the entire 
 * 24-bit address on such devices. Not all variants of this instruction is 
 * available in all devices. Refer to the device specific instruction set summary. 
 */
void KLAVRCPU::execute_STZ1( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STZ2( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 0001
 *
 *  
 */
void KLAVRCPU::execute_STZ2( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ),
                                   m_parent->zh(), ((adr+1)&0xff00)>>8,
                                   m_parent->zl(), (adr+1)&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STZ3( u08 Rr )
 *
 * Bitmask of this operation:
 * 1001 001r rrrr 0010
 *
 *  
 */
void KLAVRCPU::execute_STZ3( u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256;
        adr--;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ),
                                   m_parent->zh(), (adr&0xff00)>>8,
                                   m_parent->zl(), adr&0xff );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STZ4( u08 Rr, u08 q )
 *
 * Bitmask of this operation:
 * 10q0 qq1r rrrr 0qqq
 *
 *  
 */
void KLAVRCPU::execute_STZ4( u08 Rr, u08 q )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rr, m_parent->zl(), m_parent->zh() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned int adr = m_parent->readFromPCRam( m_parent->zl() ) +
                           m_parent->readFromPCRam( m_parent->zh() ) * 256 + q;
        m_parent->writeMemoryCell( adr, m_parent->readFromPCRam( Rr ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_STS( u16 k, u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 001d dddd 0000 kkkk kkkk kkkk kkkk
 *
 * Stores one byte from a Register to the data space. For parts with SRAM, 
 * the data space consists of the Register File, I/O memory and internal SRAM 
 * (and external SRAM if applicable). For parts without SRAM, the data space 
 * consists of the Register File only. The EEPROM has a separate address space. A 
 * 16-bit address must be supplied. Memory access is limited to the current data 
 * segment of 64K bytes. The STS instruction uses the RAMPD Register to access 
 * memory above 64K bytes. To access another data segment in devices with more 
 * than 64K bytes data space, the RAMPD in register in the I/O area has to be 
 * changed. This instruction is not available in all devices. Refer to the device 
 * specific instruction set summary. 
 */
void KLAVRCPU::execute_STS( u16 k, u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        m_parent->writeMemoryCell( k, m_parent->readFromPCRam( Rd ) );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=4;
    }
}


/**
 * \fn KLAVRCPU::execute_SUB( u08 Rd, u08 Rr )
 *
 * Bitmask of this operation:
 * 0001 10rd dddd rrrr
 *
 * Subtracts two registers and places the result in the destination 
 * register Rd. 
 */
void KLAVRCPU::execute_SUB( u08 Rd, u08 Rr )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, Rr, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = m_parent->readFromPCRam( Rr );
        unsigned char result = rd - rr;
        setBit( sreg, ((!(rd&8)) && (rr&8)) ||
                      ((rr&8) && (result&8)) ||
                      ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(rr&0x80)) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (rr&0x80) && (result&0x80), SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, (!(rd&0x80)) && (rr&0x80) ||
                      (rr&0x80) && (result&0x80) ||
                      (result&0x80) && (!(rd&0x80)), SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SUBI( u08 Rd, u08 K )
 *
 * Bitmask of this operation:
 * 0101 KKKK dddd KKKK
 *
 * Subtracts a register and a constant and places the result in the 
 * destination register Rd. This instruction is working on Register R16 to R31 and is 
 * very well suited for operations on the X, Y and Z-pointers. 
 */
void KLAVRCPU::execute_SUBI( u08 Rd, u08 K )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd, m_parent->sreg() );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char sreg = m_parent->readFromPCRam( m_parent->sreg() );
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char rr = K;
        unsigned char result = rd - rr;
        setBit( sreg, ((!(rd&8)) && (rr&8)) ||
                      ((rr&8) && (result&8)) ||
                      ((result&8) && (!(rd&8))), SREG_H );
        setBit( sreg, (rd&0x80) && (!(rr&0x80)) && (!(result&0x80)) ||
                      (!(rd&0x80)) && (rr&0x80) && (result&0x80), SREG_V );
        setBit( sreg, result&0x80, SREG_N );
        setBit( sreg, result == 0, SREG_Z );
        setBit( sreg, (!(rd&0x80)) && (rr&0x80) ||
                      (rr&0x80) && (result&0x80) ||
                      (result&0x80) && (!(rd&0x80)), SREG_C );
        // qDebug( "Carry %d", sreg & SREG_C );
        setBit( sreg, ((bool)(sreg & SREG_N)) != ((bool)(sreg & SREG_V)), SREG_S );
        m_parent->writeMemoryCell( Rd, result, m_parent->sreg(), sreg );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_SWAP( u08 Rd )
 *
 * Bitmask of this operation:
 * 1001 010d dddd 0010
 *
 * Swaps high and low nibbles in a register. 
 */
void KLAVRCPU::execute_SWAP( u08 Rd )
{
    /// @TODO QA Check this function!
    if ( m_state == STATE_READ_DATA )
    {
        // This ensures the RAM is valid:
        m_parent->readMemoryCell( Rd );
        m_state = STATE_WAIT_FOR_READ_DATA;
    }
    else if ( m_state == STATE_WRITE_BACK )
    {
        unsigned char rd = m_parent->readFromPCRam( Rd );
        unsigned char result = ((rd << 4) & 0xF0) | ((rd >> 4) & 0xF);
        m_parent->writeMemoryCell( Rd, result );
        m_state = STATE_WAIT_FOR_WRITE_BACK;
        m_programCounter+=2;
    }
}


/**
 * \fn KLAVRCPU::execute_TST( u08 Rd )
 *
 * Bitmask of this operation:
 * 0010 00dd dddd dddd
 *
 * Tests if a register is zero or negative. Performs a logical AND between a 
 * register and itself. The register will remain unchanged. 
 */
void KLAVRCPU::execute_TST( u08 )
{
    qWarning("ERROR: THIS COMMAND IS SHADOWED BY AND! %s:%d", __FILE__, __LINE__);
}


/**
 * \fn KLAVRCPU::execute_WDR(  )
 *
 * Bitmask of this operation:
 * 1001 0101 1010 1000
 *
 * This instruction resets the Watchdog Timer. This instruction must be 
 * executed within a limited time given by the WD prescaler. See the Watchdog Timer 
 * hardware specification. 
 */
void KLAVRCPU::execute_WDR(  )
{
    /// @TODO QA Check this function!
    /// @TODO Implement this function!
}


/**
 * KLAVRCPU::execute( int location )
 *
 * This is the command that executes instructions from flash.
 * @param location The location of the upper 16 bits of the command.
 */
void KLAVRCPU::execute( int location_ )
{
    if ( m_state == STATE_CMD_FINISHED )
        m_state = STATE_READ_DATA;
    else if ( m_state == STATE_WAIT_FOR_READ_DATA_INTERNAL )
        m_state = STATE_READ_DATA_INTERNAL;
    else if ( m_state == STATE_WAIT_FOR_READ_DATA )
        m_state = STATE_WRITE_BACK;
    else if ( m_state == STATE_WAIT_FOR_WRITE_BACK )
        m_state = STATE_READ_DATA;
    
    int location = location_;
    if ( location < 0 )
        location = m_programCounter;
    
    if ( location & 1 )
        qWarning("The location is odd! %s:%d", __FILE__, __LINE__);
    // Determine the command
    unsigned int cmd16Bits = get16BitCommandWordStartingAt( location );
    unsigned int cmd32Bits = get32BitCommandWordStartingAt( location );

    m_endlessLoop = false;
    /*
    unsigned char spl = m_parent->readFromPCRam( m_parent->spl() );
    unsigned char sph = m_parent->readFromPCRam( m_parent->sph() );
    unsigned int sp = spl + sph*256;
    qDebug( "SP is %d at cmd %d", sp, location );
    */
    // qDebug("executing %d", location);

    // Here come all the commands:
    // ADC
    // Bit mask:
    // 0001 11rd dddd rrrr
    if ( ( cmd16Bits & 0xfc00 ) == 0x1c00 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_ADC( Rd, Rr );
    }
    // ADD
    // Bit mask:
    // 0000 11rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0xc00 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_ADD( Rd, Rr );
    }
    // ADIW
    // Bit mask:
    // 1001 0110 KKdd KKKK
    else if ( ( cmd16Bits & 0xff00 ) == 0x9600 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0);
        Rd = Rd*2 + 24;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 7))?(1<<5):0) |
                ((cmd16Bits & (1 << 6))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_ADIW( Rd, K );
    }
    // AND
    // Bit mask:
    // 0010 00rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x2000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_AND( Rd, Rr );
    }
    // ANDI
    // Bit mask:
    // 0111 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x7000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_ANDI( Rd, K );
    }
    // ASR
    // Bit mask:
    // 1001 010d dddd 0101
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9405 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_ASR( Rd );
    }
    // BCLR
    // Bit mask:
    // 1001 0100 1sss 1000
    else if ( ( cmd16Bits & 0xff8f ) == 0x9488 )
    {
        u08 s = 0;
        s = ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_BCLR( s );
    }
    // BLD
    // Bit mask:
    // 1111 100d dddd 0bbb
    else if ( ( cmd16Bits & 0xfe08 ) == 0xf800 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_BLD( Rd, b );
    }
    // BRBC
    // Bit mask:
    // 1111 01kk kkkk ksss
    else if ( ( cmd16Bits & 0xfc00 ) == 0xf400 )
    {
        u08 s = 0;
        s = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRBC( s, k );
    }
    // BRBS
    // Bit mask:
    // 1111 00kk kkkk ksss
    else if ( ( cmd16Bits & 0xfc00 ) == 0xf000 )
    {
        u08 s = 0;
        s = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRBS( s, k );
    }
    // BRCC
    // Bit mask:
    // 1111 01kk kkkk k000
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf400 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRCC( k );
    }
    // BRCS
    // Bit mask:
    // 1111 00kk kkkk k000
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf000 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRCS( k );
    }
    // BREAK
    // Bit mask:
    // 1001 0101 1001 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9598 )
    {
        execute_BREAK();
    }
    // BREQ
    // Bit mask:
    // 1111 00kk kkkk k001
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf001 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BREQ( k );
    }
    // BRGE
    // Bit mask:
    // 1111 01kk kkkk k100
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf404 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRGE( k );
    }
    // BRHC
    // Bit mask:
    // 1111 01kk kkkk k101
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf405 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRHC( k );
    }
    // BRHS
    // Bit mask:
    // 1111 00kk kkkk k101
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf005 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRHS( k );
    }
    // BRID
    // Bit mask:
    // 1111 01kk kkkk k111
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf407 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRID( k );
    }
    // BRIE
    // Bit mask:
    // 1111 00kk kkkk k111
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf007 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRIE( k );
    }
    // BRLO
    // Bit mask:
    // 1111 00kk kkkk k000
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf000 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRLO( k );
    }
    // BRLT
    // Bit mask:
    // 1111 00kk kkkk k100
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf004 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRLT( k );
    }
    // BRMI
    // Bit mask:
    // 1111 00kk kkkk k010
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf002 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRMI( k );
    }
    // BRNE
    // Bit mask:
    // 1111 01kk kkkk k001
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf401 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRNE( k );
    }
    // BRPL
    // Bit mask:
    // 1111 01kk kkkk k010
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf402 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRPL( k );
    }
    // BRSH
    // Bit mask:
    // 1111 01kk kkkk k000
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf400 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRSH( k );
    }
    // BRTC
    // Bit mask:
    // 1111 01kk kkkk k110
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf406 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRTC( k );
    }
    // BRTS
    // Bit mask:
    // 1111 00kk kkkk k110
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf006 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRTS( k );
    }
    // BRVC
    // Bit mask:
    // 1111 01kk kkkk k011
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf403 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRVC( k );
    }
    // BRVS
    // Bit mask:
    // 1111 00kk kkkk k011
    else if ( ( cmd16Bits & 0xfc07 ) == 0xf003 )
    {
        u08 k = 0;
        k = ((cmd16Bits & (1 << 9))?(1<<6):0) |
                ((cmd16Bits & (1 << 8))?(1<<5):0) |
                ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;

        execute_BRVS( k );
    }
    // BSET
    // Bit mask:
    // 1001 0100 0sss 1000
    else if ( ( cmd16Bits & 0xff8f ) == 0x9408 )
    {
        u08 s = 0;
        s = ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_BSET( s );
    }
    // BST
    // Bit mask:
    // 1111 101d dddd 0bbb
    else if ( ( cmd16Bits & 0xfe08 ) == 0xfa00 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_BST( Rd, b );
    }
    // CALL
    // Bit mask:
    // 1001 010k kkkk 111k kkkk kkkk kkkk kkkk
    else if ( ( cmd32Bits & 0xFE0E0000L ) ==
                0x940E0000L )
    {
        u32 k = 0;
        k = ((cmd32Bits & (1 << 24))?(1<<21):0) |
                ((cmd32Bits & (1 << 23))?(1<<20):0) |
                ((cmd32Bits & (1 << 22))?(1<<19):0) |
                ((cmd32Bits & (1 << 21))?(1<<18):0) |
                ((cmd32Bits & (1 << 20))?(1<<17):0) |
                (cmd32Bits & (1 << 16)) |
                (cmd32Bits & (1 << 15)) |
                (cmd32Bits & (1 << 14)) |
                (cmd32Bits & (1 << 13)) |
                (cmd32Bits & (1 << 12)) |
                (cmd32Bits & (1 << 11)) |
                (cmd32Bits & (1 << 10)) |
                (cmd32Bits & (1 << 9)) |
                (cmd32Bits & (1 << 8)) |
                (cmd32Bits & (1 << 7)) |
                (cmd32Bits & (1 << 6)) |
                (cmd32Bits & (1 << 5)) |
                (cmd32Bits & (1 << 4)) |
                (cmd32Bits & (1 << 3)) |
                (cmd32Bits & (1 << 2)) |
                (cmd32Bits & (1 << 1)) |
                (cmd32Bits & (1 << 0)) ;

        execute_CALL( k );
    }
    // CBI
    // Bit mask:
    // 1001 1000 AAAA Abbb
    else if ( ( cmd16Bits & 0xff00 ) == 0x9800 )
    {
        u08 A = 0;
        A = ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CBI( A, b );
    }
    // CBR
    // Bit mask:
    // 0111 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x7000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CBR( Rd, K );
    }
    // CLC
    // Bit mask:
    // 1001 0100 1000 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9488 )
    {
        execute_CLC();
    }
    // CLH
    // Bit mask:
    // 1001 0100 1101 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x94d8 )
    {
        execute_CLH();
    }
    // CLI
    // Bit mask:
    // 1001 0100 1111 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x94f8 )
    {
        execute_CLI();
    }
    // CLN
    // Bit mask:
    // 1001 0100 1010 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x94a8 )
    {
        execute_CLN();
    }
    // CLR
    // Bit mask:
    // 0010 01dd dddd dddd
    else if ( ( cmd16Bits & 0xfc00 ) == 0x2400 )
    {
        u16 Rd = 0;
        Rd = ((cmd16Bits & (1 << 9)) ? 0x10 : 0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CLR( Rd );
    }
    // CLS
    // Bit mask:
    // 1001 0100 1100 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x94c8 )
    {
        execute_CLS();
    }
    // CLT
    // Bit mask:
    // 1001 0100 1110 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x94e8 )
    {
        execute_CLT();
    }
    // CLV
    // Bit mask:
    // 1001 0100 1011 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x94b8 )
    {
        execute_CLV();
    }
    // CLZ
    // Bit mask:
    // 1001 0100 1001 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9498 )
    {
        execute_CLZ();
    }
    // COM
    // Bit mask:
    // 1001 010d dddd 0000
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9400 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_COM( Rd );
    }
    // CP
    // Bit mask:
    // 0001 01rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x1400 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CP( Rd, Rr );
    }
    // CPC
    // Bit mask:
    // 0000 01rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x400 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CPC( Rd, Rr );
    }
    // CPI
    // Bit mask:
    // 0011 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x3000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CPI( Rd, K );
    }
    // CPSE
    // Bit mask:
    // 0001 00rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x1000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_CPSE( Rd, Rr );
    }
    // DEC
    // Bit mask:
    // 1001 010d dddd 1010
    else if ( ( cmd16Bits & 0xfe0f ) == 0x940a )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_DEC( Rd );
    }
    // EICALL
    // Bit mask:
    // 1001 0101 0001 1001
    else if ( ( cmd16Bits & 0xffff ) == 0x9519 )
    {
        execute_EICALL();
    }
    // EIJMP
    // Bit mask:
    // 1001 0100 0001 1001
    else if ( ( cmd16Bits & 0xffff ) == 0x9419 )
    {
        execute_EIJMP();
    }
    // ELPM1
    // Bit mask:
    // 1001 0101 1101 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x95d8 )
    {
        execute_ELPM1();
    }
    // ELPM2
    // Bit mask:
    // 1001 000d dddd 0110
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9006 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_ELPM2( Rd );
    }
    // ELPM3
    // Bit mask:
    // 1001 000d dddd 0111
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9007 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_ELPM3( Rd );
    }
    // EOR
    // Bit mask:
    // 0010 01rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x2400 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_EOR( Rd, Rr );
    }
    // FMUL
    // Bit mask:
    // 0000 0011 0ddd 1rrr
    else if ( ( cmd16Bits & 0xff88 ) == 0x308 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 Rr = 0;
        Rr = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        Rr += 16;

        execute_FMUL( Rd, Rr );
    }
    // FMULS
    // Bit mask:
    // 0000 0011 1ddd 0rrr
    else if ( ( cmd16Bits & 0xff88 ) == 0x380 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 Rr = 0;
        Rr = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        Rr += 16;

        execute_FMULS( Rd, Rr );
    }
    // FMULSU
    // Bit mask:
    // 0000 0011 1ddd 1rrr
    else if ( ( cmd16Bits & 0xff88 ) == 0x388 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 Rr = 0;
        Rr = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        Rr += 16;

        execute_FMULSU( Rd, Rr );
    }
    // ICALL
    // Bit mask:
    // 1001 0101 0000 1001
    else if ( ( cmd16Bits & 0xffff ) == 0x9509 )
    {
        execute_ICALL();
    }
    // IJMP
    // Bit mask:
    // 1001 0100 0000 1001
    else if ( ( cmd16Bits & 0xffff ) == 0x9409 )
    {
        execute_IJMP();
    }
    // IN
    // Bit mask:
    // 1011 0AAd dddd AAAA
    else if ( ( cmd16Bits & 0xf800 ) == 0xb000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 A = 0;
        A = ((cmd16Bits & (1 << 10))?(1<<5):0) |
                ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        A += 32;
        execute_IN( Rd, A );
    }
    // INC
    // Bit mask:
    // 1001 010d dddd 0011
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9403 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_INC( Rd );
    }
    // JMP
    // Bit mask:
    // 1001 010k kkkk 110k kkkk kkkk kkkk kkkk
    else if ( ( cmd32Bits & 0xFE0E0000L ) ==
                0x940C0000L )
    {
        u16 k = 0;
        k = ((cmd32Bits & (1 << 24))?(1<<21):0) |
                ((cmd32Bits & (1 << 23))?(1<<20):0) |
                ((cmd32Bits & (1 << 22))?(1<<19):0) |
                ((cmd32Bits & (1 << 21))?(1<<18):0) |
                ((cmd32Bits & (1 << 20))?(1<<17):0) |
                (cmd32Bits & (1 << 16)) |
                (cmd32Bits & (1 << 15)) |
                (cmd32Bits & (1 << 14)) |
                (cmd32Bits & (1 << 13)) |
                (cmd32Bits & (1 << 12)) |
                (cmd32Bits & (1 << 11)) |
                (cmd32Bits & (1 << 10)) |
                (cmd32Bits & (1 << 9)) |
                (cmd32Bits & (1 << 8)) |
                (cmd32Bits & (1 << 7)) |
                (cmd32Bits & (1 << 6)) |
                (cmd32Bits & (1 << 5)) |
                (cmd32Bits & (1 << 4)) |
                (cmd32Bits & (1 << 3)) |
                (cmd32Bits & (1 << 2)) |
                (cmd32Bits & (1 << 1)) |
                (cmd32Bits & (1 << 0)) ;

        execute_JMP( k );
    }
    // LDX1
    // Bit mask:
    // 1001 000d dddd 1100
    else if ( ( cmd16Bits & 0xfe0f ) == 0x900c )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDX1( Rd );
    }
    // LDX2
    // Bit mask:
    // 1001 000d dddd 1101
    else if ( ( cmd16Bits & 0xfe0f ) == 0x900d )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDX2( Rd );
    }
    // LDX3
    // Bit mask:
    // 1001 000d dddd 1110
    else if ( ( cmd16Bits & 0xfe0f ) == 0x900e )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDX3( Rd );
    }
    // LDY1
    // Bit mask:
    // 1000 000d dddd 1000
    else if ( ( cmd16Bits & 0xfe0f ) == 0x8008 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDY1( Rd );
    }
    // LDY2
    // Bit mask:
    // 1001 000d dddd 1001
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9009 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDY2( Rd );
    }
    // LDY3
    // Bit mask:
    // 1001 000d dddd 1010
    else if ( ( cmd16Bits & 0xfe0f ) == 0x900a )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDY3( Rd );
    }
    // LDY4
    // Bit mask:
    // 10q0 qq0d dddd 1qqq
    else if ( ( cmd16Bits & 0xd208 ) == 0x8008 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 q = 0;
        q = ((cmd16Bits & (1 << 13))?(1<<5):0) |
                ((cmd16Bits & (1 << 11))?(1<<4):0) |
                ((cmd16Bits & (1 << 10))?(1<<3):0) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_LDY4( Rd, q );
    }
    // LDZ1
    // Bit mask:
    // 1000 000d dddd 0000
    else if ( ( cmd16Bits & 0xfe0f ) == 0x8000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDZ1( Rd );
    }
    // LDZ2
    // Bit mask:
    // 1001 000d dddd 0001
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9001 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDZ2( Rd );
    }
    // LDZ3
    // Bit mask:
    // 1001 000d dddd 0010
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9002 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LDZ3( Rd );
    }
    // LDZ4
    // Bit mask:
    // 10q0 qq0d dddd 0qqq
    else if ( ( cmd16Bits & 0xd208 ) == 0x8000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 q = 0;
        q = ((cmd16Bits & (1 << 13))?(1<<5):0) |
                ((cmd16Bits & (1 << 11))?(1<<4):0) |
                ((cmd16Bits & (1 << 10))?(1<<3):0) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_LDZ4( Rd, q );
    }
    // LDI
    // Bit mask:
    // 1110 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0xe000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_LDI( Rd, K );
    }
    // LDS
    // Bit mask:
    // 1001 000d dddd 0000 kkkk kkkk kkkk kkkk
    else if ( ( cmd32Bits & 0xFE0F0000L ) ==
                0x90000000L )
    {
        u08 Rd = 0;
        Rd = ((cmd32Bits & (1 << 24))?(1<<4):0) |
                ((cmd32Bits & (1 << 23))?(1<<3):0) |
                ((cmd32Bits & (1 << 22))?(1<<2):0) |
                ((cmd32Bits & (1 << 21))?(1<<1):0) |
                ((cmd32Bits & (1 << 20))?(1<<0):0) ;
        u16 k = 0;
        k = (cmd32Bits & (1 << 15)) |
                (cmd32Bits & (1 << 14)) |
                (cmd32Bits & (1 << 13)) |
                (cmd32Bits & (1 << 12)) |
                (cmd32Bits & (1 << 11)) |
                (cmd32Bits & (1 << 10)) |
                (cmd32Bits & (1 << 9)) |
                (cmd32Bits & (1 << 8)) |
                (cmd32Bits & (1 << 7)) |
                (cmd32Bits & (1 << 6)) |
                (cmd32Bits & (1 << 5)) |
                (cmd32Bits & (1 << 4)) |
                (cmd32Bits & (1 << 3)) |
                (cmd32Bits & (1 << 2)) |
                (cmd32Bits & (1 << 1)) |
                (cmd32Bits & (1 << 0)) ;

        execute_LDS( Rd, k );
    }
    // LPM1
    // Bit mask:
    // 1001 0101 1100 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x95c8 )
    {
        execute_LPM1();
    }
    // LPM2
    // Bit mask:
    // 1001 000d dddd 0100
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9004 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LPM2( Rd );
    }
    // LPM3
    // Bit mask:
    // 1001 000d dddd 0101
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9005 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LPM3( Rd );
    }
    // LSL
    // Bit mask:
    // 0000 11dd dddd dddd
    else if ( ( cmd16Bits & 0xfc00 ) == 0xc00 )
    {
        u16 Rd = 0;
        Rd = (cmd16Bits & (1 << 9)) |
                (cmd16Bits & (1 << 8)) |
                (cmd16Bits & (1 << 7)) |
                (cmd16Bits & (1 << 6)) |
                (cmd16Bits & (1 << 5)) |
                (cmd16Bits & (1 << 4)) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_LSL( Rd );
    }
    // LSR
    // Bit mask:
    // 1001 010d dddd 0110
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9406 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_LSR( Rd );
    }
    // MOV
    // Bit mask:
    // 0010 11rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x2c00 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_MOV( Rd, Rr );
    }
    // MOVW
    // Bit mask:
    // 0000 0001 dddd rrrr
    else if ( ( cmd16Bits & 0xff00 ) == 0x100 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        Rd *= 2;
        Rr *= 2;
        execute_MOVW( Rd, Rr );
    }
    // MUL
    // Bit mask:
    // 1001 11rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x9c00 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_MUL( Rd, Rr );
    }
    // MULS
    // Bit mask:
    // 0000 0010 dddd rrrr
    else if ( ( cmd16Bits & 0xff00 ) == 0x200 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 Rr = 0;
        Rr = (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        Rr += 16;

        execute_MULS( Rd, Rr );
    }
    // MULSU
    // Bit mask:
    // 0000 0011 0ddd 0rrr
    else if ( ( cmd16Bits & 0xff88 ) == 0x300 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 Rr = 0;
        Rr = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        Rr += 16;

        execute_MULSU( Rd, Rr );
    }
    // NEG
    // Bit mask:
    // 1001 010d dddd 0001
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9401 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_NEG( Rd );
    }
    // NOP
    // Bit mask:
    // 0000 0000 0000 0000
    else if ( ( cmd16Bits & 0xffff ) == 0x0 )
    {
        execute_NOP();
    }
    // OR
    // Bit mask:
    // 0010 10rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x2800 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_OR( Rd, Rr );
    }
    // ORI
    // Bit mask:
    // 0110 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x6000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0);
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_ORI( Rd, K );
    }
    // OUT
    // Bit mask:
    // 1011 1AAr rrrr AAAA
    else if ( ( cmd16Bits & 0xf800 ) == 0xb800 )
    {
        u08 A = 0;
        A = ((cmd16Bits & (1 << 10))?(1<<5):0) |
                ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;
        A += 32;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_OUT( A, Rr );
    }
    // POP
    // Bit mask:
    // 1001 000d dddd 1111
    else if ( ( cmd16Bits & 0xfe0f ) == 0x900f )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_POP( Rd );
    }
    // PUSH
    // Bit mask:
    // 1001 001d dddd 1111
    else if ( ( cmd16Bits & 0xfe0f ) == 0x920f )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_PUSH( Rd );
    }
    // RCALL
    // Bit mask:
    // 1101 kkkk kkkk kkkk
    else if ( ( cmd16Bits & 0xf000 ) == 0xd000 )
    {
        u16 k = 0;
        k = (cmd16Bits & (1 << 11)) |
                (cmd16Bits & (1 << 10)) |
                (cmd16Bits & (1 << 9)) |
                (cmd16Bits & (1 << 8)) |
                (cmd16Bits & (1 << 7)) |
                (cmd16Bits & (1 << 6)) |
                (cmd16Bits & (1 << 5)) |
                (cmd16Bits & (1 << 4)) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_RCALL( k );
    }
    // RET
    // Bit mask:
    // 1001 0101 0000 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9508 )
    {
        execute_RET();
    }
    // RETI
    // Bit mask:
    // 1001 0101 0001 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9518 )
    {
        execute_RETI();
    }
    // RJMP
    // Bit mask:
    // 1100 kkkk kkkk kkkk
    else if ( ( cmd16Bits & 0xf000 ) == 0xc000 )
    {
        u16 k = 0;
        k = (cmd16Bits & (1 << 11)) |
                (cmd16Bits & (1 << 10)) |
                (cmd16Bits & (1 << 9)) |
                (cmd16Bits & (1 << 8)) |
                (cmd16Bits & (1 << 7)) |
                (cmd16Bits & (1 << 6)) |
                (cmd16Bits & (1 << 5)) |
                (cmd16Bits & (1 << 4)) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_RJMP( k );
    }
    // ROL
    // Bit mask:
    // 0001 11dd dddd dddd
    else if ( ( cmd16Bits & 0xfc00 ) == 0x1c00 )
    {
        u16 Rd = 0;
        Rd = (cmd16Bits & (1 << 9)) |
                (cmd16Bits & (1 << 8)) |
                (cmd16Bits & (1 << 7)) |
                (cmd16Bits & (1 << 6)) |
                (cmd16Bits & (1 << 5)) |
                (cmd16Bits & (1 << 4)) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_ROL( Rd );
    }
    // ROR
    // Bit mask:
    // 1001 010d dddd 0111
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9407 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_ROR( Rd );
    }
    // SBC
    // Bit mask:
    // 0000 10rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x800 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBC( Rd, Rr );
    }
    // SBCI
    // Bit mask:
    // 0100 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x4000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBCI( Rd, K );
    }
    // SBI
    // Bit mask:
    // 1001 1010 AAAA Abbb
    else if ( ( cmd16Bits & 0xff00 ) == 0x9a00 )
    {
        u08 A = 0;
        A = ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBI( A, b );
    }
    // SBIC
    // Bit mask:
    // 1001 1001 AAAA Abbb
    else if ( ( cmd16Bits & 0xff00 ) == 0x9900 )
    {
        u08 A = 0;
        A = ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBIC( A, b );
    }
    // SBIS
    // Bit mask:
    // 1001 1011 AAAA Abbb
    else if ( ( cmd16Bits & 0xff00 ) == 0x9b00 )
    {
        u08 A = 0;
        A = ((cmd16Bits & (1 << 7))?(1<<4):0) |
                ((cmd16Bits & (1 << 6))?(1<<3):0) |
                ((cmd16Bits & (1 << 5))?(1<<2):0) |
                ((cmd16Bits & (1 << 4))?(1<<1):0) |
                ((cmd16Bits & (1 << 3))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBIS( A, b );
    }
    // SBIW
    // Bit mask:
    // 1001 0111 KKdd KKKK
    else if ( ( cmd16Bits & 0xff00 ) == 0x9700 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd = Rd*2 + 24;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 7))?(1<<5):0) |
                ((cmd16Bits & (1 << 6))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBIW( Rd, K );
    }
    // SBR
    // Bit mask:
    // 0110 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x6000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBR( Rd, K );
    }
    // SBRC
    // Bit mask:
    // 1111 110r rrrr 0bbb
    else if ( ( cmd16Bits & 0xfe08 ) == 0xfc00 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBRC( Rr, b );
    }
    // SBRS
    // Bit mask:
    // 1111 111r rrrr 0bbb
    else if ( ( cmd16Bits & 0xfe08 ) == 0xfe00 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 b = 0;
        b = (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SBRS( Rr, b );
    }
    // SEC
    // Bit mask:
    // 1001 0100 0000 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9408 )
    {
        execute_SEC();
    }
    // SEH
    // Bit mask:
    // 1001 0100 0101 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9458 )
    {
        execute_SEH();
    }
    // SEI
    // Bit mask:
    // 1001 0100 0111 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9478 )
    {
        execute_SEI();
    }
    // SEN
    // Bit mask:
    // 1001 0100 0010 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9428 )
    {
        execute_SEN();
    }
    // SER
    // Bit mask:
    // 1110 1111 dddd 1111
    else if ( ( cmd16Bits & 0xff0f ) == 0xef0f )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_SER( Rd );
    }
    // SES
    // Bit mask:
    // 1001 0100 0100 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9448 )
    {
        execute_SES();
    }
    // SET
    // Bit mask:
    // 1001 0100 0110 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9468 )
    {
        execute_SET();
    }
    // SEV
    // Bit mask:
    // 1001 0100 0011 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9438 )
    {
        execute_SEV();
    }
    // SEZ
    // Bit mask:
    // 1001 0100 0001 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9418 )
    {
        execute_SEZ();
    }
    // SLEEP
    // Bit mask:
    // 1001 0101 1000 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x9588 )
    {
        execute_SLEEP();
    }
    // SPM
    // Bit mask:
    // 1001 0101 1110 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x95e8 )
    {
        execute_SPM();
    }
    // STX1
    // Bit mask:
    // 1001 001r rrrr 1100
    else if ( ( cmd16Bits & 0xfe0f ) == 0x920c )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STX1( Rr );
    }
    // STX2
    // Bit mask:
    // 1001 001r rrrr 1101
    else if ( ( cmd16Bits & 0xfe0f ) == 0x920d )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STX2( Rr );
    }
    // STX3
    // Bit mask:
    // 1001 001r rrrr 1110
    else if ( ( cmd16Bits & 0xfe0f ) == 0x920e )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STX3( Rr );
    }
    // STY1
    // Bit mask:
    // 1000 001r rrrr 1000
    else if ( ( cmd16Bits & 0xfe0f ) == 0x8208 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STY1( Rr );
    }
    // STY2
    // Bit mask:
    // 1001 001r rrrr 1001
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9209 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STY2( Rr );
    }
    // STY3
    // Bit mask:
    // 1001 001r rrrr 1010
    else if ( ( cmd16Bits & 0xfe0f ) == 0x920a )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STY3( Rr );
    }
    // STY4
    // Bit mask:
    // 10q0 qq1r rrrr 1qqq
    else if ( ( cmd16Bits & 0xd208 ) == 0x8208 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 q = 0;
        q = ((cmd16Bits & (1 << 13))?(1<<5):0) |
                ((cmd16Bits & (1 << 11))?(1<<4):0) |
                ((cmd16Bits & (1 << 10))?(1<<3):0) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_STY4( Rr, q );
    }
    // STZ1
    // Bit mask:
    // 1000 001r rrrr 0000
    else if ( ( cmd16Bits & 0xfe0f ) == 0x8200 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STZ1( Rr );
    }
    // STZ2
    // Bit mask:
    // 1001 001r rrrr 0001
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9201 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STZ2( Rr );
    }
    // STZ3
    // Bit mask:
    // 1001 001r rrrr 0010
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9202 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_STZ3( Rr );
    }
    // STZ4
    // Bit mask:
    // 10q0 qq1r rrrr 0qqq
    else if ( ( cmd16Bits & 0xd208 ) == 0x8200 )
    {
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 q = 0;
        q = ((cmd16Bits & (1 << 13))?(1<<5):0) |
                ((cmd16Bits & (1 << 11))?(1<<4):0) |
                ((cmd16Bits & (1 << 10))?(1<<3):0) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_STZ4( Rr, q );
    }
    // STS
    // Bit mask:
    // 1001 001d dddd 0000 kkkk kkkk kkkk kkkk
    else if ( ( cmd32Bits & 0xFE0F0000L ) ==
                0x92000000L )
    {
        u16 k = 0;
        k = (cmd32Bits & (1 << 15)) |
                (cmd32Bits & (1 << 14)) |
                (cmd32Bits & (1 << 13)) |
                (cmd32Bits & (1 << 12)) |
                (cmd32Bits & (1 << 11)) |
                (cmd32Bits & (1 << 10)) |
                (cmd32Bits & (1 << 9)) |
                (cmd32Bits & (1 << 8)) |
                (cmd32Bits & (1 << 7)) |
                (cmd32Bits & (1 << 6)) |
                (cmd32Bits & (1 << 5)) |
                (cmd32Bits & (1 << 4)) |
                (cmd32Bits & (1 << 3)) |
                (cmd32Bits & (1 << 2)) |
                (cmd32Bits & (1 << 1)) |
                (cmd32Bits & (1 << 0)) ;
        u08 Rd = 0;
        Rd = ((cmd32Bits & (1 << 24))?(1<<4):0) |
                ((cmd32Bits & (1 << 23))?(1<<3):0) |
                ((cmd32Bits & (1 << 22))?(1<<2):0) |
                ((cmd32Bits & (1 << 21))?(1<<1):0) |
                ((cmd32Bits & (1 << 20))?(1<<0):0) ;

        execute_STS( k, Rd );
    }
    // SUB
    // Bit mask:
    // 0001 10rd dddd rrrr
    else if ( ( cmd16Bits & 0xfc00 ) == 0x1800 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        u08 Rr = 0;
        Rr = ((cmd16Bits & (1 << 9))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SUB( Rd, Rr );
    }
    // SUBI
    // Bit mask:
    // 0101 KKKK dddd KKKK
    else if ( ( cmd16Bits & 0xf000 ) == 0x5000 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;
        Rd += 16;
        u08 K = 0;
        K = ((cmd16Bits & (1 << 11))?(1<<7):0) |
                ((cmd16Bits & (1 << 10))?(1<<6):0) |
                ((cmd16Bits & (1 << 9))?(1<<5):0) |
                ((cmd16Bits & (1 << 8))?(1<<4):0) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_SUBI( Rd, K );
    }
    // SWAP
    // Bit mask:
    // 1001 010d dddd 0010
    else if ( ( cmd16Bits & 0xfe0f ) == 0x9402 )
    {
        u08 Rd = 0;
        Rd = ((cmd16Bits & (1 << 8))?(1<<4):0) |
                ((cmd16Bits & (1 << 7))?(1<<3):0) |
                ((cmd16Bits & (1 << 6))?(1<<2):0) |
                ((cmd16Bits & (1 << 5))?(1<<1):0) |
                ((cmd16Bits & (1 << 4))?(1<<0):0) ;

        execute_SWAP( Rd );
    }
    // TST
    // Bit mask:
    // 0010 00dd dddd dddd
    else if ( ( cmd16Bits & 0xfc00 ) == 0x2000 )
    {
        u16 Rd = 0;
        Rd = (cmd16Bits & (1 << 9)) |
                (cmd16Bits & (1 << 8)) |
                (cmd16Bits & (1 << 7)) |
                (cmd16Bits & (1 << 6)) |
                (cmd16Bits & (1 << 5)) |
                (cmd16Bits & (1 << 4)) |
                (cmd16Bits & (1 << 3)) |
                (cmd16Bits & (1 << 2)) |
                (cmd16Bits & (1 << 1)) |
                (cmd16Bits & (1 << 0)) ;

        execute_TST( Rd );
    }
    // WDR
    // Bit mask:
    // 1001 0101 1010 1000
    else if ( ( cmd16Bits & 0xffff ) == 0x95a8 )
    {
        execute_WDR();
    }
    else
    {
        /// @todo Warn that there is an unhandled command.
    }
    // Check if the next command may be fired:
    if ( m_state == STATE_CMD_FINISHED )
    // If so, tell the debugger that the last command has finished:
        m_parent->slotStepFinished();
}


unsigned int KLAVRCPU::get16BitCommandWordStartingAt( unsigned int location )
{
    return m_parent->flash().read( location+1 )<<8 |
           m_parent->flash().read( location );
}


unsigned int KLAVRCPU::get32BitCommandWordStartingAt( unsigned int location )
{
    return  (get16BitCommandWordStartingAt(location) << 16) |
            (m_parent->flash().read( location+3 )<<8 | m_parent->flash().read( location+2 ) );
}


bool KLAVRCPU::isTwoWordInstruction( unsigned int adr )
{
    unsigned int cmd32Bits = get32BitCommandWordStartingAt( adr );
    // CALL
    if ( ( cmd32Bits & 0xFE0E0000L ) ==
           0x940E0000L )
        return true;
    // JMP
    else if ( ( cmd32Bits & 0xFE0E0000L ) ==
                0x940C0000L )
        return true;
    // LDS
    else if ( ( cmd32Bits & 0xFE0F0000L ) ==
                0x90000000L )
        return true;
    // STS
    else if ( ( cmd32Bits & 0xFE0F0000L ) ==
                0x92000000L )
        return true;
    else
        return false;
}

