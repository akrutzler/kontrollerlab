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
#include "kldebugger.h"
#include "klserialterminalwidget.h"
#include "klmemoryviewwidget.h"
#include "kontrollerlab.h"
#include <klocale.h>
#include "klproject.h"
#include <kmessagebox.h>
#include "kldebuggerbreakpoint.h"
#include <vector>
#include "kldocumentview.h"


KLDebugger::KLDebugger(KLSerialTerminalWidget* serTerm, KontrollerLab* parent, 
                       const char *name)
 : QObject(parent),
           m_flash(this, "FlashMemory"), m_ram(this, "RAMMemory"),
           m_cpu(this, "CPU of " + QString(name))
{
    setObjectName(name);
    m_parent = parent;
    m_state = DBG_Off;
    m_atLeastOneSrcLine = false;
    m_DMMode = false;
    m_ICDMode = false;
    m_memoryViewWidget = 0L;
    m_CLineToCompare = -1;
    m_stopCallCounter = 0;
    m_serialTerminalWidget = serTerm;

    m_requestTimeout.setObjectName("timeoutTimer");
    m_requestTimeout.setSingleShot(true);
    connect( m_serialTerminalWidget, SIGNAL(receivedICDData( const KLCharVector& ) ),
             this, SLOT( slotControllerResponded( const KLCharVector& ) ) );
    connect( &m_requestTimeout, SIGNAL(timeout()), this, SLOT(requestTimeout()) );
}


KLDebugger::~KLDebugger()
{
}

/**
 * This routine is called by the serial terminal widget as soon as the
 * controller responds to the last request. If there are further requests in queue,
 * these requests are then sent to the controller.
 * @param data The answer of the controller.
 */
void KLDebugger::slotControllerResponded( const KLCharVector & data_, bool overrideReadFromUART )
{
    KLCharVector data = data_;
    if ( overrideReadFromUART )
    {
        if ( m_requests.count() == 0 )
            return;
        KLControllerRequest responseFor = m_requests[0];
        if ( responseFor.writeRequest() )
        {
            m_ram.writeRAMNoSignal( responseFor.address(), responseFor.writeValue() );
            if (inDMMode() || inPCOnlyMode() || inICDMode())
            {
                if (m_memoryViewWidget)
                    m_memoryViewWidget->slotMemoryReadCompleted( responseFor.address(), responseFor.writeValue() );
                    // Write to memory view!
            }
        }
        m_requests.pop_front();
    }
    else
    {
        unsigned int dataPtr=0;
        while (dataPtr < data.size())
        {
            if ( m_requests.count() == 0 )
                return;
            KLControllerRequest responseFor = m_requests[0];
            if (responseFor.readRequest())
            {
                // qDebug("R OK, left: %d", m_requests.count());
                int val = data[ dataPtr ];
                if (val < 0)
                    val+=256;
                // Okay, this request is completed:
                m_requests.pop_front();
                m_requestTimeout.stop();
                m_ram.writeRAM( responseFor.address(), val );
                if (inDMMode() || inPCOnlyMode() || inICDMode())
                {
                    if (m_memoryViewWidget)
                        m_memoryViewWidget->slotMemoryReadCompleted( responseFor.address(), val );
                    // Write to memory view!
                }
            }
            else if ( responseFor.writeRequest() )
            {
                // qDebug("W OK");
                if ( data[ dataPtr ] != 'O' )
                {
                    m_parent->slotMessageBox( 1,
                        i18n("Write request to controller was unsuccessful.") );
                }
                else
                {
                    // Okay, this request is completed:
                    m_requests.pop_front();
                    m_requestTimeout.stop();
                    m_ram.writeRAM( responseFor.address(), responseFor.writeValue() );
                    // qDebug("Write request successful. Now in Queue: %d", m_requests.count() );
                    if (inDMMode() || inPCOnlyMode() || inICDMode())
                    {
                        if (m_memoryViewWidget)
                            m_memoryViewWidget->slotMemoryReadCompleted( responseFor.address(), responseFor.writeValue() );
                        // Write to memory view!
                    }
                }
            }
            dataPtr++;
        }
    }
    // If there are more requests in the queue, send the next:
    int requestsInQueue = m_requests.count();
    if ( requestsInQueue > 0 )
        sendNextRequest();
    else if ( requestsInQueue == 0 )
    {
        if ( inPCOnlyMode() || inICDMode() )
            slotStepFinished();
    }
}

