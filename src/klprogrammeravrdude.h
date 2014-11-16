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
#ifndef KLPROGRAMMERAVRDUDE_H
#define KLPROGRAMMERAVRDUDE_H

#include "klprogrammerinterface.h"
#include "kldocumentview.h"


#define AVRDUDE                              "AVRDUDE"
#define AVRDUDE_OVERRIDE_BAUD_RATE           "AVRDUDE_OVERRIDE_BAUD_RATE"
#define AVRDUDE_SPECIFY_BIT_CLOCK            "AVRDUDE_SPECIFY_BIT_CLOCK"
#define AVRDUDE_PROGRAMMER_TYPE              "AVRDUDE_PROGRAMMER_TYPE"
#define AVRDUDE_CONNECTION_PORT              "AVRDUDE_CONNECTION_PORT"
#define AVRDUDE_EXTERNAL_CONFIG_FILE         "AVRDUDE_EXTERNAL_CONFIG_FILE"
#define AVRDUDE_DISABLE_AUTO_ERASE           "AVRDUDE_DISABLE_AUTO_ERASE"
#define AVRDUDE_TEST_MODE                    "AVRDUDE_TEST_MODE"
#define AVRDUDE_OVERRIDE_INVALID_SIGNATURE   "AVRDUDE_OVERRIDE_INVALID_SIGNATURE"
#define AVRDUDE_COUNT_ERASE                  "AVRDUDE_COUNT_ERASE"


/**
This is the class for the guys who want to use AVRDUDE.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLProgrammerAVRDUDE : public KLProgrammerInterface
{
public:
    KLProgrammerAVRDUDE(KLProcessManager* processManager, KLProject* project);

    ~KLProgrammerAVRDUDE();

    virtual bool erase();
    virtual bool upload( const KUrl& url );
    virtual bool verify( const KUrl& url );
    virtual bool download( const KUrl& url );
    virtual bool ignite( const KUrl& url );
    virtual bool programFuses( QMap<QString, QString> fuses );
    virtual bool readFuses();
    QString getPortGUIStringFor( const QString port );
    QString getPortFor( const QString& port );

    void setEraseCounter( int cnt );
protected:
    virtual QMap< QString, QString > parseFuses( const QString& stdout );

    void readFromFile( QFile& file, const QString& key, QMap<QString, QString>& map );

    QString attribute( const QString& attr, const QString confKey ) const;
    QString formBaseCommand() const;
    QMap< QString, QString > m_mmcuLookup;
};

#endif
