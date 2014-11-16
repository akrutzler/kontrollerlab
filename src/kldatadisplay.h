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
#ifndef KLDATADISPLAY_H
#define KLDATADISPLAY_H

#include <qwidget.h>
#include <qpixmap.h>
#include <vector>

using std::vector;


/**
This is the base class for KLScope and KLHistogram which stores and handles data.

	@author Martin Strasser <strasser  a t  cadmaniac  d o t  org>
*/
class KLDataDisplay : public QWidget
{
public:
    KLDataDisplay( QWidget* parent, const QString& name,
                   bool mode8Bit = true, bool bigEndian = true,
                   const vector< int >& data = vector<int>() );

    virtual void addData( vector< int > dat );
    virtual void clearData();
    virtual void set16BitMode( bool val=true );
    virtual void set8BitMode( bool val=true );
    virtual void setBigEndian( bool val=true );
    virtual void shiftPhase();

    virtual ~KLDataDisplay();
protected:
    QPixmap m_erasePixmap;
    virtual void repaintBaseErasePixmap( const QSize& sizeOfPix ) = 0;
    virtual void updateDisplay() = 0;
    virtual void resizeEvent( QResizeEvent* e );
    unsigned int m_maxDataLength;
    bool m_8BitMode;
    bool m_bigEndian;
    int m_greatest, m_lowest;
    vector< int > m_data;
};

#endif
