/***************************************************************************
 *   Copyright (C) 2007 by Martin Strasser   *
 *   msr@regent.e-technik.tu-muenchen.de   *
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
#include "klmemorycelllistboxitem.h"
#include <qpainter.h>


KLMemoryCellListBoxItem::KLMemoryCellListBoxItem(int adr, int val,
                        const QString& name, QTreeWidget *listbox)
    : QTreeWidgetItem( listbox, QStringList() << hexToString(adr,4)
                                              << hexToString(val,2)
                                              << QString::number(val)
                                              << QChar(val)
                                              << binToString(val,8)
                                              << name)
{
    m_address = adr;
    m_value = val;
    m_name = name;
}


KLMemoryCellListBoxItem::KLMemoryCellListBoxItem( const KLMemoryCellListBoxItem & copyMe )
    : QTreeWidgetItem( copyMe.treeWidget() )
{
    m_address = copyMe.address();
    m_value = copyMe.value();
    m_name = copyMe.name();
}


KLMemoryCellListBoxItem::~KLMemoryCellListBoxItem()
{
}



/**
 * This routine paints the current list box item.
 * @param p The painter to use.
 */
/*
void KLMemoryCellListBoxItem::paint( QPainter * p )
{
    int left = 2;
    int startAdr = left;
    int startHex, startVal, startChar, startBin, startName;
    left += 10 + p->fontMetrics().width( hexToString( 0, 4 ) );
    startHex = left;
    left += 10 + p->fontMetrics().width( hexToString( 0, 2 ) );
    startVal = left;
    left += 10 + p->fontMetrics().width( QString::number( 255, 10 ) );
    startChar = left;
    left += 10 + p->fontMetrics().width( QChar( 65 ) );
    startBin = left;
    left += 10 + p->fontMetrics().width( binToString(0, 8) );
    startName = left;
    
    int bgR = 230, bgG = 230, bgB = 230;
    
    QColor col = QColor( (p->backgroundColor().red() + bgR) / 2,
                         (p->backgroundColor().green() + bgG) / 2,
                         (p->backgroundColor().blue() + bgB) / 2 );
    
    QPen oldPen = p->pen();
    
    p->setPen( Qt::NoPen );
    
    p->setBrush( col );
    p->drawRect( startHex - 5, 0, startVal - startHex, p->fontMetrics().height() );
    p->drawRect( startChar - 5, 0, startBin - startChar, p->fontMetrics().height() );
    
    p->setPen( oldPen );
    // Address
    p->drawText( startAdr, p->fontMetrics().ascent(), hexToString(m_address, 4) );
    // Hex
    p->drawText( startHex,
                 p->fontMetrics().ascent(), hexToString(m_value, 2) );
    // Dec
    p->drawText( startVal,
                 p->fontMetrics().ascent(), QString::number( m_value, 10 ) );
    // Char
    p->drawText( startChar,
                 p->fontMetrics().ascent(), QChar( m_value ) );
    // Binary
    p->drawText( startBin,
                 p->fontMetrics().ascent(), binToString(m_value, 8) );

    // Name
    QFont theFont = p->font();
    theFont.setBold( !theFont.bold() );
    p->setFont( theFont );
    p->drawText( startName,
                 p->fontMetrics().ascent(), m_name );
}
*/

/*
int KLMemoryCellListBoxItem::height(const QTreeWidget *lb ) const
{
    return lb->fontMetrics().height();
}

int KLMemoryCellListBoxItem::width(const QTreeWidget *lb ) const
{
    int wid = 2;
    wid += 10 + lb->fontMetrics().width( hexToString( 0, 4 ) );
    wid += 10 + lb->fontMetrics().width( hexToString( 0, 2 ) );
    wid += 10 + lb->fontMetrics().width( QString::number( 255, 10 ) );
    wid += 10 + lb->fontMetrics().width( QChar( 65 ) );
    wid += lb->fontMetrics().width( binToString(m_value, 8) );
    return wid;
}
*/
QString KLMemoryCellListBoxItem::hexToString( int val, int field ) const
{
    QString retVal, buffer = "";
    retVal = retVal.number( val, 16 );
    if ( retVal.length() < (unsigned int) field )
        buffer.fill(QChar('0'), field - retVal.length() );
    retVal = "0x" + buffer + retVal;
    return retVal;
}

QString KLMemoryCellListBoxItem::binToString( int val, int field ) const
{
    QString retVal, buffer = "";
    retVal = retVal.number( val, 2 );
    if ( retVal.length() < (unsigned int) field )
        buffer.fill(QChar('0'), field - retVal.length() );
    retVal = buffer + retVal;
    return retVal;
}


void KLMemoryCellListBoxItem::setDescription(const QString & tip)
{
    m_description = tip;
}

