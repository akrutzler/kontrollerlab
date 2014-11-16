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
#ifndef KLAVRFLASH_H
#define KLAVRFLASH_H

#include <QDebug>
#include <qobject.h>
#include <QList>
#include <vector>
#include <qstringlist.h>

class KLDebugger;
class KLDocument;

/**
This class stores the data stored in flash.
It emits signals when it is read and written.


	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLAVRFlash : public QObject
{
Q_OBJECT
public:
    /**
     * Initializes the flash memory for the program.
     * @param parent The debugger object, which creates ths object.
     * @param name An optional name.
     */
    KLAVRFlash(KLDebugger *parent = 0, const char *name = 0);

    ~KLAVRFlash();

    /**
     * Read the flash memory at a certain location
     * @param location The memory cell. location references a byte in the
     * flash memory, not a word or instruction!
     * @return The byte at the position location.
     */
    unsigned int read( unsigned int location );
    /**
     * Writes a byte to the flash memory
     * @param location The location of the byte
     * @param value The value to be written
     */
    void write( unsigned int location, unsigned int value );

    /**
     * Used to store the corresponding flash memory address
     * for an assembler line.
     * @param adr The flash memory address
     * @param theValue The corresponding assembler line
     */
    void setFlashAddressToASMLine( const unsigned int adr,
                                   const unsigned int theValue )
    { m_flashAddressToASMLine[adr] = theValue; }

    /**
     * To be aware of which source (for example, C code) line
     * corresponds to an address in the flash memory, this
     * routine is used.
     * @param adr The address in the flash memory
     * @param line The line in the source code, starting with 0
     * @param sourcePt The number of the source. This number
     * is the index of the source in the project.
     */
    void setSourceAndLineInSource( const unsigned int adr,
                                   const int line,
                                   const int sourcePt )
    {
        m_lineInSrc[ adr ] = line;
        m_sourceNr[ adr ] = sourcePt;
    }

    /**
     * Used to know the flash address for an assembler line.
     * @param adr The address of the flash memory
     * @return The assembler line corresponding to the address adr.
     */
    unsigned int flashAddressToASMLine( const unsigned int& adr ) const
    { return m_flashAddressToASMLine[ adr ]; }

    /**
     * Used to know the know the line of code in a source file for a given
     * flash memory address.
     * @param adr The address in the flash memory.
     * @return The line of code in the corresponding source file for the flash
     * memory address 
     */
    int lineInSrc( const unsigned int adr ) const
    { return m_lineInSrc[ adr ]; }

    /**
     * Used to know the right source code index.
     * This source code index is the index of the source
     * in the list of sources in the project.
     * @param adr The flash memory address
     * @return The index of the source corresponding to the
     * flash memory address adr.
     */
    int sourceNr( const unsigned int adr ) const
    { return m_sourceNr[ adr ]; }

    /**
     * Debug only routine.
     */
    void printDebug() const;

    /**
     * The highest valid address is the address of the
     * last flash memory cell which has meaningful content.
     * @param theValue The value to be set as the highest
     * valid address.
     */
    void setHighestValidAdress(unsigned int theValue)
    { m_highestValidAdress = theValue; }

    /**
     * See setHighestValidAdress
     * @return The highest valid address in the flash memory.
     */
    unsigned int highestValidAdress() const
    { return m_highestValidAdress; }

    /**
     * Resizes the flash memory. Used, e.g., when we change the
     * controller.
     * @param newSize The new size of the flash memory
     */
    void resize( unsigned int newSize );

    /**
     * @return The total flash size of the current controller.
     */
    unsigned int flashSize() const { return m_flash.size(); }

    /**
     * The sources string list is used to store the names
     * of the source files. The list items of theValue correspond
     * to the ones returned by the sourceNr routine.
     * @param theValue The list of source code file names
     */
    void setSources(const QStringList& theValue) { m_sources = theValue; }

    /**
     * @return The source file name list
     */
    QStringList& sources() { return m_sources; }

protected:
    KLDebugger* m_parent;
    unsigned int m_highestValidAdress;
    std::vector< unsigned int > m_flash;
    std::vector< unsigned int > m_flashAddressToASMLine;
    std::vector< int > m_lineInSrc;
    std::vector< int > m_sourceNr;
    QStringList m_sources;
};

#endif
