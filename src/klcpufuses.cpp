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
#include "klcpufuses.h"
#include <klocale.h>
#include <qlist.h>

#define TRUE_STRING "TRUE"
#define FALSE_STRING "FALSE"

KLCPUFuses::KLCPUFuses()
{
    QStringList empty;
    empty << i18n("Bit 0") << i18n("Bit 1") << i18n("Bit 2") << i18n("Bit 3")
          << i18n("Bit 4") << i18n("Bit 5") << i18n("Bit 6") << i18n("Bit 7");
    m_lowNames = m_highNames = m_extNames = QStringList( empty );

    QList<bool> empty2;
    empty2 << true << true << true << true
           << true << true << true << true;
    m_lowCanBeChanged = m_highCanBeChanged = m_extCanBeChanged = empty2;
}


KLCPUFuses::~KLCPUFuses()
{
}

KLCPUFuses::KLCPUFuses(const QString & mcuName,
                        QList<bool> lowCanBeChanged,
                        QList<bool> highCanBeChanged,
                        QList<bool> extCanBeChanged,
                        QStringList lowNames,
                        QStringList highNames,
                        QStringList extNames )
{
    m_lowNames = lowNames;
    m_highNames = highNames;
    m_extNames = extNames;
    m_lowCanBeChanged = lowCanBeChanged;
    m_highCanBeChanged = highCanBeChanged;
    m_extCanBeChanged = extCanBeChanged;
    m_mcuName = mcuName;
}


KLCPUFuses::KLCPUFuses(QDomDocument &, QDomElement & parent)
{
    if ( parent.nodeName().toUpper() == "FUSES" )
    {
        for( QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling() )
        {
            if ( n.isElement() )
            {
                QDomElement ele = n.toElement();
                if ( n.nodeName().toUpper() == "LOW_CAN_BE_CHANGED" )
                    m_lowCanBeChanged = stringToBoolValList( ele.text() );
                else if ( n.nodeName().toUpper() == "LOW_NAMES" )
                {
                    m_lowNames = ele.text().split(",");
                }
                else if ( n.nodeName().toUpper() == "HIGH_CAN_BE_CHANGED" )
                    m_highCanBeChanged = stringToBoolValList( ele.text() );
                else if ( n.nodeName().toUpper() == "HIGH_NAMES" )
                {

                    m_highNames = ele.text().split(",");
                }
                else if ( n.nodeName().toUpper() == "EXT_CAN_BE_CHANGED" )
                    m_extCanBeChanged = stringToBoolValList( ele.text() );
                else if ( n.nodeName().toUpper() == "EXT_NAMES" )
                {
                    m_extNames = ele.text().split(",");;
                }
            }
        }
    }
}


void KLCPUFuses::createDOMElement(QDomDocument & document, QDomElement & parent)
{
    QDomElement fuse = document.createElement( "FUSES" );

    createAndAddDOM( document, fuse, "LOW_CAN_BE_CHANGED",
                     boolValListToString( m_lowCanBeChanged ) );
    createAndAddDOM( document, fuse, "LOW_NAMES",
                     m_lowNames.join(", ") );
    createAndAddDOM( document, fuse, "HIGH_CAN_BE_CHANGED",
                     boolValListToString( m_highCanBeChanged ) );
    createAndAddDOM( document, fuse, "HIGH_NAMES",
                     m_highNames.join(", ") );
    createAndAddDOM( document, fuse, "EXT_CAN_BE_CHANGED",
                     boolValListToString( m_extCanBeChanged ) );
    createAndAddDOM( document, fuse, "EXT_NAMES",
                     m_extNames.join(", ") );

    parent.appendChild( fuse );
}


void KLCPUFusesList::readFromDOMElement(QDomDocument &, QDomElement &)
{
    // Not to be implemented -> not needed at the moment.
}


void KLCPUFusesList::createDOMElement(QDomDocument & document, QDomElement & parent)
{
    QList<KLCPUFuses>::iterator it;
    for (it=begin(); it!=end(); ++it)
    {
        (*it).createDOMElement( document, parent );
    }
}

void KLCPUFuses::createAndAddDOM(QDomDocument & document, QDomElement & fuse,
                                 const QString & name, const QString & text)
{
    QDomElement cur = document.createElement( name );
    QDomText textDOM = document.createTextNode( text );
    cur.appendChild( textDOM );
    fuse.appendChild( cur );
}


QList< bool > KLCPUFuses::stringToBoolValList(const QString & boolList) const
{
    QStringList list;
    QList< bool > retVal;

    list =  boolList.split(",");
    for (QStringList::iterator it = list.begin(); it != list.end(); ++it)
    {
        if ( (*it).trimmed().length() != 0 )
            retVal.append( (*it).trimmed().toUpper() == TRUE_STRING );
    }
    return retVal;
}


QString KLCPUFuses::boolValListToString(QList< bool > vals) const
{
    QList< bool >::iterator it;
    QString retVal;
    for ( it = vals.begin(); it != vals.end(); ++it )
    {
        retVal += (*it) ? TRUE_STRING : FALSE_STRING;
        retVal += ", ";
    }
    retVal = retVal.left( retVal.length() - 2 );
    return retVal;
}

