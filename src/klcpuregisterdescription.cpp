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
#include "klcpuregisterdescription.h"
#include <Qt3Support>

KLCPURegisterDescription::KLCPURegisterDescription( unsigned int location,
                          const QString& name, const QString& description )
{
    m_location = location;
    m_name = name;
    m_description = description;
}


KLCPURegisterDescription::~KLCPURegisterDescription()
{
    // Not necessary at the moment.
}


bool operator<( const KLCPURegisterDescription& a, const KLCPURegisterDescription& b)
{
    return a.m_location < b.m_location;
}


KLCPURegisterDescription KLCPURegisterDescriptionList::registerDescriptionFor(unsigned int location) const
{
    KLCPURegisterDescriptionList copy = *this;
    for( iterator it = copy.begin(); it != copy.end(); ++it )
    {
        if ( (*it).location() == location )
            return *it;
    }
    return KLCPURegisterDescription();
}


/**
 * Reads the register names form the registers.xml file
 * @param doc The DOM document
 * @param elem The DOM element to read the data from
 * @param nane The name of the CPU to read the register descriptions for
 */
void KLCPURegisterDescriptionList::readFromDOMElement(QDomDocument &, QDomElement & elem, const QString& name )
{
    clear();
    if ( elem.nodeName().upper() == "REGISTER_DESCRIPTIONS" )
    {
        QString ver = elem.attribute( "VERSION", "" );
        // if ( !ver.isEmpty() )
        //     qDebug( "Reading register definition file version %s for CPU %s.", ver.ascii(), m_name.ascii() );
        QDomNode n = elem.firstChild();
        while ( !n.isNull() )
        {
            if ( !n.isElement() )
            {
                n = n.nextSibling();
                continue;
            }
            QDomElement e = n.toElement();
            if ( ( e.nodeName().upper() == "CPU" ) &&
                    ( e.attribute( "NAME", "" ).upper() == name.upper() ) )
            {
                QDomNode reg = e.firstChild();
                while ( !reg.isNull() )
                {
                    if ( !reg.isElement() )
                    {
                        reg = reg.nextSibling();
                        continue;
                    }
                    QDomElement regEle = reg.toElement();
                    if ( regEle.nodeName().upper() == "REGISTER" )
                    {
                        QString locString = regEle.attribute( "LOC", "-1" );
                        bool ok;
                        int loc = locString.toInt( &ok );
                        if ( ok && (loc >= 0) )
                        {
                            QString regText = regEle.text();
                            QString regName, regDesc = "";
                            if ( regText.contains( "," ) )
                            {
                            // There is a description
                                regName = regText.left( regText.find( "," ) );
                                regName = regName.stripWhiteSpace();
                                regDesc = regText.right( regText.length() - regName.length() - 1 );
                                regDesc = regDesc.stripWhiteSpace();
                            }
                            else
                            {
                                regName = regText.stripWhiteSpace();
                            }
                            append( KLCPURegisterDescription( loc, regName, regDesc ) );
                        }
                    }
                    reg = reg.nextSibling();
                }
                // We found what we were looking for -> break!
                // qDebug("Found def for %s.", m_name.ascii() );
                break;
            }
            n = n.nextSibling();
        }
    }
    setForCPU( name );
    //qHeapSort( *this );
}

