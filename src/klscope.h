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
#ifndef KLSCOPE_H
#define KLSCOPE_H

#include "kldatadisplay.h"
#include <qpixmap.h>
#include <qcolor.h>


/**
This is the scope for showing data scope-like.

	@author Martin Strasser <strasser  a t  cadmaniac  d o t  org>
*/
class KLScope : public KLDataDisplay
{
public:
    KLScope( QWidget* parent, const QString& name, int top = 255, int bottom = 0,
             bool mode8Bit = true, bool bigEndian = true,
             const vector< int >& data = vector< int >(),
             const QColor& graphColor = Qt::black );

    ~KLScope();

    void setGraphColor(const QColor& theValue);
    QColor graphColor() const { return m_graphColor; }

    void updateDisplay();
    void copyToClipboard();

    void setYTop(const int& theValue);
    int yTop() const { return m_greatest; }
    void setYBottom(const int& theValue);
    int yBottom() const { return m_lowest; }

protected:
    void drawData( QPixmap& on );
    void repaintBaseErasePixmap( const QSize& sizeOfPix );
    QPixmap m_erasePixmap;
    QColor m_graphColor;
};

#endif
