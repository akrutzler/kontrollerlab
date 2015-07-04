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
#ifndef KLDEBUGGERMEMMAPPINGLISTVIEWITEM_H
#define KLDEBUGGERMEMMAPPINGLISTVIEWITEM_H

#include <QTreeWidgetItem>
#include "kldebuggermemorymapping.h"

/**
This class displays a list view item for the memory mapping and stores the according instance of KLDebuggerMemoryMapping.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLDebuggerMemMappingTreeWidgetItem : public QTreeWidgetItem
{
public:
    KLDebuggerMemMappingTreeWidgetItem(QTreeWidget* lv, unsigned int from, unsigned int to);

    ~KLDebuggerMemMappingTreeWidgetItem();

    void setMapping(const KLDebuggerMemoryMapping& theValue)
    { m_mapping = theValue; }
    KLDebuggerMemoryMapping mapping() const
    { return m_mapping; }

    void setFrom( unsigned int from );
    unsigned int from() const { return m_mapping.from(); }
    void setTo( unsigned int to );
    unsigned int to() const { return m_mapping.to(); }
protected:
    KLDebuggerMemoryMapping m_mapping;

private:
    bool operator<(const QTreeWidgetItem &other) const;
};

#endif
