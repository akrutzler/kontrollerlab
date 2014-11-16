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
#include "klcpufeatures.h"
#include <qdom.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <qfile.h>
#include <Qt3Support>


#define TRUE_STRING "TRUE"
#define FALSE_STRING "FALSE"

KLCPUFeatures::KLCPUFeatures( const QString & name, const QString & mmcuFlag,
                              const QString& arch,
                              int maxClockSpeedHz, int internalRAMSize,
                              int EEPROMSize, bool allowsExternalRAM,
                              int maxExternalRAMSize, int flashEEPROMSize )
{
    init();
    m_name = name;
    m_mmcuFlag = mmcuFlag;
    m_arch = arch;
    m_maxClockSpeedHz = maxClockSpeedHz;
    m_internalRAMSize = internalRAMSize;
    m_EEPROMSize = EEPROMSize;
    m_allowsExternalRAM = allowsExternalRAM;
    m_maxExternalRAMSize = maxExternalRAMSize;
    m_flashEEPROMSize = flashEEPROMSize;
    // m_registerDescriptions = registerDescriptions();
}


KLCPUFeatures::KLCPUFeatures( QDomDocument & document, QDomElement & parent )
{
    for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        if ( parent.nodeName().upper() == "FEATURE_SET" )
        {
            if ( !n.isElement() )
                continue;
            QDomElement elem = n.toElement();
            if ( elem.nodeName() != "FUSES" )
                parseSettings( elem.nodeName(), elem.text() );
            else
                m_fuses = KLCPUFuses( document, elem );
        }
    }
    // m_registerDescriptions = registerDescriptions();
}


KLCPUFeatures::~KLCPUFeatures()
{
}

KLCPUFeatures::KLCPUFeatures( )
{
    init();
}

KLCPUFeatures::KLCPUFeatures( const QString & name, const QString & mmcuFlag,
                              const QString & arch, int flashEEPROMSize, int EEPROMSize,
                              int internalRAMSize, int maxIOPins, int maxClockSpeedHz, int maxExtRAM,
                              double minVCC, double maxVCC, int number16BitTimers,
                              int number8BitTimers, int pwmChannels, bool rtc, bool spi,
                              int uarts, bool twi, bool isp,
                              int adChannels, bool analogComparator, bool brownOutDetect,
                              bool watchdog, bool onChipOsc, bool hwMult, int interrupts,
                              int extInt, bool selfProgMem, const QString & packages )
{
    init();
    m_name = name;
    m_arch = arch;
    m_mmcuFlag = mmcuFlag;
    m_maxClockSpeedHz = maxClockSpeedHz;
    m_internalRAMSize = internalRAMSize;
    m_EEPROMSize = EEPROMSize;
    m_maxExternalRAMSize = maxExtRAM;
    m_allowsExternalRAM = maxExtRAM>0;
    m_flashEEPROMSize = flashEEPROMSize;
    m_maxIOPins = maxIOPins;
    m_minVCC = minVCC;
    m_maxVCC = maxVCC;
    m_16BitTimers = number16BitTimers;
    m_8BitTimers = number8BitTimers;
    m_PWM_channels = pwmChannels;
    m_RTC = rtc;
    m_SPI = spi;
    m_UARTs = uarts;
    m_TWI = twi;
    m_ISP = isp;
    m_ADChannels = adChannels;
    m_analogComparators = analogComparator;
    m_brownOutDetect = brownOutDetect;
    m_watchdog = watchdog;
    m_onChipOscillator = onChipOsc;
    m_hardwareMultiplier = hwMult;
    m_interrupts = interrupts;
    m_externalInterrupts = extInt;
    m_selfProgramMemory = selfProgMem;
    m_packages = packages;
}

bool KLCPUFeatures::operator <( const KLCPUFeatures & other ) const
{
    return m_name < other.name();
}


