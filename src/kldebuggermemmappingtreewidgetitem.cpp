/***************************************************************************
 *   Copyright (C) 2006 by Martin Strasser                                 *
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
#include "kldebuggermemmappingtreewidgetitem.h"



KLDebuggerMemMappingTreeWidgetItem::KLDebuggerMemMappingTreeWidgetItem(QTreeWidget *lv, unsigned int from, unsigned int to )
 : QTreeWidgetItem( lv, QStringList() << QString("0x%1").arg( from, 0, 16) << QString("0x%1").arg( to, 0, 16) ),
 m_mapping( from, to )
{
}


KLDebuggerMemMappingTreeWidgetItem::~KLDebuggerMemMappingTreeWidgetItem()
{
}

void KLDebuggerMemMappingTreeWidgetItem::setFrom( unsigned int from )
{
    m_mapping.setFrom( from );
    setText( 0, QString("0x%1").arg( from, 0, 16) );
}


void KLDebuggerMemMappingTreeWidgetItem::setTo( unsigned int to )
{
    m_mapping.setTo( to );
    setText( 1, QString("0x%1").arg( to, 0, 16) );
}

bool KLDebuggerMemMappingTreeWidgetItem::operator<(const QTreeWidgetItem &other) const
{
    const KLDebuggerMemMappingTreeWidgetItem* mem = dynamic_cast< const KLDebuggerMemMappingTreeWidgetItem* >( &other );
    if ( !mem )
    {
        return QTreeWidgetItem::operator <(other);
    }
    return m_mapping < mem->mapping();
}