void KLDebugger::writeMemoryCell( unsigned int adr, unsigned char val,
                                  int numberOfCells,
                                  unsigned int adr2, unsigned char val2,
                                  unsigned int adr3, unsigned char val3,
                                  unsigned int adr4, unsigned char val4 )
{
    // If it is neccessary to initiate the request, this is set here:
    // (If the queue is empty, we have to trigger the sending,
    // otherwise, not.)
    bool sendNextRequestNeccessary = ( m_requests.count() == 0 );

    m_requests.append( KLControllerRequest( false, adr, val ) );
    if ( numberOfCells > 1 )
        m_requests.append( KLControllerRequest( false, adr2, val2 ) );
    if ( numberOfCells > 2 )
        m_requests.append( KLControllerRequest( false, adr3, val3 ) );
    if ( numberOfCells > 3 )
        m_requests.append( KLControllerRequest( false, adr4, val4 ) );
    // qDebug("W request");
    
    if (sendNextRequestNeccessary)
        sendNextRequest();
}


void KLDebugger::readMemoryCell( unsigned int adr, int numberOfCells,
                                 unsigned int adr2, unsigned int adr3,
                                 unsigned int adr4 )
{
    // If it is neccessary to initiate the request, this is set here:
    // (If the queue is empty, we have to trigger the sending,
    // otherwise, not.)
    bool sendNextRequestNeccessary = ( m_requests.count() == 0 );

    m_requests.append( KLControllerRequest( true, adr ) );
    if ( numberOfCells>1 )
        m_requests.append( KLControllerRequest( true, adr2 ) );
    if ( numberOfCells>2 )
        m_requests.append( KLControllerRequest( true, adr3 ) );
    if ( numberOfCells>3 )
        m_requests.append( KLControllerRequest( true, adr4 ) );
    // qDebug("R request");
    
    if (sendNextRequestNeccessary)
        sendNextRequest();
}


void KLDebugger::setDMMode( bool val )
{
    if ( val != m_DMMode )
        m_requests.clear();
    m_DMMode = val;
    m_ICDMode = false;
    m_serialTerminalWidget->setTerminalToICDMode( val );
    m_memoryViewWidget->allowSetMemoryCell( true );
    if ( val )
    {
        m_serialTerminalWidget->slotOpenICD( DEBUGGER_BAUD_RATE );
    }
}


void KLDebugger::setICDMode( bool val )
{
    if ( val != m_ICDMode )
        m_requests.clear();
    m_DMMode = false;
    m_ICDMode = val;
    m_serialTerminalWidget->setTerminalToICDMode( val );
    // We allow to change values here now:
    /// @TODO Check if this works for ICD mode!
    // if ( val )
    //     m_memoryViewWidget->allowSetMemoryCell( false );
    if ( val )
    {
        m_serialTerminalWidget->slotOpenICD( DEBUGGER_BAUD_RATE );
    }
}


void KLDebugger::sendNextRequest( )
{
    if ( m_requests.count() == 0 )
        qWarning( "sendNextRequest() called, but the request queue is empty. %s:%d",
                  __FILE__, __LINE__ );
    KLControllerRequest cur = m_requests[0];
    bool readFromExternalDeviceNeccessary = false;
    
    if ( m_DMMode )
    {
        readFromExternalDeviceNeccessary = true;
    }
    else if ( m_ICDMode )
    {
        // If the request is mapped to the external device, set the flag:
        readFromExternalDeviceNeccessary = m_memoryMappingList.inRange( cur.address() );
        // m_ram.mappedToExternalDevice( cur.address() );
    }
    // If in PC Only mode, no read from external device is neccessary!
    if ( readFromExternalDeviceNeccessary )
    {
        if ( cur.readRequest() )
        {
            // qDebug("R sent");
            std::vector< unsigned char > data;
            data.push_back( 'R' );
            data.push_back( (cur.address() & 0xff00)>>8 );
            data.push_back( cur.address() & 0xff );
            /*
            m_serialTerminalWidget->slotSendICDData( QString("R%1%2")
                    .arg(QChar((cur.address()&0xff00)>>8))
                    .arg(QChar(cur.address()&0xff)) );
            */
            m_serialTerminalWidget->slotSendICDData( data );
            m_requestTimeout.start( REQUEST_TIMEOUT );
        }
        else if ( cur.writeRequest() )
        {
            // qDebug("W sent");
            std::vector< unsigned char > data;
            data.push_back( 'W' );
            data.push_back( (cur.address() & 0xff00)>>8 );
            data.push_back( cur.address() & 0xff );
            data.push_back( cur.writeValue() );
            /*
            m_serialTerminalWidget->slotSendICDData( QString("W%1%2%3")
                    .arg(QChar((cur.address()&0xff00)>>8))
                    .arg(QChar(cur.address()&0xff))
                    .arg(QChar(cur.writeValue())) );
            */
            m_serialTerminalWidget->slotSendICDData( data );
            m_requestTimeout.start( REQUEST_TIMEOUT );
        }
    }
    else
    {
        // This overrides the transaction over UART.
        slotControllerResponded( KLCharVector(), true );
    }
}