void KLCPUFeatures::createDOMElement( QDomDocument & document, QDomElement & parent )
{
    QDomElement feat = document.createElement( "FEATURE_SET" );

    createAndAddDOM( document, feat, "NAME", m_name );
    createAndAddDOM( document, feat, "ARCH", m_arch );
    createAndAddDOM( document, feat, "MMCU_FLAG", m_mmcuFlag );
    createAndAddDOM( document, feat, "MAX_CLOCK_SPEED_HZ", QString::number( m_maxClockSpeedHz ) );
    createAndAddDOM( document, feat, "INTERNAL_RAM_SIZE", QString::number( m_internalRAMSize ) );
    createAndAddDOM( document, feat, "EEPROM_SIZE", QString::number( m_EEPROMSize ) );
    createAndAddDOM( document, feat, "MAX_EXTERNAL_RAM_SIZE", QString::number( m_maxExternalRAMSize ) );
    createAndAddDOM( document, feat, "FLASH_EEPROM_SIZE", QString::number( m_flashEEPROMSize ) );
    createAndAddDOM( document, feat, "ALLOWS_EXTERNAL_RAM", m_allowsExternalRAM ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "MAX_IO_PINS", QString::number( m_maxIOPins ) );
    createAndAddDOM( document, feat, "MIN_VCC", QString::number( m_minVCC ) );
    createAndAddDOM( document, feat, "MAX_VCC", QString::number( m_maxVCC ) );
    createAndAddDOM( document, feat, "BIT_16_TIMERS", QString::number( m_16BitTimers ) );
    createAndAddDOM( document, feat, "BIT_8_TIMERS", QString::number( m_8BitTimers ) );
    createAndAddDOM( document, feat, "PWM_CHANNELS", QString::number( m_PWM_channels ) );
    createAndAddDOM( document, feat, "RTC", m_RTC ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "SPI", m_SPI ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "UARTS", QString::number( m_UARTs ) );
    createAndAddDOM( document, feat, "TWI", m_TWI ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "ISP", m_ISP ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "AD_CHANNELS", QString::number( m_ADChannels ) );
    createAndAddDOM( document, feat, "ANALOG_COMPARATORS", m_analogComparators ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "BROWN_OUT_DETECT", m_brownOutDetect ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "WATCHDOG", m_watchdog ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "ON_CHIP_OSCILLATOR", m_onChipOscillator ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "HARDWARE_MULTIPLIER", m_hardwareMultiplier
            ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "INTERRUPTS", QString::number( m_interrupts ) );
    createAndAddDOM( document, feat, "EXTERNAL_INTERRUPTS", QString::number( m_externalInterrupts ) );
    createAndAddDOM( document, feat, "SELF_PROGRAM_MEMORY", m_selfProgramMemory ? TRUE_STRING : FALSE_STRING );
    createAndAddDOM( document, feat, "PACKAGES", m_packages );
    m_fuses.createDOMElement( document, feat );
    parent.appendChild( feat );
}

void KLCPUFeaturesList::createDOMElement( QDomDocument & document, QDomElement & parent )
{
    QList<KLCPUFeatures>::iterator it;
    for (it=begin(); it!=end(); ++it)
    {
        (*it).createDOMElement( document, parent );
    }
}

void KLCPUFeaturesList::readFromDOMElement( QDomDocument & document, QDomElement & parent )
{
    for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        // qDebug( "n.nodeName() = %s", n.nodeName().ascii() );
        if ( n.nodeName().upper() == "FEATURE_SET" )
        {
            if ( !n.isElement() )
                continue;
            QDomElement elem = n.toElement();
            append( KLCPUFeatures( document, elem ) );
        }
    }
}

void KLCPUFeatures::createAndAddDOM( QDomDocument & document, QDomElement & feat,
                                     const QString & name, const QString & text )
{
    QDomElement cur = document.createElement( name );
    QDomText textDOM = document.createTextNode( text );
    cur.appendChild( textDOM );
    feat.appendChild( cur );
}

