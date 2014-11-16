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
#include "klavrram.h"
#include <klocale.h>
#include "kldebugger.h"
#include "kontrollerlab.h"


KLAVRRAM::KLAVRRAM(KLDebugger *parent, const char *name)
    : QObject(parent, name), m_ram( DEBUGGER_RAM_SIZE, 0 )
{
    m_parent = parent;


    m_nameToLocationMap.insert( "SREG", 0x5F );
}


KLAVRRAM::~KLAVRRAM()
{
}


unsigned int KLAVRRAM::readRAM( unsigned int location )
{
    emit( signalReadRAM( location, m_ram[ location ] ) );
    return m_ram[ location ];
}


unsigned int KLAVRRAM::readRAM( const QString& name )
{
    if ( nameExists( name ) )
        return readRAM( m_nameToLocationMap[ name ] );
    else
        qWarning( "READ: There is no RAM location %s.", name.ascii() );
    return 0;
}


void KLAVRRAM::writeRAM( unsigned int location, unsigned char value )
{
    m_ram[ location ] = value;
    emit( signalWriteRAM( location, value ) );
}


void KLAVRRAM::writeRAM( const QString& name, unsigned char value )
{
    if ( nameExists( name ) )
        writeRAM( m_nameToLocationMap[ name ], value );
    else
        qWarning( "WRITE: There is no RAM location %s.", name.ascii() );
}

bool KLAVRRAM::nameExists( const QString& name )
{
    if ( m_nameToLocationMap.find( name ) == m_nameToLocationMap.end() )
        return false;
    else
        return true;
}

void KLAVRRAM::writeRAMNoSignal( unsigned int location, unsigned char value )
{
    m_ram[ location ] = value;
}

unsigned int KLAVRRAM::readRAMNoSignal( unsigned int location )
{
    return m_ram[ location ];
}

void KLAVRRAM::clear( )
{
    // qDebug("clearing %d", m_ram.size());
    for ( unsigned int i=0; i<m_ram.size(); i++ )
    {
        m_ram[i] = 0;
    }
    m_parent->parent()->setAllMemoryViewValuesToZero();
}


