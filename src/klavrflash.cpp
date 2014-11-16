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
#include "klavrflash.h"
#include "kldebugger.h"

KLAVRFlash::KLAVRFlash(KLDebugger *parent, const char *name)
 : QObject(parent)
{
    m_parent = parent;
    resize( 1024 );
}


KLAVRFlash::~KLAVRFlash()
{
}

void KLAVRFlash::write( unsigned int location, unsigned int value )
{
    m_flash[ location ] = value;
}

unsigned int KLAVRFlash::read( unsigned int location )
{
    return m_flash[ location ];
}

void KLAVRFlash::printDebug( ) const
{
    qDebug("m_flash.size() == %d", m_flash.size() );
    for (unsigned int adr=0; adr<m_flash.size(); adr+=16)
    {
        QString out = QString::number( adr, 16 ) + ": ";
        for ( unsigned int off=0; off<16; off++ )
        {
            out += QString::number( m_flash[ adr+off ], 16 ) + " ";
        }
        qDebug() << out;
    }
}


void KLAVRFlash::resize( unsigned int newSize )
{
    m_flash = std::vector< unsigned int >( newSize, 0xff);
    m_flashAddressToASMLine = std::vector< unsigned int >( newSize, 0 );
    m_lineInSrc = std::vector< int >( newSize, 0 );
    m_sourceNr = std::vector< int >( newSize, 0L );
}

