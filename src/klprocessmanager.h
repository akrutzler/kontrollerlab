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
#ifndef KLPROCESSMANAGER_H
#define KLPROCESSMANAGER_H

#include <qobject.h>
#include "klprocess.h"
#include <QList>
#include <kurl.h>


class KLProcessHandler;
class KontrollerLab;


/**
This is the central process manager.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLProcessManager : public QObject
{
Q_OBJECT
public:
    KLProcessManager(KontrollerLab *parent = 0, const char *name = 0);

    ~KLProcessManager();
    void setCWD( const KUrl& url ) { m_cwd = url; }
    void addProcess( const QString& cmd, KLProcessHandler* callBack );
    void setFinishMessage(const QString& theValue) { m_finishMessage = theValue; }
    QString finishMessage() const { return m_finishMessage; }

    void setDontAbortQueueOnError(bool theValue)
    { m_dontAbortQueueOnError = theValue; }
    bool dontAbortQueueOnError() const
    { return m_dontAbortQueueOnError; }
    int procsInQueue() const { return m_procs.count(); }
    KLProcess* currentlyRunningProc() const
    { return m_currentlyRunningProc; }

public slots:
    void processExited( KLProcess* who );
    void slotMessage( int exitVal, const QString& msg );

protected:
    void addProcessImmediately( const QString& cmd, KLProcessHandler* callBack );
    
    QList< KLProcess* > m_procs;
    KLProcess* m_currentlyRunningProc;
    KUrl m_cwd;
    KontrollerLab* m_parent;
    QString m_finishMessage;
    bool m_dontAbortQueueOnError;
};

#endif
