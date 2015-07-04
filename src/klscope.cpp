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
#include "klscope.h"
#include <qpixmap.h>
#include <qpainter.h>
#include <math.h>
#include <kapplication.h>
#include <qclipboard.h>
#include <QPolygon>


KLScope::KLScope( QWidget* parent, const QString& name, int top, int bottom,
                  bool mode8Bit, bool bigEndian,
                  const vector< int >& data, const QColor& graphColor )
    : KLDataDisplay( parent, name, mode8Bit, bigEndian, data ),
                     m_graphColor( graphColor )
{
    repaintBaseErasePixmap( QSize( 100, 100 ) );
    //TODO setErasePixmap( m_erasePixmap );
    m_greatest = top;
    m_lowest = bottom;
}


KLScope::~KLScope()
{
}


void KLScope::drawData( QPixmap & on )
{
    if ( m_data.size() == 0 )
        return;
    vector< int > data;
    QPainter p( &on );
    p.setPen( m_graphColor );


    if ( m_8BitMode )
        data = m_data;
    else
    {
        unsigned int end = m_data.size() / 2;
        for( unsigned int i=0; i < end; i++ )
        {
            int sta = m_data[i*2];
            int end = m_data[i*2 + 1];

            int val;
            if (m_bigEndian)
                val = sta + end * 256;
            else
                val = sta * 256 + end;
            if ( val > 32768 ) val = -65535 + val;
            data.push_back( val );
        }
    }

    unsigned int height = on.height();
    unsigned int width = on.width();
    unsigned int pixelsPerDiv = 25;
    unsigned int divs = height / pixelsPerDiv;
    if ( divs < 1 ) divs = 1;
    double unitsPerDiv = (1.0 * (m_greatest - m_lowest)) / divs;

    int start = data.size() - width;
    if ( start < 0 ) start = 0;
    QPolygon pa;
    pa.resize( data.size() - start );
    unsigned int cntr=0;
    unsigned int end = data.size();
    for ( unsigned int i=start; i < end; i++ )
    {
        int value = data[ i ];
        pa.setPoint( cntr, i-start, (int) (height - (value - m_lowest) / unitsPerDiv * pixelsPerDiv) );
        cntr++;
    }
    p.drawPolyline( pa );
}


void KLScope::repaintBaseErasePixmap( const QSize& sizeOfPix )
{
    QPixmap pix( sizeOfPix );
    pix.fill( Qt::white );

    QPainter p( &pix );
    unsigned int height = sizeOfPix.height();
    unsigned int width = sizeOfPix.width();
    unsigned int pixelsPerDiv = 25;
    unsigned int divs = height / pixelsPerDiv;
    if ( divs < 1 ) divs = 1;
    double unitsPerDiv = (1.0 * (m_greatest - m_lowest)) / divs;

    p.setPen( Qt::gray );
    for ( unsigned int div=0; div <= divs; div++ )
    {
        int yPos = height - div*pixelsPerDiv - 1;
        p.drawLine( 0, yPos, width, yPos );
        int val = (int) (div*unitsPerDiv + m_lowest);
        if ( div == divs )
            val = m_greatest;
        QString text = QString("%1").arg( val );
        QRect brect = p.fontMetrics().boundingRect( text );
        if ( yPos - p.fontMetrics().ascent() < 0 )
            yPos += p.fontMetrics().ascent();
        p.drawText( width - 10 - brect.width(), yPos, text );
    }
    m_erasePixmap = pix;
}


void KLScope::setGraphColor(const QColor& theValue)
{
    m_graphColor = theValue;
    updateDisplay();
}


void KLScope::updateDisplay( )
{
    if ( m_erasePixmap.size().height() * m_erasePixmap.size().width() == 0 )
        return;
    QPixmap pix( m_erasePixmap );
    drawData( pix );
    //TODO setErasePixmap( pix );
}


void KLScope::copyToClipboard( )
{
    QPixmap pix( m_erasePixmap );
    drawData( pix );
    KApplication::clipboard()->setPixmap( pix );
}


void KLScope::setYTop(const int& theValue)
{
    m_greatest = theValue;
    updateDisplay();
}


void KLScope::setYBottom( const int & theValue )
{
    m_lowest = theValue;
    updateDisplay();
}
