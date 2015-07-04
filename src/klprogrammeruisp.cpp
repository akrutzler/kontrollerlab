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
#include "klprogrammeruisp.h"
#include <klocale.h>
#include <qregexp.h>
#include "klproject.h"
#include "kldocumentview.h"


KLProgrammerUISP::KLProgrammerUISP(KLProcessManager* processManager, KLProject* project)
 : KLProgrammerInterface(processManager, project)
{
    m_runningProc = false;
    project->setProgrammerBusy( false );


    // Possible values for -dprog:
    m_programmerTypes["avr910"] = i18n("Standard Atmel Serial Programmer/Atmel Low Cost Programmer");
    m_programmerTypesKeyList.append("avr910");
    m_serialProgrammer.append("avr910");
    m_programmerTypes["pavr"] = i18n("pavr, see http://www.avr1.org/pavr/pavr.html");
    m_programmerTypesKeyList.append("pavr");
    m_parallelProgrammer.append("pavr");
    m_programmerTypes["stk500"] = i18n("Atmel STK500");
    m_programmerTypesKeyList.append("stk500");
    m_serialProgrammer.append("stk500");
    m_programmerTypes["avrisp"] = i18n("Atmel AVRISP (STK500 over SPI only)");
    m_programmerTypesKeyList.append("avrisp");
    m_parallelProgrammer.append("avrisp");
    m_programmerTypes["dapa"] = i18n("Direct AVR Parallel Access");
    m_programmerTypesKeyList.append("dapa");
    m_parallelProgrammer.append("dapa");
    m_programmerTypes["stk200"] = i18n("Parallel Starter Kit STK200, STK300");
    m_programmerTypesKeyList.append("stk200");
    m_parallelProgrammer.append("stk200");
    m_programmerTypes["abb"] = i18n("Altera ByteBlasterMV Parallel Port Download Cable");
    m_programmerTypesKeyList.append("abb");
    m_parallelProgrammer.append("abb");
    m_programmerTypes["atdh"] = i18n("Atmel-ISP Download Cable");
    m_programmerTypesKeyList.append("atdh");
    m_parallelProgrammer.append("atdh");
    m_programmerTypes["paravrisp"] = i18n("Parallel port programmer (unknown origin)");
    m_programmerTypesKeyList.append("paravrisp");
    m_parallelProgrammer.append("paravrisp");
    m_programmerTypes["bsd"] = i18n("http://www.bsdhome.com/avrdude/ (parallel)");
    m_programmerTypesKeyList.append("bsd");
    m_parallelProgrammer.append("bsd");
    m_programmerTypes["fbprg"] = i18n("http://ln.com.ua/~real/avreal/adapters.html (parallel)");
    m_programmerTypesKeyList.append("fbprg");
    m_parallelProgrammer.append("fbprg");
    m_programmerTypes["dt006"] = i18n("http://www.dontronics.com/dt006.html (parallel)");
    m_programmerTypesKeyList.append("dt006");
    m_parallelProgrammer.append("dt006");
    m_programmerTypes["maxi"] = i18n("Investment Technologies Maxi (parallel)");
    m_programmerTypesKeyList.append("maxi");
    m_parallelProgrammer.append("maxi");
    m_programmerTypes["xil"] = i18n("Xilinx HW-JTAG-PC Cable (parallel)");
    m_programmerTypesKeyList.append("xil");
    m_parallelProgrammer.append("xil");
    m_programmerTypes["ett"] = i18n("ETT AVR Programmer V2.0 [from Futurlec] (parallel)");
    m_programmerTypesKeyList.append("ett");
    m_parallelProgrammer.append("ett");
    m_programmerTypes["dm04"] = i18n("Datamekatronik 2004 (parallel)");
    m_programmerTypesKeyList.append("dm04");
    m_parallelProgrammer.append("dm04");
    m_programmerTypes["dasa"] = i18n("serial (RESET=RTS SCK=DTR MOSI=TXD MISO=CTS)");
    m_programmerTypesKeyList.append("dasa");
    m_serialProgrammer.append("dasa");
    m_programmerTypes["dasa2"] = i18n("serial (RESET=!TXD SCK=RTS MOSI=DTR MISO=CTS)");
    m_programmerTypesKeyList.append("dasa2");
    m_serialProgrammer.append("dasa2");
    m_programmerTypes["dasa3"] = i18n("serial (RESET=!DTR SCK=RTS MOSI=TXD MISO=CTS)");
    m_programmerTypesKeyList.append("dasa3");
    m_serialProgrammer.append("dasa3");
// Possible values for -dlpt
    m_parallelPorts["ppdev"] = i18n("Linux ppdev");
    m_parallelPortsKeyList.append("ppdev");
    m_parallelPorts["ppi"] = i18n("FreeBSD ppi");
    m_parallelPortsKeyList.append("ppi");
    m_parallelPorts["serial"] = i18n("serial");
    m_parallelPortsKeyList.append("serial");
    m_parallelPorts["0x378"] = i18n("Direct adress 0x378");
    m_parallelPortsKeyList.append("0x378");
    m_parallelPorts["0x278"] = i18n("Direct adress 0x278");
    m_parallelPortsKeyList.append("0x278");
    m_parallelPorts["0x3BC"] = i18n("Direct adress 0x3BC");
    m_parallelPortsKeyList.append("0x3BC");
    m_parallelPorts["/dev/parport0"] = i18n("Parallel port /dev/parport0");
    m_parallelPortsKeyList.append("/dev/parport0");
    m_parallelPorts[USER_KEY] = i18n("User defined port");
    m_parallelPortsKeyList.append(USER_KEY);
// Serial ports
    m_serialPorts["/dev/avr"] = i18n("Default /dev/avr");
    m_serialPortsKeyList.append("/dev/avr");
    m_serialPorts["/dev/ttyS0"] = i18n("Serial port /dev/ttyS0");
    m_serialPortsKeyList.append("/dev/ttyS0");
    m_serialPorts["/dev/ttyS1"] = i18n("Serial port /dev/ttyS1");
    m_serialPortsKeyList.append("/dev/ttyS1");
    m_serialPorts["/dev/ttyS2"] = i18n("Serial port /dev/ttyS2");
    m_serialPortsKeyList.append("/dev/ttyS2");
    m_serialPorts["/dev/ttyS3"] = i18n("Serial port /dev/ttyS3");
    m_serialPortsKeyList.append("/dev/ttyS3");
    m_serialPorts[USER_KEY] = i18n("User defined port");
    m_serialPortsKeyList.append(USER_KEY);
}


