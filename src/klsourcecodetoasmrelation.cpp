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
#include "klsourcecodetoasmrelation.h"

KLSourceCodeToASMRelation::KLSourceCodeToASMRelation( const QString& sourceFilename, unsigned int line,
                                                      unsigned int asmCommandAddress)
{
    // The asm address where the command starts:
    m_asmCommandAddress = asmCommandAddress;
    m_line = line;
    m_sourceFilename = sourceFilename;
    m_sortByASMAdress = true;
}


KLSourceCodeToASMRelation::~KLSourceCodeToASMRelation()
{
}

KLSourceCodeToASMRelation::KLSourceCodeToASMRelation( )
{
    m_asmCommandAddress = m_line = 0;
    m_sourceFilename = ""; // UNINITIALIZED!
    m_sortByASMAdress = true;
}


