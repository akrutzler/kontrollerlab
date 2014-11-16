/***************************************************************************
 *   Copyright (C) 2006 by Martin Strasser   *
 *   strasser    a t    cadmaniac    d o t    org   *
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
#include "kleditdotswidget.h"
#include <qpainter.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <q3dragobject.h>
#include <kurl.h>
#include <knotification.h>
#include <klocale.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <QResizeEvent>


KLEditDotsWidget::KLEditDotsWidget( int dotWidth, int dotHeight, QWidget *parent, QString name )
 : QWidget( parent, name )
{
/*    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(m_erasePixmap));
    setPalette(palette);*/
    setErasePixmap( m_erasePixmap );
    setMinimumSize( 100, 100 );
    m_erasePixmap.resize( 100, 100 );
    m_dotWidth = dotWidth;
    m_dotHeight = dotHeight;
    QList< int > row;
    for (int y=0; y<dotHeight; y++)
        row.append( 0 );
    for (int x=0; x<dotWidth; x++)
        m_dots.append( row );
    m_lastPaintedDots = m_dots;
    triggerRecreateErasePixmap();
    setAcceptDrops( true );
    // To reduce the CPU load, only redraw the picture
    // if the mouse was moved to the next dot:
    m_lastX = -1;
    m_lastY = -1;
}


KLEditDotsWidget::~KLEditDotsWidget()
{
}

void KLEditDotsWidget::resizeEvent( QResizeEvent * ev )
{
    m_erasePixmap.resize( ev->size() );
    triggerRecreateErasePixmap();
    setErasePixmap( m_erasePixmap );
}

void KLEditDotsWidget::triggerRecreateErasePixmap( )
{
    m_erasePixmap.fill( backgroundColor() );
    double pixWid = (width() * 1.0 / m_dotWidth);
    double pixHei = (height() * 1.0 / m_dotHeight);
    QPainter paint( &m_erasePixmap );
    for (int x=0; x < m_dotWidth; x++)
    {
        for (int y=0; y < m_dotHeight; y++)
        {
            int startX = (int) pixWid * x;
            int startY = (int) pixHei * y;
            if ( dot( x, y ) > 0 )
            {
                paint.setBrush( QBrush( foregroundColor(), Qt::SolidPattern ) );
            }
            else
            {
                paint.setBrush( QBrush( foregroundColor(), Qt::NoBrush ) );
            }
            paint.drawRect( startX+1, startY+1, (int) pixWid-2, (int) pixHei-2 );
        }
    }
    if ( m_dots != m_lastPaintedDots )
    {
        emit( dotsChanged() );
        m_lastPaintedDots = m_dots;
    }
}

void KLEditDotsWidget::paintEvent( QPaintEvent * )
{
}

void KLEditDotsWidget::mousePressEvent( QMouseEvent * ev )
{
    int pixelX = (int) (ev->x() / (width() * 1.0 / m_dotWidth));
    int pixelY = (int) (ev->y() / (height() * 1.0 / m_dotHeight));

    if ( ev->button() == Qt::LeftButton )
        setDot( pixelX, pixelY, !dot( pixelX, pixelY ) );
    else if ( ev->button() == Qt::RightButton )
        setDot( pixelX, pixelY, 0 );

    triggerRecreateErasePixmap();
    setErasePixmap( m_erasePixmap );
    m_lastX = pixelX;
    m_lastY = pixelY;
}


void KLEditDotsWidget::mouseMoveEvent( QMouseEvent * ev )
{
    int pixelX = (int) (ev->x() / (width() * 1.0 / m_dotWidth));
    int pixelY = (int) (ev->y() / (height() * 1.0 / m_dotHeight));
    if ( ( pixelX == m_lastX ) && ( pixelY == m_lastY ) )
        return;
    if ( ev->state() == Qt::LeftButton )
        setDot( pixelX, pixelY, 1 );
    else if ( ev->state() == Qt::RightButton )
        setDot( pixelX, pixelY, 0 );
    triggerRecreateErasePixmap();
    setErasePixmap( m_erasePixmap );
    m_lastX = pixelX;
    m_lastY = pixelY;
}


bool KLEditDotsWidget::readFromPixmap( const QPixmap & pix )
{
    if ((pix.width() != m_dotWidth) || (pix.height() != m_dotHeight))
    {
        return false;
    }

    QImage image=pix.convertToImage();

    for (int x=0; x<m_dotWidth; x++)
    {
        for (int y=0; y<m_dotHeight; y++)
        {
            QRgb pixel = image.pixel( x, y );
            int r = (pixel&0xff0000)>>16, g = (pixel&0xff00)>>8, b = pixel&0xff;
            if ( (r+g+b)/3 < 128 )
            {
                setDot( x, y, 1 );
            }
            else
            {
                setDot( x, y, 0 );
            }
        }
    }
    return true;
}

