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
#include "kldebuggermemmappinglistviewitem.h"



KLDebuggerMemMappingListViewItem::KLDebuggerMemMappingListViewItem( Q3ListView * lv, unsigned int from, unsigned int to )
 : Q3ListViewItem( lv, "0x" + QString::number( from, 16 ), "0x" + QString::number( to, 16 ) ),
 m_mapping( from, to )
{
}


KLDebuggerMemMappingListViewItem::~KLDebuggerMemMappingListViewItem()
{
}

int KLDebuggerMemMappingListViewItem::compare( Q3ListViewItem * i, int col, bool ascending ) const
{
    KLDebuggerMemMappingListViewItem* mem = dynamic_cast< KLDebuggerMemMappingListViewItem* >( i );
    if ( !mem )
    {
        return Q3ListViewItem::compare( i, col, ascending );
    }
    return m_mapping < mem->mapping() ? -1 : 1;
}


void KLDebuggerMemMappingListViewItem::setFrom( unsigned int from )
{
    m_mapping.setFrom( from );
    setText( 0, "0x" + QString::number( from, 16 ) );
}


void KLDebuggerMemMappingListViewItem::setTo( unsigned int to )
{
    m_mapping.setTo( to );
    setText( 1, "0x" + QString::number( to, 16 ) );
}


