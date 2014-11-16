/***************************************************************************
 *   Copyright (C) 2007 by Martin Strasser                                 *
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
#include "kldatadisplay.h"
#include <assert.h>


KLDataDisplay::KLDataDisplay( QWidget * parent, const QString & name,
                              bool mode8Bit, bool bigEndian,
                              const vector< int >& data )
    : QWidget( parent, name ), m_8BitMode( mode8Bit ), m_bigEndian( bigEndian ),
      m_data( data )
{
    m_maxDataLength = 8192;
}


KLDataDisplay::~KLDataDisplay()
{
}


void KLDataDisplay::addData( vector< int > dat )
{
    for ( unsigned int i=0; i < dat.size(); i++ )
        m_data.push_back( dat[i] );

    if ( m_data.size() > m_maxDataLength )
    {
        vector< int > newData( m_maxDataLength );
        unsigned cnt = 0;
        for ( unsigned int i=m_data.size() - m_maxDataLength; i < m_data.size(); i++ )
        {
            newData[cnt] = m_data[i];
            cnt++;
        }
        m_data = newData;
    }
    int lowest;
    int greatest;
    if ( m_data.size() > 0 )
    {
        lowest = greatest = m_data[ 0 ];
    }
    for ( unsigned int i=0; i < m_data.size(); i++ )
    {
        if ( m_data[ i ] > greatest )
            greatest = m_data[ i ];
        if ( m_data[ i ] < lowest )
            lowest = m_data[ i ];
    }
    if ( ( m_lowest != lowest ) || ( m_greatest != greatest ) )
    {
        m_lowest = lowest;
        m_greatest = greatest;
        QSize theSize = size();
        if ( theSize.width() == 0 || theSize.height() == 0 )
            theSize = QSize( 100, 100 );
        repaintBaseErasePixmap( theSize );
    }
    m_lowest = lowest;
    m_greatest = greatest;
    updateDisplay();
}


void KLDataDisplay::shiftPhase()
{
    if ( m_data.size() > 0 )
    {
        unsigned int sizeBefore = m_data.size();
        m_data.erase( m_data.begin() );
        assert( m_data.size() == sizeBefore-1 );
    }
    updateDisplay();
}


void KLDataDisplay::set8BitMode( bool val )
{
    m_8BitMode = val;
    updateDisplay();
}


void KLDataDisplay::set16BitMode( bool val )
{
    m_8BitMode = !val;
    updateDisplay();
}


void KLDataDisplay::setBigEndian( bool val )
{
    m_bigEndian = val;
    updateDisplay();
}


void KLDataDisplay::clearData( )
{
    m_data.clear();
    updateDisplay();
}


void KLDataDisplay::resizeEvent( QResizeEvent * e )
{
    //repaintBaseErasePixmap( e->size() );
    updateDisplay();
}