void KLDebugger::slotUserSetsMemoryCell( unsigned int adr, unsigned char val )
{
    
    // If it is neccessary to initiate the request, this is set here:
    // (If the queue is empty, we have to trigger the sending,
    // otherwise, not.)
    bool sendNextRequestNeccessary = ( m_requests.count() == 0 );

    m_requests.append( KLControllerRequest( false, adr, val ) );
    
    if (sendNextRequestNeccessary)
        sendNextRequest();
}

void KLDebugger::setMemoryViewWidget( KLMemoryViewWidget * wdg )
{
        m_memoryViewWidget = wdg;
        if ( wdg )
            connect( m_memoryViewWidget, SIGNAL( memoryCellSet( unsigned int, unsigned char ) ),
               this, SLOT( slotUserSetsMemoryCell( unsigned int, unsigned char ) ) );

}

void KLDebugger::requestTimeout( )
{
    // qDebug("Timeout. Request resent.");
    sendNextRequest();
}

void KLDebugger::startDebugger( )
{
    // This reads the program from the .out file and the debug info.
    // After this is finished, the debugger is set into the "started"
    // state.
    if ( m_state == DBG_Off )
    {
        KLProject* prj = m_parent->project();
        m_atLeastOneSrcLine = false;
        if (!prj)
        {
            qWarning("No project to debug loaded.");
            return;
        }
        KLCPUFeatures features = prj->cpuFeaturesFor( prj->cpu() );
        int newFlashSize = features.flashEEPROMSize();
        if ( newFlashSize <= 0 )
            newFlashSize = 65536 * 4;
        m_flash.resize( newFlashSize );
        prj->readDebugHexAndInfo();
        // Write the debugger mem mappings to the debugger:
        QString mappings = prj->debuggerSettings()[ DEBUGGER_MAPPINGS ];
        KLDebuggerMemoryMappingList lst;
        lst.fromString( mappings );
        m_memoryMappingList = lst;
    }
    else if ( m_state == DBG_Paused )
    {
        m_state = DBG_Started;
        performStep();
    }
    else
    {
        qWarning("Debugger already started!");
    }
}

void KLDebugger::setFlash( unsigned int adr, unsigned int value, unsigned int asmLine )
{
    m_flash.write( adr, value );
    m_flash.setFlashAddressToASMLine( adr, asmLine );
}

