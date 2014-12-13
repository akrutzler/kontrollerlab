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
#ifndef KLSOURCECODETOASMRELATION_H
#define KLSOURCECODETOASMRELATION_H

#include <qstring.h>

/**
This class stores the information which ASM command is related with which line of code in which file.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLSourceCodeToASMRelation{
public:
    KLSourceCodeToASMRelation();
    KLSourceCodeToASMRelation( const QString& sourceFilename, unsigned int line,
                               unsigned int asmCommandAddress );

    ~KLSourceCodeToASMRelation();

    void setSourceFilename(const QString& theValue)
    { m_sourceFilename = theValue; }
    QString sourceFilename() const { return m_sourceFilename; }
    void setAsmCommandAddress(unsigned int theValue)
    { m_asmCommandAddress = theValue; }
    unsigned int asmCommandAddress() const { return m_asmCommandAddress; }
    void setLine(unsigned int theValue) { m_line = theValue; }
    unsigned int line() const { return m_line; }
    
    // bool operator < (const KLSourceCodeToASMRelation& other) {return m_line < other.line(); }
    bool operator < (const KLSourceCodeToASMRelation& other) const
    {
        if ( m_sortByASMAdress )
            return m_asmCommandAddress < other.asmCommandAddress();
        else
            return m_line < other.line();
    }

    void setSortByASMAdress(bool theValue)
    { m_sortByASMAdress = theValue; }
    bool sortByASMAdress() const
    { return m_sortByASMAdress; }

protected:
    bool m_sortByASMAdress;
    QString m_sourceFilename;
    unsigned int m_line;
    unsigned int m_asmCommandAddress;
};

#endif
