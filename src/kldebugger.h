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
#ifndef KLDEBUGGER_H
#define KLDEBUGGER_H

#include <qobject.h>
#include <QList>
#include <qtimer.h>

#define DEBUGGER_BAUD_RATE 19200
#define REQUEST_TIMEOUT 100
#define DEBUGGER_RAM_SIZE 65536

// Config settings keys:
#define DEBUGGER_OBJDUMP_COMMAND          "DEBUGGER_OBJDUMP_COMMAND"
#define DEBUGGER_COM_PORT                 "DEBUGGER_COM_PORT"
#define DEBUGGER_COM_BAUD                 "DEBUGGER_COM_BAUD"
#define DEBUGGER_COM_UBRR                 "DEBUGGER_COM_UBRR"
#define DEBUGGER_MONITOR_CATCH_INTERRUPTS "DEBUGGER_MONITOR_CATCH_INTERRUPTS"
#define DEBUGGER_INTERRUPT_COUNT          "DEBUGGER_INTERRUPT_COUNT"
#define DEBUGGER_MEMORY_VIEW_BYTECOUNT    "DEBUGGER_MEMORY_VIEW_BYTECOUNT"
#define DEBUGGER_MAPPINGS                 "DEBUGGER_MAPPINGS"


class KLSerialTerminalWidget;
class KontrollerLab;
class KLMemoryViewWidget;
class KLSourceCodeToASMRelation;
class KLASMInstruction;
class KLDebuggerBreakpoint;

#include "klavrflash.h"
#include "klavrram.h"
#include "klavrcpu.h"
#include <kurl.h>
#include "klcharvector.h"
#include "kldebuggermemorymapping.h"


class KLControllerRequest
{
public:
    KLControllerRequest( bool read=true, unsigned int address=0, unsigned char writeValue=0 )
    {
        m_address = address;
        m_read = read;
        // In case we write:
        m_writeValue = writeValue;
        if ( m_read && (m_writeValue != 0) )
            qWarning("Write value != 0 in read request. %s:%d", __FILE__, __LINE__);
    }
    bool readRequest() const { return m_read; }
    bool writeRequest() const { return !m_read; }
    unsigned int address() const { return m_address; }
    unsigned char writeValue() const { return m_writeValue; }
protected:
    bool m_read;
    unsigned int m_address;
    unsigned char m_writeValue;
};


typedef enum { DBG_Off = 0, DBG_ReadIn = 1, DBG_Ready = 2,
               DBG_Started = 3, DBG_Paused = 4,
               DBG_StopWhenCLineChanges = 5,
               DBG_StopWhenCLineChangesSteppingOverFunctions = 6,
               DBG_RunToCLine = 7, DBG_StopWhenStopCallCouterReached = 8 }
        KLDebugger_State;

