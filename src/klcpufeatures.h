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
#ifndef KLCPUFEATURES_H
#define KLCPUFEATURES_H

#include <qstring.h>
#include <qstringlist.h>
#include <QList>

#include <qdom.h>
#include "klcpufuses.h"
#include <vector>
#include "klcpuregisterdescription.h"

using namespace std;

/**
This class stores the features of a CPU.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLCPUFeatures{
public:
    KLCPUFeatures();
    KLCPUFeatures( const QString& name, const QString& mmcuFlag,
                   const QString& arch, int maxClockSpeedHz,
                   int internalRAMSize, int EEPROMSize,
                   bool allowsExternalRAM, int maxExternalRAMSize,
                   int flashEEPROMSize );
    KLCPUFeatures( const QString& name, const QString& mmcuFlag, const QString& arch,
                   int flashEEPROMSize, int EEPROMSize, int internalRAMSize,
                   int maxIOPins, int maxClockSpeedHz, int maxExtRAM, double minVCC, double maxVCC,
                   int number16BitTimers, int number8BitTimers,
                   int pwmChannels, bool rtc, bool spi,
                   int uarts, bool twi, bool isp, int adChannels, bool analogComparator,
                   bool brownOutDetect, bool watchdog, bool onChipOsc, bool hwMult,
                   int interrupts, int extInt, bool selfProgMem, const QString& packages);
    KLCPUFeatures( QDomDocument & document, QDomElement & parent );

    bool operator<(const KLCPUFeatures& other) const;

    ~KLCPUFeatures();

    void setName(const QString& theValue)
    { m_name = theValue; }
    QString name() const
    { return m_name; }

    int internalRAMSize() const
    { return m_internalRAMSize; }
    int maxClockSpeedHz() const
    { return m_maxClockSpeedHz; }
    int EEPROMSize() const
    { return m_EEPROMSize; }
    int flashEEPROMSize() const
    { return m_flashEEPROMSize; }

    QString mmcuFlag() const
    { return m_mmcuFlag; }

    /**
     * This function is used to get the register descriptions.
     * These descriptions are parsed from a config file named
     * registers.xml, which is located in the standard KDE
     * resource directory of KontrollerLab.
     * @return A list of register descriptions
     */
    KLCPURegisterDescriptionList registerDescriptions();

    /**
     * Reads the register definitions for this CPU from the registers.xml file.
     * The result is stored in m_registerDescriptions directly.
     * @param doc The DOM document
     * @param elem The DOM element to read the info from
     */
    void readRegDefsFromDOMElement( QDomDocument& doc, QDomElement& elem );

    void createDOMElement( QDomDocument & document, QDomElement & parent );

    void setFuses ( const KLCPUFuses& theValue ) { m_fuses = theValue; }
    KLCPUFuses fuses() const { return m_fuses; }

protected:
    void init();
    void createAndAddDOM( QDomDocument & document, QDomElement & feat,
                          const QString& name, const QString& text );
    void parseSettings( const QString& name, const QString& value );
    QString m_name, m_arch;
    QString m_mmcuFlag;
    int m_maxClockSpeedHz, m_internalRAMSize;
    int m_EEPROMSize, m_maxExternalRAMSize, m_flashEEPROMSize;
    bool m_allowsExternalRAM;
    int m_maxIOPins;
    double m_minVCC, m_maxVCC;
    int m_16BitTimers, m_8BitTimers, m_PWM_channels;
    bool m_RTC, m_SPI;
    int m_UARTs;
    bool m_TWI, m_ISP;
    int m_ADChannels;
    bool m_analogComparators, m_brownOutDetect, m_watchdog, m_onChipOscillator;
    bool m_hardwareMultiplier;
    int m_interrupts, m_externalInterrupts;
    bool m_selfProgramMemory;
    QString m_packages;
    KLCPUFuses m_fuses;
    KLCPURegisterDescriptionList m_registerDescriptions;
};


class KLCPUFeaturesList : public QList< KLCPUFeatures >
{
public:
    void readFromDOMElement( QDomDocument & document, QDomElement & parent );
    void createDOMElement( QDomDocument & document, QDomElement & parent );
};

#endif
