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
#ifndef KLMEMORYCELLLISTBOXITEM_H
#define KLMEMORYCELLLISTBOXITEM_H


#include <QTreeWidget>


/**
This is the class for a single list box item storing a memory cell.

	@author Martin Strasser <strasser  a t  cadmaniac  d o t  org>
*/
class KLMemoryCellListBoxItem : public QTreeWidgetItem
{
public:
    KLMemoryCellListBoxItem(int adr, int val,
                            const QString& name, QTreeWidget *listbox = 0);
    KLMemoryCellListBoxItem( const KLMemoryCellListBoxItem& copyMe );
    ~KLMemoryCellListBoxItem();

    void setDescription( const QString& tip );
    QString description() const { return m_description; }
    //void paint( QPainter * p );
    //int height ( const QTreeWidget * lb ) const;
    //int width ( const QTreeWidget * lb ) const;

    void setValue(const int& theValue)
    {
        if(m_value == theValue)
            return;

        m_value = theValue;
        setText(1,hexToString(theValue,2));
        setText(2,QString::number(theValue));
        //setText(3,QChar(theValue));
        setText(4,binToString(theValue,8));
    }
    int value() const { return m_value; }
    void setAddress(const int& theValue) { m_address = theValue; }
    int address() const { return m_address; }
    void setName(const QString& theValue)
    {
        if(m_name == theValue)
            return;
        m_name = theValue;
        setText(5,theValue);
    }
    QString name() const { return m_name; }

protected:
    int m_value;
    int m_address;
    QString m_name, m_description;
    QString hexToString( int val, int field ) const;
    QString binToString( int val, int field ) const;
};

#endif
