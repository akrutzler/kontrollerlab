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
#include "klproject.h"
#include "kontrollerlab.h"
#include <klocale.h>
#include "klprogrammerinterface.h"
#include <qregexp.h>
#include <qfileinfo.h>
#include "kldocumentview.h"


KLProgrammerInterface::KLProgrammerInterface( KLProcessManager* processManager, KLProject* project ) :
KLProcessHandler( processManager )
{
    m_project = project;
    m_readFusesBackannotate = 0;
    m_stk500Backannotate = 0;
    m_killed = false;
}


KLProgrammerInterface::~KLProgrammerInterface()
{
}

QString KLProgrammerInterface::config( const QString & key ) const
{
    if ( m_configuration.find( key ) != m_configuration.end() )
        return m_configuration[key];
    return "";
}

void KLProgrammerInterface::setConfig( const QString & key, const QString & val )
{
    m_configuration[key] = val;
}


void KLProgrammerInterface::shellProcessExitedWith( int exitVal, const QString & stdout, const QString & stderr )
{
    m_runningProc = false;
    if ( (!stdout.isEmpty()) && (!stdout.isNull()) )
        m_project->parent()->slotMessageBox( exitVal, stdout );
    if ( (!stderr.isEmpty()) && (!stderr.isNull()) )
        m_project->parent()->slotMessageBox( exitVal, stderr );
    if ( m_killed || ( ( exitVal != 0 ) && (!m_processManager->dontAbortQueueOnError()) ) )
    {
        m_project->parent()->slotMessageBox( exitVal, i18n("Error(s) occurred: The exit status was %1.").arg( exitVal ) );
        if ( m_readFusesBackannotate > 0 )
        {
            m_project->backannotateFuses( );
            m_readFusesBackannotate = 0;
        }
        else if ( m_stk500Backannotate > 0 )
        {
            m_project->backannotateSTK500( );
            m_stk500Backannotate = 0;
        }
    }
    else
    {
        if ( m_readFusesBackannotate > 0 )
        {
            QMap< QString, QString > fuses = parseFuses( stdout );
            m_project->backannotateFuses( fuses );
            m_readFusesBackannotate--;
        }
        else if ( m_stk500Backannotate > 0 )
        {
            m_project->backannotateSTK500( stdout );
            m_stk500Backannotate--;
        }
    }
    if ( m_readFusesBackannotate == 0 )
        m_processManager->setDontAbortQueueOnError( false );
    if ( m_processManager->procsInQueue() <= 1 )
        m_project->setProgrammerBusy( false );
    m_killed = false;
}


QString KLProgrammerInterface::validPath(const KUrl &url )
{
    QString retVal = url.path();
    retVal.replace( QRegExp("([^\\\\])\\s"), "\\1\\ " );
    return retVal;
}


QString KLProgrammerInterface::getProgrammerTypeGUIStringFor( const QString & val ) const
{
    if ( m_programmerTypes.find( val ) != m_programmerTypes.end() )
        return m_programmerTypes[ val ];
    else
        return val;
}


QString KLProgrammerInterface::getParallelPortGUIStringFor( const QString & val ) const
{
    if ( m_parallelPorts.find( val ) != m_parallelPorts.end() )
        return m_parallelPorts[ val ];
    else
        return val;
}


QString KLProgrammerInterface::getSerialPortGUIStringFor( const QString & val ) const
{
    if ( m_serialPorts.find( val ) != m_serialPorts.end() )
        return m_serialPorts[ val ];
    else
        return val;
}


QString KLProgrammerInterface::getProgrammerTypeFor( const QString & val ) const
{
    QMap<QString, QString>::Iterator it;
    QMap<QString, QString> buffer(m_programmerTypes);
    for ( it = buffer.begin(); it != buffer.end(); ++it )
    {
        if ( it.value() == val )
            return it.key();
    }
    return "";
}


QString KLProgrammerInterface::getParallelPortFor( const QString & val ) const
{
    QMap<QString, QString>::Iterator it;
    QMap<QString, QString> buffer(m_parallelPorts);
    for ( it = buffer.begin(); it != buffer.end(); ++it )
    {
        if ( it.value() == val )
            return it.key();
    }
    return "";
}


QString KLProgrammerInterface::getSerialPortFor( const QString & val ) const
{
    QMap<QString, QString>::Iterator it;
    QMap<QString, QString> buffer(m_serialPorts);
    for ( it = buffer.begin(); it != buffer.end(); ++it )
    {
        if ( it.value() == val )
            return it.key();
    }
    return "";
}


void KLProgrammerInterface::stopKillingProc( )
{
    if ( m_processManager->currentlyRunningProc() )
    {
        m_killed = true;
        m_processManager->currentlyRunningProc()->kill();
    }
}


bool KLProgrammerInterface::portIsReadAndWritable(const QString & port)
{
    QFileInfo fi( port );
    return fi.isReadable() && fi.isWritable();
}