void KLDebugger::setASMRelations( QList< KLSourceCodeToASMRelation > rlist )
{
    qSort( rlist );
    if ( rlist.count() > 0 )
    {
        m_relationList = rlist;
        // If the instructions were already read:
        if ( m_flash.highestValidAdress() > 0 )
            m_state = DBG_Started;
        m_cpu.setProgramCounter( 0 );
        m_ram.clear();
        m_state = DBG_Ready;
        // This is neccessary because the user may select
        // unexecutable lines for breakpoints:
        confirmBreakpoints();
        // Now, to speed up debugging, we set the src line and the src
        // for all ASM lines:

        // First, unify the list of sources:
        QStringList srcList;
        for ( unsigned int i=0; i < m_relationList.size(); i++ )
        {
            QString srcFN = m_relationList[i].sourceFilename();
            if ( srcList.indexOf( srcFN ) < 0 )
                srcList.append( srcFN );
        }
        m_flash.setSources( srcList );
        unsigned int asmI = 0;
        for ( unsigned int i=0; i < m_relationList.size()-1; i++ )
        {
            const KLSourceCodeToASMRelation src = m_relationList[ i ];
            unsigned int stop = m_relationList[ i+1 ].asmCommandAddress();
            for ( asmI = src.asmCommandAddress(); asmI < stop; asmI++ )
            {
                /*
                QString testFN;
                int testLine = getCSourceCodeLineForASMCommand( asmI, testFN );
                if ( ( src.line() != testLine ) || ( src.sourceFilename() != testFN ) )
                    qDebug("???");
                */
                m_flash.setSourceAndLineInSource( asmI, src.line(), srcList.indexOf( src.sourceFilename() ) );
            }
        }
        unsigned int lenOfFlash = m_flash.flashSize();
        const KLSourceCodeToASMRelation lastSrc = m_relationList[ m_relationList.size()-1 ];
        int lastLine = lastSrc.line();
        int srcNr = srcList.indexOf( lastSrc.sourceFilename() );
        for ( unsigned int i=lastSrc.asmCommandAddress(); i < lenOfFlash; i++ )
        {
            m_flash.setSourceAndLineInSource( i, lastLine, srcNr );
        }
        m_parent->notifyDebuggerReady();
        /*
        for (unsigned int i=0; i < m_relationList.size(); i++)
            qDebug( "Relation %d says line %s:%d on asm line %d.", i, m_relationList[i].sourceFilename().ascii(), m_relationList[i].line(),
                    m_relationList[i].asmCommandAddress() );
        */
        m_cycleTimer.singleShot( 0, this, SLOT( performStep() ) );
    }
    else
    {
        KMessageBox mb;
        mb.information( m_parent, i18n("The debugger could not read debug symbols.\n" \
                "Make sure the project was successfully compiled. " \
                "Check if the -g flag was activated during the compilation of the project."),
                  i18n("Debugger could not be started") );
    }
}

void KLDebugger::setASMInstructions(QList<KLASMInstruction> ilist, const QString & asmLines )
{
    unsigned int highestFlashAddress = 0;
    for ( unsigned int i=0; i < ilist.count(); i++ )
    {
        // qDebug( "%d", i );
        KLASMInstruction inst = ilist[ i ];
        for ( unsigned int j=0; j<inst.hexCmds().count(); j++)
        {
            // qDebug( "asm line %d", inst.address() );
            setFlash( inst.address()+j, inst.hexCmds()[j], inst.asmLine() );
            if ( highestFlashAddress < inst.address()+j )
                highestFlashAddress = inst.address()+j;
        }
    }
    m_flash.setHighestValidAdress( highestFlashAddress );
    // m_flash.printDebug();
    m_asmLines = asmLines;
}

