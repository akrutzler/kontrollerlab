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
#include "kldebuggermemorymapping.h"
#include <qstringlist.h>


KLDebuggerMemoryMapping::KLDebuggerMemoryMapping()
{
    m_from = m_to = 0;
}


KLDebuggerMemoryMapping::KLDebuggerMemoryMapping( unsigned int from, unsigned int to )
{
    m_from = from;
    m_to = to;
}

bool KLDebuggerMemoryMapping::operator <( const KLDebuggerMemoryMapping & other ) const
{
    return m_from < other.m_from;
}


QString KLDebuggerMemoryMappingList::toString( )
{
    QString mapping = "";
    for ( iterator it = begin(); it != end(); ++it )
    {
        QString fromStr = QString("0x%1").arg((*it).from(),0,16);
        QString toStr = QString("0x%1").arg((*it).to(),0,16);
        if ( mapping.length() != 0 )
            mapping.append( "," );
        mapping.append( fromStr + "," + toStr );
    }
    return mapping;
}


void KLDebuggerMemoryMappingList::fromString( const QString & val )
{
    QStringList lst = val.split(",",QString::SkipEmptyParts);
    for ( unsigned int i=0; i+1<lst.count(); i+=2 )
    {
        QString fromStr_ = lst[ i ].toLower();
        QString fromStr = fromStr_;
        fromStr = fromStr.replace( "0x", "" );
        QString toStr_ = lst[ i+1 ].toLower();
        QString toStr = toStr_;
        toStr = toStr.replace( "0x", "" );
        bool ok1, ok2;
        int from = fromStr.toInt( &ok1, 16 );
        int to = toStr.toInt( &ok2, 16 );
        if ( ok1 && ok2 )
        {
            append( KLDebuggerMemoryMapping( from, to ) );
        }
        else
            qWarning( "Internal error in %s:%d", __FILE__, __LINE__ );
    }
}

void KLDebuggerMemoryMappingList::printDebug( )
{
    qDebug("%s", "*************");
    for ( iterator it = begin(); it != end(); ++it )
    {
        (*it).printDebug();
    }
}

void KLDebuggerMemoryMapping::printDebug( )
{
    qDebug( "from %d to %d", from(), to() );
}

bool KLDebuggerMemoryMappingList::inRange( unsigned int val )
{
    for ( iterator it = begin(); it != end(); ++it )
    {
        if ( (*it).inRange( val ) )
            return true;
    }
    return false;
}