void KLEditDotsWidget::dropEvent( QDropEvent * ev )
{
    QPixmap pix;
    Q3StrList lst;
    
    if ( Q3ImageDrag::decode( ev, pix ) )
    {
        readFromPixmap( pix );
    }
    else if ( Q3UriDrag::decode( ev, lst ) )
    {
        if (lst.count() == 0 )
            return;

        KUrl url( lst.first() );
        QString localFileName;

        if ( url.isLocalFile() )
            localFileName = url.path();
        else if (!KIO::NetAccess::download( url, localFileName, this ))
        {
            KNotification::event( KNotification::Error, i18n("Could not download the file.") );
            return;
        }

        QFileInfo fi( localFileName );
        if ( !fi.exists() )
        {
            KIO::NetAccess::removeTempFile(localFileName);
            return;
        }
        pix.load( localFileName );
        if ( !readFromPixmap( pix ) )
        {
            KMessageBox::information( this, i18n( "The image You dropped is %1 x %2, which doesn't "
                                                  "fit the selected size of %3 x %4.")
                                                          .arg(pix.width()).arg(pix.height())
                                    .arg(m_dotWidth).arg(m_dotHeight) );
        }
        else
        {
            triggerRecreateErasePixmap();
            setErasePixmap( m_erasePixmap );
        }
        KIO::NetAccess::removeTempFile(localFileName);
    }
}

void KLEditDotsWidget::dragEnterEvent( QDragEnterEvent * ev )
{
    ev->accept( Q3ImageDrag::canDecode( ev ) || Q3UriDrag::canDecode( ev ) );
}

QList< int > KLEditDotsWidget::get8BitMask( ) const
{
    QList< int > retVal;

    int current=0;
    for (int y=0; y<m_dotHeight;y++)
    {
        current=0;
        for (int x=0; x<m_dotWidth; x++)
        {
            current <<= 1;
            if ( m_dots[x][y] > 0 )
                current |= 1;
        }
        retVal.append( current );
    }
    return retVal;
}

void KLEditDotsWidget::clear( )
{
    for (int y=0; y<m_dotHeight;y++)
    {
        for (int x=0; x<m_dotWidth; x++)
        {
            m_dots[x][y] = 0;
        }
    }
    triggerRecreateErasePixmap();
    setErasePixmap( m_erasePixmap );
}

void KLEditDotsWidget::setFrom8BitMask(QList<int> mask )
{
    int endX = m_dotWidth > 8? m_dotWidth-8 : 0;
    for (int y=0; y<(mask.count()<m_dotHeight?
         mask.count():m_dotHeight);y++)
    {
        for (int x=m_dotWidth-1; x>=endX; x--)
        {
            m_dots[x][y] = mask[y] & (1<<(m_dotWidth - x - 1)) ? 1 : 0;
        }
    }
    triggerRecreateErasePixmap();
    setErasePixmap( m_erasePixmap );
}


QList< int > KLEditDotsWidget::serializeHorizontally( ) const
{
    int currentBitCounter = 0;
    int current = 0;
    QList< int > retVal;

    for (int y=0; y<m_dotHeight; y++)
    {
        for (int x=0; x<m_dotWidth; x++)
        {
            current <<= 1;
            if (m_dots[x][y]>0)
                current |= 1;
            currentBitCounter++;
            if (currentBitCounter==8)
            {
                retVal.append( current );
                currentBitCounter = 0;
                current=0;
            }
        }
    }
    if ( currentBitCounter > 0 )
        retVal.append ( current );
    return retVal;
}


QList< int > KLEditDotsWidget::serializeVertically( ) const
{
    int currentBitCounter = 0;
    int current = 0;
    QList< int > retVal;

    for (int x=0; x<m_dotWidth; x++)
    {
        for (int y=0; y<m_dotHeight; y++)
        {
            current <<= 1;
            if (m_dots[x][y]>0)
                current |= 1;
            currentBitCounter++;
            if (currentBitCounter==8)
            {
                retVal.append( current );
                currentBitCounter = 0;
                current=0;
            }
        }
    }
    if ( currentBitCounter > 0 )
        retVal.append ( current );
    return retVal;
}


void KLEditDotsWidget::mouseReleaseEvent( QMouseEvent * )
{
    m_lastX = m_lastY = -1;
}


void KLEditDotsWidget::resizeDots( int newWidth, int newHeight )
{
    QList< QList< int > > newDots;

    QList< int > row;
    for (int y=0; y<newHeight; y++)
        row.append( 0 );
    for (int x=0; x<newWidth; x++)
        newDots.append( row );

    for (int y=0; y<(m_dotHeight<newHeight?m_dotHeight:newHeight);y++)
    {
        for (int x=0; x<(m_dotWidth<newWidth?m_dotWidth:newWidth); x++)
        {
            newDots[x][y] = m_dots[x][y];
        }
    }
    m_dotHeight = newHeight;
    m_dotWidth = newWidth;
    m_dots = newDots;
    triggerRecreateErasePixmap();
    setErasePixmap( m_erasePixmap );
}