void KLDebugger::performStep( )
{
    QString curFn;
    
    int curLine = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), curFn );
    // qDebug( "%s:%d (%s) %d", curFn.ascii(), curLine, QString::number( m_cpu.programCounter(), 16 ).ascii(), m_stopCallCounter );
    KLProject * prj = m_parent->project();
    KUrl curURL( curFn );
    bool fileNotInProject = false;
    if ( prj )
    {
        KLDocument* doc = prj->getDocumentByURL( curURL );
        fileNotInProject = doc ? false : true;
    }
    bool fileInProject = !fileNotInProject;
    // Now we need to save if in this debug run,
    // there was at least one visible source code line.
    if ( !m_atLeastOneSrcLine )
    {
        if ( fileInProject )
        {
            // qDebug("Setting m_stopCallCounter");
            m_stopCallCounter = m_cpu.callCounter();
        }
    }
    if ( fileInProject && (curLine > 0) )
    {
        m_atLeastOneSrcLine = true;
    }
    // NOW ALL INFORMATION ABOUT THE CURRENT LINE IS KNOWN
    if ( m_state == DBG_Started )
    {
        prj->markOnlyLineInDocument( KUrl( "" ), -1 );
        bool stillAtLineWhereWeStarted = ( curURL == m_CFileToCompare ) && ( curLine == m_CLineToCompare );
        if ( stillAtLineWhereWeStarted )
        {
            m_cpu.execute();
        }
        else if ( fileNotInProject || !isBreakPoint( curURL, curLine ) )
        {
            // The line comparison is not reset when we jump
            // into a function. Only when the line is changed and we
            // are on the same call level, the line was passed
            // and we are aware of breakpoints again.
            if ( (m_CLineToCompare > 0) && fileInProject &&
                 ( m_stopCallCounter >= m_cpu.callCounter() ) )
            {
                // Stop the next time we pass by:
                m_CLineToCompare = -1;
                m_CFileToCompare = KUrl();
            }
            m_cpu.execute();
        }
        else
        {
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
            prj->markOnlyLineInDocument( KUrl( curFn ), curLine );
            m_parent->notifyDebuggerPaused();
        }
    }
    else if ( m_state == DBG_StopWhenCLineChanges ) 
    {
        if ( prj )
        {
            prj->markOnlyLineInDocument( KUrl( curFn ), curLine );
        }
        if ( ( ( curURL == m_CFileToCompare ) && ( curLine == m_CLineToCompare ) ) || fileNotInProject )
        {
            m_cpu.execute();
        }
        else
        {
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
        }
    }
    else if ( ( m_state == DBG_StopWhenCLineChangesSteppingOverFunctions ) ||
              ( m_state == DBG_StopWhenStopCallCouterReached ) )
    {
        if ( prj )
        {
            if ( m_cpu.callCounter() == m_stopCallCounter )
                prj->markOnlyLineInDocument( KUrl( curFn ), curLine );
        }
        if ( ( m_cpu.callCounter() > m_stopCallCounter ) || ( ( curURL == m_CFileToCompare ) &&
               ( curLine == m_CLineToCompare ) ) || fileNotInProject )
        {
            if ( ( curURL == m_CFileToCompare ) && ( curLine == m_CLineToCompare ) )
                m_cpu.execute();
            else if ( fileNotInProject || !isBreakPoint( curURL, curLine ) )
                m_cpu.execute();
            else
            {
                m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
                m_memoryViewWidget->updateGUI();
                m_state = DBG_Paused;
                prj->markOnlyLineInDocument( KUrl( curFn ), curLine );
            }
        }
        else
        {
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
        }
    }
    else if ( m_state == DBG_RunToCLine )
    {
        if ( ( ( curURL != m_CFileToCompare ) || ( curLine != m_CLineToCompare ) ) || fileNotInProject )
        {
            if ( fileNotInProject || !isBreakPoint( curURL, curLine ) )
                m_cpu.execute();
            else
            {
                m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
                m_memoryViewWidget->updateGUI();
                m_state = DBG_Paused;
            }
        }
        else
        {
            if ( prj )
                prj->markOnlyLineInDocument( curURL, curLine );
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
        }
    }
}

