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
#ifndef KLPROGRAMMERINTERFACE_H
#define KLPROGRAMMERINTERFACE_H

#include <qstring.h>
#include <kurl.h>
#include <qmap.h>
#include <qstringlist.h>
#include "klprocessmanager.h"
#include "klprocesshandler.h"


#define PROGRAMMER_NAME     "PROGRAMMER_NAME"
#define PROGRAMMER_COMMAND  "PROGRAMMER_COMMAND"

#define FUSE_L                               "FUSE_L"
#define FUSE_H                               "FUSE_H"
#define FUSE_E                               "FUSE_E"
#define FUSE_LOCK                            "FUSE_LOCK"

#define TRUE_STRING "TRUE"
#define FALSE_STRING "FALSE"

class KLProject;

/**
This is the interface class to all programmers, e.g. the UISP.
Don't try to instantiate this class directly, it is a pure interface.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLProgrammerInterface : public KLProcessHandler
{
public:
    KLProgrammerInterface( KLProcessManager* processManager, KLProject* project);

    virtual ~KLProgrammerInterface();

    virtual bool erase() = 0;
    virtual bool upload( const KUrl& url ) = 0;
    virtual bool verify( const KUrl& url ) = 0;
    virtual bool download( const KUrl& url ) = 0;
    virtual bool ignite( const KUrl& url ) = 0;
    virtual bool programFuses( QMap<QString, QString> fuses ) = 0;
    virtual bool readFuses() = 0;

    virtual bool portIsReadAndWritable( const QString& port );
    void stopKillingProc();

    void setConfig( const QString& key, const QString& val );
    QString config( const QString& key ) const;
    QMap<QString, QString>& configuration() { return m_configuration; }

    QString getProgrammerTypeGUIStringFor( const QString& val ) const;
    QString getParallelPortGUIStringFor( const QString& val ) const;
    QString getSerialPortGUIStringFor( const QString& val ) const;

    QString getProgrammerTypeFor( const QString& val ) const;
    QString getParallelPortFor( const QString& val ) const;
    QString getSerialPortFor( const QString& val ) const;

    QStringList programmerTypesKeyList() const
    { return m_programmerTypesKeyList; }
    QStringList parallelPortsKeyList() const
    { return m_parallelPortsKeyList; }
    QStringList serialPortsKeyList() const
    { return m_serialPortsKeyList; }

    
    void shellProcessExitedWith( int exitVal, const QString & stdout, const QString & stderr );
protected:
    QString validPath( const KUrl& url );
    virtual QMap< QString, QString > parseFuses( const QString& stdout ) = 0;
    bool m_runningProc;
    bool m_killed;
    int m_readFusesBackannotate, m_stk500Backannotate;
    QMap<QString, QString> m_configuration;
    KLProject* m_project;

    QStringList m_programmerTypesKeyList;
    QStringList m_parallelPortsKeyList;
    QStringList m_serialPortsKeyList;
    QStringList m_serialProgrammer, m_parallelProgrammer;
    QMap<QString, QString> m_programmerTypes, m_parallelPorts, m_serialPorts;

};

#endif
