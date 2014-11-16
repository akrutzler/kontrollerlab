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
#ifndef KLDEBUGGERMEMORYMAPPING_H
#define KLDEBUGGERMEMORYMAPPING_H

#include <QList>

/**
This class stores the mapping of the RAM.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLDebuggerMemoryMapping{
public:
    KLDebuggerMemoryMapping();
    KLDebuggerMemoryMapping(unsigned int from, unsigned int to);

    bool operator<( const KLDebuggerMemoryMapping& other ) const;

    void setFrom(unsigned int theValue) { m_from = theValue; }
    unsigned int from() const { return m_from; }
    void setTo(unsigned int theValue) { m_to = theValue; }
    unsigned int to() const { return m_to; }
    void printDebug();
    bool inRange( unsigned int val )
    {
        return (m_from <= val) && (val <= m_to);
    }
protected:
    unsigned int m_from, m_to;

};


class KLDebuggerMemoryMappingList : public QList< KLDebuggerMemoryMapping >
{
public:
    QString toString();
    void fromString( const QString& val );
    void printDebug();
    bool inRange( unsigned int val );
};

#endif