void KLDebugger::slotStepFinished( )
{
    QString curFn;
    int curLine = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), curFn );
    KLProject * prj = m_parent->project();
    KUrl curURL( curFn );
    bool fileNotInProject = false;
    if ( prj )
    {
        KLDocument* doc = prj->getDocumentByURL( curURL );
        fileNotInProject = doc ? false : true;
    }
    // NOW ALL INFORMATION ABOUT THE CURRENT LINE IS KNOWN
    // qDebug("trigger new step");
    if ( m_state == DBG_Started )
    {
        if ( m_cpu.endlessLoop() )
            m_cycleTimer.singleShot( 10, this, SLOT( performStep() ) );
        else
            m_cycleTimer.singleShot( 0, this, SLOT( performStep() ) );
    }
    else if ( m_state == DBG_StopWhenCLineChanges )
    {
        if ( prj )
        {
            prj->markOnlyLineInDocument( curURL, curLine );
        }
        if ( m_cpu.endlessLoop() )
        {
            // qDebug( "stop in line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
        }
        else
        {
            if ( ( ( curURL == m_CFileToCompare ) && ( curLine == m_CLineToCompare ) ) || fileNotInProject )
            {
                // qDebug( "source line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
                m_cycleTimer.singleShot( 0, this, SLOT( performStep() ) );
            }
            else
            {
                // qDebug( "stop in line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
                m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
                m_memoryViewWidget->updateGUI();
                m_state = DBG_Paused;
            }
        }
    }
    else if ( ( m_state == DBG_StopWhenCLineChangesSteppingOverFunctions ) ||
              ( m_state == DBG_StopWhenStopCallCouterReached ) )
    {
        if ( prj )
        {
            if ( m_cpu.callCounter() == m_stopCallCounter )
                prj->markOnlyLineInDocument( curURL, curLine );
        }
        if ( m_cpu.endlessLoop() )
        {
            // qDebug( "stop in line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
        }
        else
        {
            if ( ( m_cpu.callCounter() > m_stopCallCounter ) || ( ( curURL == m_CFileToCompare ) &&
                   ( curLine == m_CLineToCompare ) ) || fileNotInProject )
            {
                // qDebug( "source line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
                m_cycleTimer.singleShot( 0, this, SLOT( performStep() ) );
            }
            else
            {
                // qDebug( "stop in line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
                m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
                m_memoryViewWidget->updateGUI();
                m_state = DBG_Paused;
            }
        }
    }
    else if ( m_state == DBG_RunToCLine )
    {
        if ( ( ( curURL != m_CFileToCompare ) || ( curLine != m_CLineToCompare ) ) || fileNotInProject )
        {
            // qDebug( "source line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
            m_cycleTimer.singleShot( 0, this, SLOT( performStep() ) );
        }
        else
        {
            // qDebug( "stop in line %d in %s", curLine, m_CFileToCompare.filename().ascii() );
            if ( prj )
                prj->markOnlyLineInDocument( curURL, curLine );
            m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
            m_memoryViewWidget->updateGUI();
            m_state = DBG_Paused;
        }
    }
}


void KLDebugger::stopDebugger( )
{
    m_state = DBG_Off;
    m_requestTimeout.stop();
    m_requests.clear();
    m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
    m_memoryViewWidget->updateGUI();
    m_parent->notifyDebuggerStopped();
    KLProject * prj = m_parent->project();
    if ( prj )
    {
        prj->markOnlyLineInDocument( KUrl(""), -1 );
    }
}

void KLDebugger::playpauseDebugger( )
{
    if ( m_state != DBG_Started )
    {
        QString fn;
        m_state = DBG_Started;
        m_CLineToCompare = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), fn );
        m_CFileToCompare = KUrl( fn );
        m_stopCallCounter = m_cpu.callCounter();
        m_memoryViewWidget->setEnableAutomaticGUIUpdates( false );
        performStep();
    }
    else
    {
        m_state = DBG_Paused;
        m_memoryViewWidget->setEnableAutomaticGUIUpdates( true );
        m_memoryViewWidget->updateGUI();
        KLProject * prj = m_parent->project();
        if ( prj )
        {
            QString curFn;
            int curLine = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), curFn );
            prj->markOnlyLineInDocument( KUrl( curFn ), curLine );
        }

    }
}

int KLDebugger::getCSourceCodeLineForASMCommand( unsigned int adr, QString& filename )
{
    filename = m_flash.sources()[ m_flash.sourceNr( adr ) ];
    return m_flash.lineInSrc( adr );

    // THIS WAS A PERFORMANCE LEAK!!!
    /*
    ///@TODO THIS WAS A PERFORMANCE LEAK!!!
    int retVal = -1;
    for ( unsigned int i=0; i < m_relationList.size(); i++ )
    {
        const KLSourceCodeToASMRelation src = m_relationList[ i ];
        if ( src.asmCommandAddress() > adr )
            break;
        retVal = src.line();
        filename = src.sourceFilename();
    }
    const KLSourceCodeToASMRelation lastSrc = m_relationList[ m_relationList.size()-1 ];
    if ( adr > lastSrc.asmCommandAddress() )
    {
        retVal = lastSrc.line();
        filename = lastSrc.sourceFilename();
    }
    
    return retVal;
    */
}

void KLDebugger::runUntilCLineChanges( )
{
    QString fn;
    if ( ( m_state == DBG_Ready ) || ( m_state == DBG_Paused ) )
    {
        m_CLineToCompare = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), fn );
        m_CFileToCompare = KUrl( fn );
        m_state = DBG_StopWhenCLineChanges;
        m_memoryViewWidget->setEnableAutomaticGUIUpdates( false );
        performStep();
    }
}


