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
#ifndef KLASMINSTRUCTION_H
#define KLASMINSTRUCTION_H

#include <QList>

/**
This class stores one ASM instruction and the representing line in the disassembled code.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLASMInstruction{
public:
    KLASMInstruction();
    KLASMInstruction( unsigned int adr, QList< unsigned int > hexCmds, unsigned int asmLine );

    /**
     * @return A list of unsigned ints which form the assembler instruction.
     */
    QList< unsigned int > hexCmds() const
    { return m_hexCmds; }
    /**
     * @return The address of the assembler command.
     */
    unsigned int address() const
    { return m_address; }
    /**
     * @return The line in the file that causes this command.
     */
    unsigned int asmLine() const
    { return m_asmLine; }

protected:
    QList< unsigned int > m_hexCmds;
    unsigned int m_asmLine;
    unsigned int m_address;
};

#endif