void KLCPUFeatures::init( )
{
    m_maxIOPins = 0;
    m_minVCC = 1.8;
    m_maxVCC = 5.5;
    m_16BitTimers = 0;
    m_8BitTimers = 1;
    m_PWM_channels = 0;
    m_RTC = 0;
    m_SPI = 0;
    m_UARTs = 1;
    m_TWI = true;
    m_ISP = true;
    m_ADChannels = 0;
    m_analogComparators = 0;
    m_brownOutDetect = true;
    m_watchdog = true;
    m_onChipOscillator = true;
    m_hardwareMultiplier = false;
    m_interrupts = 0;
    m_externalInterrupts = 0;
    m_selfProgramMemory = false;
    m_packages = "";
    
    m_name = "";
    m_mmcuFlag = "";
    m_arch = "";
    m_maxClockSpeedHz = 4000000;
    m_internalRAMSize = 32;
    m_EEPROMSize = 64;
    m_allowsExternalRAM = false;
    m_maxExternalRAMSize = 0;
    m_flashEEPROMSize = 1024;
}

void KLCPUFeatures::parseSettings(const QString & name, const QString & value)
{
    // qDebug( "parse %s : %s", name.ascii(), value.ascii() );
    bool ok = true;
    if ( name.upper() == "NAME" ) m_name = value;
    else if ( name.upper() == "ARCH" ) m_arch = value;
    else if ( name.upper() == "MMCU_FLAG" ) m_mmcuFlag = value;
    else if ( name.upper() == "MAX_CLOCK_SPEED_HZ" ) m_maxClockSpeedHz = value.toUInt( &ok );
    else if ( name.upper() == "INTERNAL_RAM_SIZE" ) m_internalRAMSize = value.toUInt( &ok );
    else if ( name.upper() == "EEPROM_SIZE" ) m_EEPROMSize = value.toUInt( & ok );
    else if ( name.upper() == "MAX_EXTERNAL_RAM_SIZE" ) m_maxExternalRAMSize = value.toUInt( &ok );
    else if ( name.upper() == "FLASH_EEPROM_SIZE" ) m_flashEEPROMSize = value.toUInt( &ok );
    else if ( name.upper() == "ALLOWS_EXTERNAL_RAM" ) m_allowsExternalRAM = value.upper() == TRUE_STRING;
    else if ( name.upper() == "MAX_IO_PINS" ) m_maxIOPins = value.toUInt( &ok );
    else if ( name.upper() == "MIN_VCC" ) m_minVCC = value.toDouble( &ok );
    else if ( name.upper() == "MAX_VCC" ) m_maxVCC = value.toDouble( &ok );
    else if ( name.upper() == "BIT_16_TIMERS" ) m_16BitTimers = value.toUInt( &ok );
    else if ( name.upper() == "BIT_8_TIMERS" ) m_8BitTimers = value.toUInt( &ok );
    else if ( name.upper() == "PWM_CHANNELS" ) m_PWM_channels = value.toUInt( &ok );
    else if ( name.upper() == "RTC" ) m_RTC = value == TRUE_STRING;
    else if ( name.upper() == "SPI" ) m_SPI = value == TRUE_STRING;
    else if ( name.upper() == "UARTS" ) m_UARTs = value.toUInt( &ok );
    else if ( name.upper() == "TWI" ) m_TWI = value == TRUE_STRING;
    else if ( name.upper() == "ISP" ) m_ISP = value == TRUE_STRING;
    else if ( name.upper() == "AD_CHANNELS" ) m_ADChannels = value.toUInt( &ok );
    else if ( name.upper() == "ANALOG_COMPARATORS" ) m_analogComparators = value == TRUE_STRING;
    else if ( name.upper() == "BROWN_OUT_DETECT" ) m_brownOutDetect = value == TRUE_STRING;
    else if ( name.upper() == "WATCHDOG" ) m_watchdog = value == TRUE_STRING;
    else if ( name.upper() == "ON_CHIP_OSCILLATOR" ) m_onChipOscillator = value == TRUE_STRING;
    else if ( name.upper() == "HARDWARE_MULTIPLIER" ) m_hardwareMultiplier = value == TRUE_STRING;
    else if ( name.upper() == "INTERRUPTS" ) m_interrupts = value.toUInt( &ok );
    else if ( name.upper() == "EXTERNAL_INTERRUPTS" ) m_externalInterrupts = value.toUInt( &ok );
    else if ( name.upper() == "SELF_PROGRAM_MEMORY" ) m_selfProgramMemory = value == TRUE_STRING;
    else if ( name.upper() == "PACKAGES" ) m_packages = value;
    if ( !ok )
        qWarning("Problem with parsing %s = %s", name.ascii(), value.ascii());
}


