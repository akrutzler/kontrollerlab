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
#ifndef KLEDITDOTSWIDGET_H
#define KLEDITDOTSWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qobject.h>

/**
This is a class which is used to edit the dots of a character in a graphics display or as a user defined character in an alphanumeric dot matrix display.

	@author Martin Strasser <strasser    a t    cadmaniac    d o t    org>
*/
class KLEditDotsWidget : public QWidget
{
    Q_OBJECT
public:

    KLEditDotsWidget( int dotWidth, int dotHeight, QWidget *parent, QString name="" );

    ~KLEditDotsWidget();

    void setDot( int x, int y, int val )
    {
        if ( x < 0 || x >= m_dotWidth || y < 0 || y >= m_dotHeight )
            return;
        m_dots[x][y] = val;
    }

    int dot( int x, int y ) const
    {
        if ( x < 0 || x >= m_dotWidth || y < 0 || y >= m_dotHeight )
            return -1;
        return m_dots[x][y];
    }
    void clear();
    void resizeDots( int newWidth, int newHeight );
    QList<int> get8BitMask() const;
    void setFrom8BitMask( QList<int> mask );
    QList<int> serializeHorizontally() const;
    QList<int> serializeVertically() const;
signals:
    void dotsChanged();
protected:
    void resizeEvent( QResizeEvent* ev );
    void paintEvent( QPaintEvent* ev );
    void mousePressEvent( QMouseEvent* ev );
    void mouseMoveEvent( QMouseEvent* ev );
    void mouseReleaseEvent( QMouseEvent* ev );
    void dragEnterEvent( QDragEnterEvent* ev );
    void dropEvent( QDropEvent* ev );
    void triggerRecreateErasePixmap();
    bool readFromPixmap( const QPixmap& pix );

    QPixmap m_erasePixmap;
    int m_dotWidth, m_dotHeight;
    QList< QList< int > > m_dots;
    QList< QList< int > > m_lastPaintedDots;
    int m_lastX, m_lastY;
};

#endif
