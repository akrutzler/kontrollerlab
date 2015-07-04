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
#include "klprocess.h"
#include <klocale.h>
#include <qregexp.h>

#define KONTROLLERLAB_SPACE "____KONTROLLERLAB__SPACE__!!__ESCAPE__SEQUENCE____"


KLProcess::KLProcess(QString command_, QString workingDir,
                     const QStringList& envVars,
                     QObject *parent, const char *name)
 : QObject(parent)
{
    setObjectName(name);
    m_process = new QProcess(this);
    m_process->setObjectName("process_" + QString(name));
    QStringList cmds;
    QString command = command_;
    // Set the variable which is used to display the command in the message box:
    m_cmd = command;
    // Replace all escaped spaces with a string that can't be an argument:
    command.replace( "\\ ", KONTROLLERLAB_SPACE );
    // Split the command:
    cmds = command.split(" ",QString::SkipEmptyParts);
    for ( QStringList::iterator it = cmds.begin(); it != cmds.end(); ++it )
    {
        QString arg = *it;
        // and replace the KONTROLLERLAB_SPACE by the space again.
        arg.replace( KONTROLLERLAB_SPACE, " " );
        m_arguments.append( arg );
    }
    m_stderr = m_stdout = "";
    m_program = m_arguments.takeFirst();

/*
    for ( QStringList::iterator it = cmds.begin(); it != cmds.end(); ++it )
    {
        QString arg = *it;
        m_process->addArgument( *it );
    }
*/
    
    if ( (!workingDir.isEmpty()) && (!workingDir.isNull()) )
        m_process->setWorkingDirectory( workingDir );

    m_envVars = envVars;
    m_otherError = false;
    
    // qDebug( command );
    connect( m_process, SIGNAL(readyReadStandardOutput()),
             this, SLOT(readFromStdOut()) );
    connect( m_process, SIGNAL(readyReadStandardError()),
             this, SLOT(readFromStdErr()) );
    connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(slotProcessExited(int,QProcess::ExitStatus)) );
}


KLProcess::~KLProcess()
{
}

void KLProcess::slotProcessExited(int exitCode, QProcess::ExitStatus  exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    emit processExited( this );
}

void KLProcess::readFromStdOut( )
{
    m_stdout += m_process->readAllStandardOutput();
}

void KLProcess::readFromStdErr( )
{
    m_stderr += m_process->readAllStandardError();
}

void KLProcess::start( )
{
    m_otherError = false;

    if ( m_envVars.count() )
        m_process->start(m_program, m_arguments);
    else
        m_process->start(m_program, m_arguments);
    if (!m_process->waitForStarted())
    {
        m_otherError = true;
        m_stderr = i18n("Command not found: %1").arg( m_cmd );
        slotProcessExited(1,QProcess::NormalExit);   //wrong exitcode?
    }
}


void KLProcess::kill( )
{
    m_process->kill();
}


