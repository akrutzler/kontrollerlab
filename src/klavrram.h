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
#ifndef KLAVRRAM_H
#define KLAVRRAM_H

#include <qobject.h>
#include <QList>
#include <qmap.h>
#include <vector>
using namespace std;

class KLDebugger;

/**
 * This class stores the data in the RAM.
 * It emits signals when data is written and read.
 * The debugger does only catch these signals to display it,
 * if neccessary. But the data is sent to the AVR
 * by the notifyRAMWrite or notifyRAMread().
 * If a RAM cell has to be read from the AVR, this is
 * stored in the m_mappedToAVR array here.
 * @author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLAVRRAM : public QObject
{
Q_OBJECT
public:
    KLAVRRAM(KLDebugger *parent = 0, const char *name = 0);

    ~KLAVRRAM();

    /**
     * CAUTION: All locations are absolute, in the sense of
     * how ST and LD would read them!
     * SREG == 0x5F for example!
     * Emits a signalReadRAM signal.
     * @param location The location to be read from RAM.
     * @return The value of location.
     */
    unsigned int readRAM( unsigned int location );

    /**
     * Emits a signalReadRAM signal.
     * @param name The name of the location to be read from RAM.
     * @return The value of location.
     */
    unsigned int readRAM( const QString& name );

    /**
     * CAUTION: All locations are absolute, in the sense of
     * how ST and LD would read them!
     * SREG == 0x5F for example!
     * Emits NO signalReadRAM signal.
     * @param name The name of the location to be read from RAM.
     * @return The value of location.
     */
    unsigned int readRAMNoSignal( unsigned int location );

    /**
     * CAUTION: All locations are absolute, in the sense of
     * how ST and LD would read them!
     * SREG == 0x5F for example!
     * Emits a signalWriteRAM signal.
     * @param location The location to be written in RAM.
     * @param value The new value of the memory cell location.
     */
    void writeRAM( unsigned int location, unsigned char value );

    /**
     * Emits a signalWriteRAM signal.
     * 
     * @param name The name of the memory cell to be written in RAM.
     * @param value The new value of the memory cell name.
     */
    void writeRAM( const QString& name, unsigned char value );

    /**
     * CAUTION: All locations are absolute, in the sense of
     * how ST and LD would read them!
     * SREG == 0x5F for example!
     * Emits NO signalWriteRAM signal.
     * @param location The location to be written in RAM.
     * @param value The new value of the memory cell location.
     */
    void writeRAMNoSignal( unsigned int location, unsigned char value );
    // This routine is now part of the KLDebuggerMemoryMappingList, used mainly
    // in the KLDebugger class.
    // bool mappedToExternalDevice( unsigned int adr );
    /**
     * Clears the RAM.
     * Calls KontrollerLab to clear all memory views too.
     */
    void clear();
signals:
    /**
     * Emitted after RAM read.
     * @param location The read location
     * @param valueRead The read value
     */
    void signalReadRAM( unsigned int location, unsigned char valueRead );
    /**
     * Emitted after RAM write.
     * @param location The written location
     * @param valueWritten The written value
     */
    void signalWriteRAM( unsigned int location, unsigned char valueWritten );
protected:
    vector< unsigned char > m_ram;
    QMap< QString, unsigned int > m_nameToLocationMap;
    bool nameExists( const QString& name );
    KLDebugger* m_parent;
};

#endif