/**
This is the debugger class that implements the basic features needed for debugging.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLDebugger : public QObject
{
Q_OBJECT
public:
    KLDebugger(KLSerialTerminalWidget* serTerm, KontrollerLab *parent = 0, const char *name = 0);

    ~KLDebugger();

    void setMemoryViewWidget( KLMemoryViewWidget* wdg );

    void writeMemoryCell( unsigned int adr, unsigned char val )
    {
        writeMemoryCell( adr, val, 1, 0, 0, 0, 0, 0, 0 );
    }
    void writeMemoryCell( unsigned int adr, unsigned char val,
                          unsigned int adr2, unsigned char val2 )
    {
        writeMemoryCell( adr, val, 2, adr2, val2, 0, 0, 0, 0 );
    }
    void writeMemoryCell( unsigned int adr, unsigned char val,
                          unsigned int adr2, unsigned char val2,
                          unsigned int adr3, unsigned char val3 )
    {
        writeMemoryCell( adr, val, 3, adr2, val2, adr3, val3, 0, 0 );
    }
    void writeMemoryCell( unsigned int adr, unsigned char val,
                          unsigned int adr2, unsigned char val2,
                          unsigned int adr3, unsigned char val3,
                          unsigned int adr4, unsigned char val4 )
    {
        writeMemoryCell( adr, val, 4, adr2, val2, adr3, val3, adr4, val4 );
    }

    // This lets You access the buffer of the memory cell.
    // It returns immediately. If in DM Mode, the registered
    // memory view's slotMemoryReadCompleted( adr, val ) slot gets
    // called, as soon as the controller responds.
    void readMemoryCell( unsigned int adr )
    {
        readMemoryCell( adr, 1, 0, 0 );
    }
    void readMemoryCell( unsigned int adr,
                         unsigned int adr2 )
    {
        readMemoryCell( adr, 2, adr2, 0 );
    }
    void readMemoryCell( unsigned int adr,
                         unsigned int adr2,
                         unsigned int adr3 )
    {
        readMemoryCell( adr, 3, adr2, adr3 );
    }
    void readMemoryCell( unsigned int adr,
                         unsigned int adr2,
                         unsigned int adr3,
                         unsigned int adr4 )
    {
        readMemoryCell( adr, 4, adr2, adr3, adr4 );
    }



    // To enable the direct memory debug mode:
    void setDMMode( bool val );
    // To enable the in circuit debug mode:
    void setICDMode( bool val );

    unsigned int requestCount() const { return m_requests.count(); }
    bool inDMMode() const { return m_DMMode; }
    bool inICDMode() const { return m_ICDMode; }
    bool inPCOnlyMode() const { return !( m_DMMode || m_ICDMode ); }
    void sendNextRequest();
    void startDebugger();
    void stopDebugger();
    void playpauseDebugger();
    void runUntilCLineChanges();
    void runUntilCLineChangesSteppingOverFunctions();
    void stepOutOfFunction();
    void runToCLine( const KUrl&, int );
    void toggleBreakpoint( const KUrl& url, int line );
    void confirmBreakpoints();
    int seekToNextExecutableLine( const KUrl&, int line );
    void setFlash( unsigned int adr, unsigned int value, unsigned int asmLine );
    void setASMRelations( QList< KLSourceCodeToASMRelation > rlist );
    void setASMInstructions( QList< KLASMInstruction > ilist, const QString& asmLines );
    unsigned char readFromPCRam( unsigned int adr ) { return m_ram.readRAMNoSignal( adr ); }
    unsigned int sreg() const { return 0x5f; }
    unsigned int spl() const { return 0x5d; }
    unsigned int sph() const { return 0x5e; }
    unsigned int zl() const { return 30; }
    unsigned int zh() const { return 31; }
    unsigned int yl() const { return 28; }
    unsigned int yh() const { return 29; }
    unsigned int xl() const { return 26; }
    unsigned int xh() const { return 27; }
    
    KLAVRFlash& flash()
    { return m_flash; }

    KLDebugger_State state() const { return m_state; }

    int getCSourceCodeLineForASMCommand( unsigned int adr, QString& filename );

    KontrollerLab* parent() const { return m_parent; }
    bool isBreakPoint(const KUrl &url, int line );

    QMap< QString, QString > settings() const;

private:
    void writeMemoryCell( unsigned int adr, unsigned char val, int numberOfCells,
                          unsigned int adr2, unsigned char val2,
                          unsigned int adr3, unsigned char val3,
                          unsigned int adr4, unsigned char val4 );
    void readMemoryCell( unsigned int adr, int numberOfCells,
                         unsigned int adr2,
                         unsigned int adr3,
                         unsigned int adr4 );

    
public slots:
    void slotControllerResponded( const KLCharVector& data, bool overrideReadFromUART = false );
    void slotUserSetsMemoryCell( unsigned int adr, unsigned char val );
    void requestTimeout();
    void performStep();
    void slotStepFinished();

protected:
    // This flag tells us if there was at least one line
    // which is visible to the user. This is important
    // if the user presses "step over" before the first
    // line was executed, because the call counter can
    // be wrong then and then the stepping never finishes.
    // Example: A call to the main routine -> call counter
    // is 1, m_stopCallCounter would be 0 -> no return!
    // To avoid this, m_atLeastOneSrcLine is checked!
    bool m_atLeastOneSrcLine;
    bool m_DMMode;
    bool m_ICDMode;
    unsigned int m_stopCallCounter;
    KLDebugger_State m_state;
    KLMemoryViewWidget* m_memoryViewWidget;
    KLSerialTerminalWidget* m_serialTerminalWidget;
    QList< KLSourceCodeToASMRelation > m_relationList;
    QList< KLControllerRequest > m_requests;
    QList< KLDebuggerBreakpoint > m_breakpoints;
    KontrollerLab* m_parent;
    KLDebuggerMemoryMappingList m_memoryMappingList;
    QTimer m_requestTimeout;
    QTimer m_cycleTimer;
    QString m_asmLines;
    KLAVRFlash m_flash;
    KLAVRRAM m_ram;
    KLAVRCPU m_cpu;
    int m_CLineToCompare;
    KUrl m_CFileToCompare;
};

#endif