KLProgrammerUISP::~KLProgrammerUISP()
{
}


bool KLProgrammerUISP::erase( )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " --erase", this );
    return true;
}


bool KLProgrammerUISP::upload(const KUrl &url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " --upload if=" + validPath(url), this );
    return true;
}


bool KLProgrammerUISP::verify( const KUrl & url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " --verify if=" + validPath(url), this );
    return true;
}


bool KLProgrammerUISP::download( const KUrl & url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " --download of=" + validPath(url), this );
    return true;
}


bool KLProgrammerUISP::ignite(const KUrl &url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " --erase --upload --verify if=" + validPath(url), this );
    return true;
}


bool KLProgrammerUISP::programFuses( QMap< QString, QString > fuses )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    QString cmd = formBaseCommand();
    if ( fuses.contains( FUSE_L ) )
    {
        cmd += " --wr_fuse_l=" + fuses[ FUSE_L ];
    }
    if ( fuses.contains( FUSE_H ) )
    {
        cmd += " --wr_fuse_h=" + fuses[ FUSE_H ];
    }
    if ( fuses.contains( FUSE_E ) )
    {
        cmd += " --wr_fuse_e=" + fuses[ FUSE_E ];
    }
    if ( fuses.contains( FUSE_LOCK ) )
    {
        cmd += " --wr_lock=" + fuses[ FUSE_LOCK ];
    }


    m_processManager->addProcess( cmd, this );
    return true;
}


QString KLProgrammerUISP::attribute( const QString & attr, const QString confKey ) const
{
    QString retVal;
    if ( m_configuration.contains( confKey ) )
    {
        retVal = m_configuration[ confKey ];
        if ( (!retVal.isNull()) && (!retVal.isEmpty()) )
            return " " + attr + retVal;
    }
    return "";
}

