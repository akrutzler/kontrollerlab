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
#ifndef KLCPUREGISTERDESCRIPTION_H
#define KLCPUREGISTERDESCRIPTION_H
#include <QList>
#include <qstring.h>
#include <qdom.h>


/**
This class store a register location, the name of the register as used by avr-gcc, and it's description.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLCPURegisterDescription{
public:
    KLCPURegisterDescription() { m_location = 0; }

    KLCPURegisterDescription( unsigned int location,
                              const QString& name, const QString& description );

    ~KLCPURegisterDescription();

    unsigned int location() const { return m_location; }
    QString name() const { return m_name; }
    QString description() const { return m_description; }

    friend bool operator<( const KLCPURegisterDescription& a, const KLCPURegisterDescription& b);
protected:
    unsigned int m_location;
    QString m_name, m_description;
};


typedef QList< KLCPURegisterDescription > KLCPURegisterDescriptionList_;

class KLCPURegisterDescriptionList : public KLCPURegisterDescriptionList_
{
public:
    void setForCPU( const QString& name ) { m_forCPU = name; }
    QString forCPU() const { return m_forCPU; }

    KLCPURegisterDescription registerDescriptionFor( unsigned int location ) const;
    void readFromDOMElement(QDomDocument & doc, QDomElement & elem, const QString& name);

protected:
    QString m_forCPU;
    unsigned int m_highestRegisterLocation;
};

#endif
