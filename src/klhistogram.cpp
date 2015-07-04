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
#include "klhistogram.h"
#include <qpixmap.h>
#include <qpainter.h>
#include <math.h>
#include <kapplication.h>
#include <qclipboard.h>
#include <klocale.h>
#include <QMouseEvent>


KLHistogram::KLHistogram( QWidget * parent, const QString & name,
                          int greatest, int lowest,
                          bool mode8Bit, bool bigEndian,
                          const vector< int >& data )
    : KLDataDisplay( parent, name, mode8Bit, bigEndian, data )
{
    setMouseTracking( true );
    m_currentMouseX = -1;
    m_lowest = lowest;
    m_greatest = greatest;
}


KLHistogram::~KLHistogram()
{
}

void KLHistogram::repaintBaseErasePixmap( const QSize & sizeOfPix )
{
    QPixmap pix( sizeOfPix );
    pix.fill( Qt::white );

    QPainter p( &pix );
    m_erasePixmap = pix;
}

void KLHistogram::updateDisplay( )
{
    if ( m_erasePixmap.size().height() * m_erasePixmap.size().width() == 0 )
        return;
    int range = m_greatest-m_lowest;
    QPixmap pix( m_erasePixmap );
    unsigned int width = pix.width();
    unsigned int height = pix.height();

    vector<int> buffer( width, 0 );
    double max = 0.0;

    int increment = m_8BitMode ? 1 : 2;
    
    for ( unsigned int i=0; i+1 < m_data.size(); i+= increment )
    {
        unsigned int from, to;
        int val = m_data[i];
        if ( !m_8BitMode )
        {
            if ( m_bigEndian )
                val = m_data[i] + 256* m_data[i+1];
            else
                val = m_data[i] * 256 + m_data[i+1];
        }
        if ( val > 32768 ) val = -65535 + val;
        mapping( val, from, to, width );
        if ( from >= width ) from = width-1;
        if ( to >= width ) to = width-1;
        for ( unsigned int j=from; j<=to; j++ )
        {
            buffer[j]++;
            if ( max < buffer[j] )
                max = buffer[j];
        }
    }
    QPolygon pa( width*2 );
    unsigned int cnt = 0;
    unsigned int yBottom = height;
    unsigned int bottomMargin = 15, topMargin = 20;
    if ( height > bottomMargin + topMargin ) yBottom = height - bottomMargin - topMargin;
    double scale = 1.0;
    scale = ( yBottom - bottomMargin - topMargin ) / max;
    if ( scale < 0.1 )
        scale = 0.1;

    int mouseMaxVal = -1;
    
    for ( unsigned int i=0; i < width; i++ )
    {
        if (buffer[i] > 0)
        {
            if ( m_currentMouseX == (int) i )
            {
                if ( mouseMaxVal < buffer[i] )
                    mouseMaxVal = buffer[i];
            }
            pa.setPoint( cnt, i, yBottom );
            cnt++;
            pa.setPoint( cnt, i, (int) (yBottom - buffer[i] * scale) );
            cnt++;
        }
    }
    QPainter painter( &pix );
    painter.drawPolyline(pa);

    if ( m_currentMouseX > 0 && m_currentMouseX < (int) width )
    {
        int triSize = 3;
        int mouseY = (int) (yBottom - mouseMaxVal * scale - triSize);
        pa.resize( 3 );
        pa.setPoint( 0, m_currentMouseX, mouseY );
        pa.setPoint( 1, m_currentMouseX - triSize, mouseY - triSize );
        pa.setPoint( 2, m_currentMouseX + triSize, mouseY - triSize );

        painter.setPen( Qt::green );
        painter.drawPolygon( pa );
        painter.setPen( Qt::black );
        painter.setBrush( Qt::white );
        QString text = QString(i18n("N(%1+-%2)=%3")).arg( ((int)(((double)m_currentMouseX) / width * range)) + m_lowest )
                .arg( (int) ceil(((double)range)/width) )
                .arg( buffer[ m_currentMouseX ] );
        QRect bRectOrig = fontMetrics().boundingRect( text );
        QRect bRect( bRectOrig );
        bRect.setCoords( -5, -5, 5, 5 );
        bRect.moveCenter( QPoint( m_currentMouseX, mouseY - triSize * 3 - bRect.height() / 2 ) );
        if (bRect.right() >= (int) width)
            bRect.moveCenter( bRect.center() - QPoint(bRect.right() - width + 1, 0) );
        if (bRect.left() < 0)
            bRect.moveCenter( bRect.center() - QPoint(bRect.left(), 0) );
        painter.drawRoundRect( bRect, 25, 50 );
        painter.drawText( bRect.left() + (bRect.width() - bRectOrig.width()) / 2,
                          bRect.bottom() + (bRectOrig.height() - bRect.height()) / 2 - fontMetrics().descent(), text );
    }

    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pix));
    setPalette(palette);
}

void KLHistogram::mapping( int val, unsigned int & from, unsigned int & to, unsigned int width )
{
    int range = m_greatest-m_lowest;
    if ( range == 0 )
        return;

    from = (int) floor(((double) val - m_lowest) / ((double) range) * width);
    to = (int) floor(((double) (val+1) - m_lowest) / ((double) range) * width);
    if (to > 0 )
        to--;
    if ( to >= width )
        to = width-1;
    if ( to < from ) to = from;
}

void KLHistogram::mouseMoveEvent( QMouseEvent * e )
{
    m_currentMouseX = e->x();
    if ( (e->y() >= height()) || (e->y() < 0) )
        m_currentMouseX = -1;
    updateDisplay();
}