void KLDebugger::runUntilCLineChangesSteppingOverFunctions( )
{
    QString fn;
    if ( ( m_state == DBG_Ready ) || ( m_state == DBG_Paused ) )
    {
        m_CLineToCompare = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), fn );
        m_CFileToCompare = KUrl( fn );
        m_stopCallCounter = m_cpu.callCounter();
        m_state = DBG_StopWhenCLineChangesSteppingOverFunctions;
        m_memoryViewWidget->setEnableAutomaticGUIUpdates( false );
        performStep();
    }
}


void KLDebugger::runToCLine(const KUrl &url, int line )
{
    if ( ( m_state == DBG_Ready ) || ( m_state == DBG_Paused ) )
    {
        m_CLineToCompare = seekToNextExecutableLine( url, line );
        m_CFileToCompare = url;
        m_state = DBG_RunToCLine;
        m_memoryViewWidget->setEnableAutomaticGUIUpdates( false );
        performStep();
    }
}

void KLDebugger::stepOutOfFunction( )
{
    QString fn;
    if ( m_cpu.callCounter() == 0 )
        return;
    if ( ( m_state == DBG_Ready ) || ( m_state == DBG_Paused ) )
    {
        m_CLineToCompare = getCSourceCodeLineForASMCommand( m_cpu.programCounter(), fn );
        m_CFileToCompare = KUrl( fn );
        m_stopCallCounter = m_cpu.callCounter() - 1;
        m_state = DBG_StopWhenStopCallCouterReached;
        m_memoryViewWidget->setEnableAutomaticGUIUpdates( false );
        performStep();
    }
}

void KLDebugger::toggleBreakpoint(const KUrl &url, int line )
{
    if ( line < 0 )
        return;
    if ( m_parent->project() )
    {
        KLDebuggerBreakpoint bp( url, line, seekToNextExecutableLine( url, line ) );
        if ( m_breakpoints.indexOf( bp ) >= 0 )
        {
            m_breakpoints.removeAll( bp );
            m_parent->project()->markBreakpointInDocument( url, line, false );
        }
        else
        {
            m_breakpoints.append( bp );
            m_parent->project()->markBreakpointInDocument( url, line, true );
        }
    }
}

bool KLDebugger::isBreakPoint( const KUrl & url, int line )
{
    // line = seekToNextExecutableLine( url, line );
    
    for ( unsigned int i=0; i<m_breakpoints.size(); i++ )
    {
        KLDebuggerBreakpoint bp = m_breakpoints[ i ];
        if ( bp.url() == url )
        {
            if (bp.realBreakpointLine() == line)
                return true;
        }
    }
    // KLDebuggerBreakpoint bp( url, line );
    return false;
}


QMap< QString, QString > KLDebugger::settings( ) const
{
    return m_parent->project()->settings();
}


int KLDebugger::seekToNextExecutableLine(const KUrl &url, int line )
{
    QList< KLSourceCodeToASMRelation > thisUrlOnly;
    for ( unsigned int i=0; i < m_relationList.size(); i++ )
    {
        if ( m_relationList[ i ].sourceFilename() == url.path() )
        {
            KLSourceCodeToASMRelation addMe( m_relationList[ i ] );
            addMe.setSortByASMAdress( false );
            thisUrlOnly.append( addMe );
        }
    }
    qSort( thisUrlOnly );
    for ( unsigned int i=0; i < thisUrlOnly.size(); i++ )
    {
        if ( thisUrlOnly[ i ].line() >= (unsigned int) line )
        {
            return thisUrlOnly[ i ].line();
        }
    }
    if ( thisUrlOnly.size() == 0 )
    {
        return -1;
    }
    return thisUrlOnly[ thisUrlOnly.size()-1 ].line();
}

void KLDebugger::confirmBreakpoints( )
{
    for ( unsigned int i=0; i<m_breakpoints.size(); i++ )
    {
        KLDebuggerBreakpoint bp = m_breakpoints[ i ];
        bp.setRealBreakpointLine( seekToNextExecutableLine( bp.url(), bp.line() ) );
        m_breakpoints[ i ] = bp;
    }
}
