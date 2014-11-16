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
#ifndef KLCPUFUSES_H
#define KLCPUFUSES_H

#include <qstringlist.h>
#include <QList>
#include <qdom.h>


/**
In this class, the names and the change mode of the fuse bits are saved.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLCPUFuses{
public:
    KLCPUFuses( const QString& mcuName,
                QList< bool > lowCanBeChanged,
                QList< bool > highCanBeChanged,
                QList< bool > extCanBeChanged,
                QStringList lowNames,
                QStringList highNames,
                QStringList extNames );
    KLCPUFuses( QDomDocument & document, QDomElement & parent );
    KLCPUFuses();

    ~KLCPUFuses();

    void setLowCanBeChanged(const QList< bool >& theValue)
    { m_lowCanBeChanged = theValue; }
    QList< bool > lowCanBeChanged() const
    { return m_lowCanBeChanged; }
    void setHighCanBeChanged(const QList< bool >& theValue)
    { m_highCanBeChanged = theValue; }
    QList< bool > highCanBeChanged() const
    { return m_highCanBeChanged; }
    void setExtCanBeChanged(const QList< bool >& theValue)
    { m_extCanBeChanged = theValue; }
    QList< bool > extCanBeChanged() const
    { return m_extCanBeChanged; }
    void setLowNames(const QStringList& theValue)
    { m_lowNames = theValue; }
    QStringList lowNames() const
    { return m_lowNames; }
    void setHighNames(const QStringList& theValue)
    { m_highNames = theValue; }
    QStringList highNames() const
    { return m_highNames; }
    void setExtNames(const QStringList& theValue)
    { m_extNames = theValue; }
    QStringList extNames() const
    { return m_extNames; }

    void setMcuName(const QString& theValue)
    { m_mcuName = theValue; }
    QString mcuName() const
    { return m_mcuName; }
    void createDOMElement( QDomDocument & document, QDomElement & parent );

protected:
    QList< bool > m_lowCanBeChanged, m_highCanBeChanged, m_extCanBeChanged;
    QStringList m_lowNames, m_highNames, m_extNames;
    QString m_mcuName;
    QString boolValListToString( QList< bool > vals ) const;
    QList< bool > stringToBoolValList( const QString& boolList ) const;
    void createAndAddDOM( QDomDocument & document, QDomElement & fuse,
                          const QString & name, const QString & text );
};


class KLCPUFusesList : public QList< KLCPUFuses >
{
public:
    void readFromDOMElement( QDomDocument & document, QDomElement & parent );
    void createDOMElement( QDomDocument & document, QDomElement & parent );
};

#endif