KLCPURegisterDescriptionList KLCPUFeatures::registerDescriptions()
{
    if ( m_registerDescriptions.forCPU() == m_name )
    {
        return m_registerDescriptions;
    }
        // READ THE CONFIGURATION:

    KStandardDirs *dirs = KGlobal::dirs();
    QString resPathConf = dirs->findResource( "data", "kontrollerlab/registers.xml" );
    // Open it:
    if ( resPathConf.isNull() || resPathConf.isEmpty() )
    {
        qWarning( "Could not locate %s", "registers.xml" );
    }
    else
    {
        KUrl urlConf( resPathConf );
        QFile qfile( urlConf.path() );
        QDomDocument docConf("REGISTER_DESCRIPTIONS");
        QString errorMesg;
        int errorLine, errorCol;
        if ( !docConf.setContent( &qfile, false, &errorMesg, &errorLine, &errorCol ) ) {
            qWarning( "Error in file %s: %d(%d) %s", "registers.xml", errorLine, errorCol, errorMesg.ascii() );
            qfile.close();
            return m_registerDescriptions;
        }
        qfile.close();
        QDomElement docElem = docConf.documentElement();
        // readRegDefsFromDOMElement( docConf, docElem );
        m_registerDescriptions.readFromDOMElement( docConf, docElem, m_name );
    }

    return m_registerDescriptions;
}


void KLCPUFeatures::readRegDefsFromDOMElement(QDomDocument &, QDomElement & elem)
{
    m_registerDescriptions.clear();
    if ( elem.nodeName().upper() == "REGISTER_DESCRIPTIONS" )
    {
        QString ver = elem.attribute( "VERSION", "" );
        // if ( !ver.isEmpty() )
        //     qDebug( "Reading register definition file version %s for CPU %s.", ver.ascii(), m_name.ascii() );
        QDomNode n = elem.firstChild();
        while ( !n.isNull() )
        {
            if ( !n.isElement() )
            {
                n = n.nextSibling();
                continue;
            }
            QDomElement e = n.toElement();
            if ( ( e.nodeName().upper() == "CPU" ) &&
                 ( e.attribute( "NAME", "" ).upper() == m_name.upper() ) )
            {
                QDomNode reg = e.firstChild();
                while ( !reg.isNull() )
                {
                    if ( !reg.isElement() )
                    {
                        reg = reg.nextSibling();
                        continue;
                    }
                    QDomElement regEle = reg.toElement();
                    if ( regEle.nodeName().upper() == "REGISTER" )
                    {
                        QString locString = regEle.attribute( "LOC", "-1" );
                        bool ok;
                        int loc = locString.toInt( &ok );
                        if ( ok && (loc >= 0) )
                        {
                            QString regText = regEle.text();
                            QString regName, regDesc = "";
                            if ( regText.contains( "," ) )
                            {
                                // There is a description
                                regName = regText.left( regText.find( "," ) );
                                regName = regName.stripWhiteSpace();
                                regDesc = regText.right( regText.length() - regName.length() - 1 );
                                regDesc = regDesc.stripWhiteSpace();
                            }
                            else
                            {
                                regName = regText.stripWhiteSpace();
                            }
                            m_registerDescriptions.append(
                                KLCPURegisterDescription( loc, regName, regDesc ) );
                        }
                    }
                    reg = reg.nextSibling();
                }
                // We found what we were looking for -> break!
                // qDebug("Found def for %s.", m_name.ascii() );
                break;
            }
            n = n.nextSibling();
        }
    }
    m_registerDescriptions.setForCPU( m_name );
    //qHeapSort( m_registerDescriptions );
}