QString KLProgrammerUISP::formBaseCommand( ) const
{
    QString cmd = m_configuration[ PROGRAMMER_COMMAND ];
    cmd += attribute( "-dprog=", UISP_PROGRAMMER_TYPE );
    if ( m_configuration[ UISP_SPECIFY_PART ] == TRUE_STRING )
        cmd += " -dpart=" + m_project->currentCpuFeatures().name().toUpper();
    if ( m_serialProgrammer.indexOf( attribute( "", UISP_PROGRAMMER_TYPE ).trimmed() ) >= 0 )
    {
        cmd += attribute( "-dserial=", UISP_SERIAL_PORT );
        cmd += attribute( "-dspeed=", UISP_SERIAL_SPEED );
    }
    else
    {
        cmd += attribute( "-dlpt=", UISP_PARALLEL_PORT );
        if ( m_configuration[ UISP_PARALLEL_NO_DATA_POLLING ] == TRUE_STRING )
            cmd += " -dno-poll";
        if ( m_configuration[ UISP_PARALLEL_DISABLE_RETRIES ] == TRUE_STRING )
            cmd += " -dno-retry";
        if ( m_configuration[ UISP_PARALLEL_AT89S ] == TRUE_STRING )
            cmd += " -d89";
        cmd += attribute( "-dvoltage=", UISP_PARALLEL_VOLTAGE );
        cmd += attribute( "-dt_sck=", UISP_PARALLEL_SCK_HIGH_LOW_TIME );
        cmd += attribute( "-dt_wd_flash=", UISP_PARALLEL_FLASH_MAX_WRITE_DELAY );
        cmd += attribute( "-dt_wd_eeprom=", UISP_PARALLEL_EEPROM_MAX_WRITE_DELAY );
        cmd += attribute( "-dt_reset=", UISP_PARALLEL_RESET_HIGH_TIME );
    }
    if ( m_configuration[ UISP_STK500_USE_HIGH_VOLTAGE ] == TRUE_STRING )
        cmd += " -dhiv";
    return cmd;
}

bool KLProgrammerUISP::readFuses( )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_readFusesBackannotate = 1;
    m_processManager->addProcess( formBaseCommand() + " --rd_fuses", this );
    return true;
}

QMap< QString, QString > KLProgrammerUISP::parseFuses( const QString & stdout )
{
    QRegExp reLow( "^.*\\s+low\\s+.*0x([0-9a-f]*)\\s*$" );
    QRegExp reHigh( "^.*\\s+high\\s+.*0x([0-9a-f]*)\\s*$" );
    QRegExp reExt( "^.*\\s+extended\\s+.*0x([0-9a-f]*)\\s*$" );
    QRegExp reLock( "^.*\\s*lock\\s+.*0x([0-9a-f]*)\\s*$" );
    QMap< QString, QString > retVal;

    QStringList lines;
    QStringList::iterator it;
    lines = stdout.split("\n");
    for (it = lines.begin(); it != lines.end(); ++it)
    {
        QString line = (*it).toLower();
        if ( reLow.indexIn( line.toLower() ) != -1 )
            retVal[ FUSE_L ] = reLow.cap( 1 );
        if ( reHigh.indexIn( line.toLower() ) != -1 )
            retVal[ FUSE_H ] = reHigh.cap( 1 );
        if ( reLock.indexIn( line.toLower() ) != -1 )
            retVal[ FUSE_LOCK ] = reLock.cap( 1 );
        if ( reExt.indexIn( line.toLower() ) != -1 )
            retVal[ FUSE_E ] = reExt.cap( 1 );
    }
    return retVal;
}


void KLProgrammerUISP::writeSTK500( double a_ref, double v_sup, double clk )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() +
            QString(" --wr_aref=%1 --wr_vtg=%2 --wr_osc=%3").arg(a_ref).arg(v_sup).arg(clk, 0, 'f', 2),
            this );
}

void KLProgrammerUISP::readSTK500(  )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_stk500Backannotate = 1;
    m_processManager->addProcess( formBaseCommand() +
            " --rd_aref --rd_vtg --rd_osc", this );
}

