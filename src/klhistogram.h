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
#ifndef KLHISTOGRAM_H
#define KLHISTOGRAM_H

#include "kldatadisplay.h"
#include <qpixmap.h>
#include <qcolor.h>


/**
This is a class showing a histogram.

	@author Martin Strasser <strasser  a t  cadmaniac  d o t  org>
*/
class KLHistogram : public KLDataDisplay
{
public:
    KLHistogram( QWidget* parent, const QString& name,
                 int greatest = 255, int lowest=0,
                 bool mode8Bit = true, bool bigEndian = true,
                 const vector< int >& data = vector< int >() );

    ~KLHistogram();
protected:
    void repaintBaseErasePixmap( const QSize& sizeOfPix );
    virtual void updateDisplay();
    void mouseMoveEvent( QMouseEvent* e );
    void mapping( int val, unsigned int& from, unsigned int& to, unsigned int width );
    int m_currentMouseX;
};

#endif
