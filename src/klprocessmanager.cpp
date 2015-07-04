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
#include "klprocessmanager.h"
#include "klprogrammerinterface.h"
#include "kontrollerlab.h"


KLProcessManager::KLProcessManager(KontrollerLab *parent, const char *name)
    : QObject((QObject*)parent)
{
    setObjectName(name);
    m_parent = parent;
    m_dontAbortQueueOnError = false;
    m_currentlyRunningProc = 0L;
}


KLProcessManager::~KLProcessManager()
{
}


void KLProcessManager::processExited( KLProcess * who )
{
    KLProcessHandler *cb = who->callBack();
    m_currentlyRunningProc = 0L;
    cb->shellProcessExitedWith( who->exitStatus(), who->stdout(), who->stderr() );
    if ( (who->exitStatus() != 0) && (!m_dontAbortQueueOnError) )
        m_procs.clear();
    else
        m_procs.removeAll( who );

    if ( m_procs.count() > 0)
    {
        slotMessage( 0, m_procs.first()->cmd() );
        m_currentlyRunningProc = m_procs.first();
        m_procs.first()->start();
    }
    else
    {
        if ( (!m_finishMessage.isEmpty()) && (!m_finishMessage.isNull()) && (who->exitStatus() == 0) )
            slotMessage( 0, m_finishMessage );
        m_finishMessage = "";
    }
    delete who;
}


void KLProcessManager::addProcess( const QString & cmd, KLProcessHandler * callBack )
{
    // qDebug("adding: %s", cmd.ascii() );
    KLProcess *proc = new KLProcess( cmd, m_cwd.path() );
    proc->setCallBack( callBack );
    m_procs.append( proc );
    connect( proc, SIGNAL( processExited( KLProcess* ) ),
             this, SLOT( processExited( KLProcess* ) ) );
    if ( m_procs.first() == proc )
    {
        slotMessage( 0, proc->cmd() );
        m_currentlyRunningProc = m_procs.first();
        proc->start();
    }
}


void KLProcessManager::addProcessImmediately( const QString & cmd, KLProcessHandler * callBack )
{
    KLProcess *proc = new KLProcess( cmd, m_cwd.path() );
    proc->setCallBack( callBack );
    m_procs.append( proc );
    connect( proc, SIGNAL( processExited( KLProcess* ) ),
             this, SLOT( processExited( KLProcess* ) ) );
    slotMessage( 0, cmd );
    m_currentlyRunningProc = m_procs.first();
    proc->start();
}

void KLProcessManager::slotMessage( int exitVal, const QString & msg )
{
    if (m_parent)
        m_parent->slotMessageBox( exitVal, msg );
}


