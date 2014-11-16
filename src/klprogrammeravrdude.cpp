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
#include "klprogrammeravrdude.h"
#include "klproject.h"
#include <klocale.h>


KLProgrammerAVRDUDE::KLProgrammerAVRDUDE(KLProcessManager* processManager, KLProject* project)
 : KLProgrammerInterface(processManager, project)
{
    m_runningProc = false;
    project->setProgrammerBusy( false );

    // ***********************************
    m_programmerTypes["dasa3"] = i18n("serial port banging, reset=!dtr sck=rts mosi=txd miso=cts");
    m_programmerTypesKeyList.append("dasa3");
    m_serialProgrammer.append("dasa3");
    // ***********************************
    m_programmerTypes["dasa"] = i18n("serial port banging, reset=rts sck=dtr mosi=txd miso=cts");
    m_programmerTypesKeyList.append("dasa");
    m_serialProgrammer.append("dasa");
    // ***********************************
    m_programmerTypes["ponyser"] = i18n("serial port banging, design ponyprog serial");
    m_programmerTypesKeyList.append("ponyser");
    m_serialProgrammer.append("ponyser");
    // ***********************************
    m_programmerTypes["dapa"] = i18n("Direct AVR Parallel Access cable");
    m_programmerTypesKeyList.append("dapa");
    m_parallelProgrammer.append("dapa");
    // ***********************************
    m_programmerTypes["xil"] = i18n("Xilinx JTAG cable");
    m_programmerTypesKeyList.append("xil");
    m_parallelProgrammer.append("xil");
    // ***********************************
    m_programmerTypes["futurlec"] = i18n("Futurlec.com programming cable.");
    m_programmerTypesKeyList.append("futurlec");
    m_parallelProgrammer.append("futurlec");
    // ***********************************
    m_programmerTypes["abcmini"] = i18n("ABCmini Board, aka Dick Smith HOTCHIP");
    m_programmerTypesKeyList.append("abcmini");
    m_parallelProgrammer.append("abcmini");
    // ***********************************
    m_programmerTypes["picoweb"] = i18n("Picoweb Programming Cable");
    m_programmerTypesKeyList.append("picoweb");
    m_parallelProgrammer.append("picoweb");
    // ***********************************
    m_programmerTypes["sp12"] = i18n("Steve Bolt's Programmer");
    m_programmerTypesKeyList.append("sp12");
    m_parallelProgrammer.append("sp12");
    // ***********************************
    m_programmerTypes["alf"] = i18n("Nightshade ALF-PgmAVR");
    m_programmerTypesKeyList.append("alf");
    m_parallelProgrammer.append("alf");
    // ***********************************
    m_programmerTypes["bascom"] = i18n("Bascom SAMPLE programming cable");
    m_programmerTypesKeyList.append("bascom");
    m_parallelProgrammer.append("bascom");
    // ***********************************
    m_programmerTypes["dt006"] = i18n("Dontronics DT006");
    m_programmerTypesKeyList.append("dt006");
    m_parallelProgrammer.append("dt006");
    // ***********************************
    m_programmerTypes["pony-stk200"] = i18n("Pony Prog STK200");
    m_programmerTypesKeyList.append("pony-stk200");
    m_parallelProgrammer.append("pony-stk200");
    // ***********************************
    m_programmerTypes["stk200"] = i18n("STK200");
    m_programmerTypesKeyList.append("stk200");
    m_parallelProgrammer.append("stk200");
    // ***********************************
    m_programmerTypes["bsd"] = i18n("Brian Dean's Programmer");
    m_programmerTypesKeyList.append("bsd");
    m_parallelProgrammer.append("bsd");
    // ***********************************
    m_programmerTypes["pavr"] = i18n("Jason Kyle's pAVR Serial Programmer");
    m_programmerTypesKeyList.append("pavr");
    m_parallelProgrammer.append("pavr");
    // ***********************************
    m_programmerTypes["jtag2"] = i18n("Atmel JTAG ICE mkII");
    m_programmerTypesKeyList.append("jtag2");
    m_parallelProgrammer.append("jtag2");
    // ***********************************
    m_programmerTypes["jtag2fast"] = i18n("Atmel JTAG ICE mkII (fast)");
    m_programmerTypesKeyList.append("jtag2fast");
    m_parallelProgrammer.append("jtag2fast");
    // ***********************************
    m_programmerTypes["jtag2slow"] = i18n("Atmel JTAG ICE mkII (slow)");
    m_programmerTypesKeyList.append("jtag2slow");
    m_parallelProgrammer.append("jtag2slow");
    // ***********************************
    m_programmerTypes["jtag1slow"] = i18n("Atmel JTAG ICE mkI (slow)");
    m_programmerTypesKeyList.append("jtag1slow");
    m_parallelProgrammer.append("jtag1slow");
    // ***********************************
    m_programmerTypes["jtag1"] = i18n("Atmel JTAG ICE mkI");
    m_programmerTypesKeyList.append("jtag1");
    m_parallelProgrammer.append("jtag1");
    // ***********************************
    m_programmerTypes["avr911"] = i18n("Atmel AppNote AVR911 AVROSP");
    m_programmerTypesKeyList.append("avr911");
    m_parallelProgrammer.append("avr911");
    // ***********************************
    m_programmerTypes["avr109"] = i18n("Atmel AppNote AVR109 Boot Loader");
    m_programmerTypesKeyList.append("avr109");
    m_parallelProgrammer.append("avr109");
    // ***********************************
    m_programmerTypes["butterfly"] = i18n("Atmel Butterfly Development Board");
    m_programmerTypesKeyList.append("butterfly");
    m_parallelProgrammer.append("butterfly");
    // ***********************************
    m_programmerTypes["avr910"] = i18n("Atmel Low Cost Serial Programmer");
    m_programmerTypesKeyList.append("avr910");
    m_parallelProgrammer.append("avr910");
    // ***********************************
    m_programmerTypes["stk500v2"] = i18n("Atmel STK500 V2");
    m_programmerTypesKeyList.append("stk500v2");
    m_parallelProgrammer.append("stk500v2");
    // ***********************************
    m_programmerTypes["stk500"] = i18n("Atmel STK500");
    m_programmerTypesKeyList.append("stk500");
    m_parallelProgrammer.append("stk500");
    // ***********************************
    m_programmerTypes["avrisp2"] = i18n("Atmel AVR ISP mkII");
    m_programmerTypesKeyList.append("avrisp2");
    m_parallelProgrammer.append("avrisp2");
    // ***********************************
    m_programmerTypes["avrispv2"] = i18n("Atmel AVR ISP V2");
    m_programmerTypesKeyList.append("avrispv2");
    m_parallelProgrammer.append("avrispv2");
    // ***********************************
    m_programmerTypes["avrisp"] = i18n("Atmel AVR ISP");
    m_programmerTypesKeyList.append("avrisp");
    m_parallelProgrammer.append("avrisp");
    // ***********************************
    QString tmp = "dragon_dw";
    m_programmerTypes[tmp] = i18n("Atmel AVR Dragon in debugWire mode");
    m_programmerTypesKeyList.append(tmp);
    m_serialProgrammer.append(tmp);
    // ***********************************
    tmp = "dragon_hvsp";
    m_programmerTypes[tmp] = i18n("Atmel AVR Dragon in HVSP mode");
    m_programmerTypesKeyList.append(tmp);
    m_serialProgrammer.append(tmp);
    // ***********************************
    tmp = "dragon_pp";
    m_programmerTypes[tmp] = i18n("Atmel AVR Dragon in PP mode");
    m_programmerTypesKeyList.append(tmp);
    m_serialProgrammer.append(tmp);
    // ***********************************
    tmp = "dragon_isp";
    m_programmerTypes[tmp] = i18n("Atmel AVR Dragon in ISP mode");
    m_programmerTypesKeyList.append(tmp);
    m_serialProgrammer.append(tmp);
    // ***********************************
    tmp = "dragon_jtag";
    m_programmerTypes[tmp] = i18n("Atmel AVR Dragon in JTAG mode");
    m_programmerTypesKeyList.append(tmp);
    m_serialProgrammer.append(tmp);
    // ***********************************
    tmp = "usbasp";
    m_programmerTypes[tmp] = i18n("USBasp http://www.fischl.de/usbasp/");
    m_programmerTypesKeyList.append(tmp);
    m_serialProgrammer.append(tmp);

    // Possible values for -P
    m_parallelPorts["/dev/lp0"] = i18n("Parallel port /dev/lp0");
    m_parallelPortsKeyList.append("/dev/lp0");
    m_parallelPorts["/dev/parport0"] = i18n("Parallel port /dev/parport0");
    m_parallelPortsKeyList.append("/dev/parport0");

    m_serialPorts["usb"] = i18n("USB");
    m_serialPortsKeyList.append("usb");
    m_serialPorts["/dev/ttyS0"] = i18n("Serial port /dev/ttyS0");
    m_serialPortsKeyList.append("/dev/ttyS0");
    m_serialPorts["/dev/ttyS1"] = i18n("Serial port /dev/ttyS1");
    m_serialPortsKeyList.append("/dev/ttyS1");
    m_serialPorts["/dev/ttyS2"] = i18n("Serial port /dev/ttyS2");
    m_serialPortsKeyList.append("/dev/ttyS2");
    m_serialPorts["/dev/ttyS3"] = i18n("Serial port /dev/ttyS3");
    m_serialPortsKeyList.append("/dev/ttyS3");
    m_serialPorts["/dev/ttyUSB0"] = i18n("Serial port /dev/ttyUSB0");
    m_serialPortsKeyList.append("/dev/ttyUSB0");

    m_mmcuLookup[QString("ATtiny84").upper()] = "t84";
    m_mmcuLookup[QString("ATtiny44").upper()] = "t44";
    m_mmcuLookup[QString("ATtiny24").upper()] = "t24";
    m_mmcuLookup[QString("ATMEGA1281").upper()] = "m1281";
    m_mmcuLookup[QString("ATMEGA1280").upper()] = "m1280";
    m_mmcuLookup[QString("ATMEGA640").upper()] = "m640";
    m_mmcuLookup[QString("ATtiny85").upper()] = "t85";
    m_mmcuLookup[QString("ATtiny45").upper()] = "t45";
    m_mmcuLookup[QString("ATtiny25").upper()] = "t25";
    m_mmcuLookup[QString("AT90PWM3").upper()] = "pwm3";
    m_mmcuLookup[QString("AT90PWM2").upper()] = "pwm2";
    m_mmcuLookup[QString("ATtiny2313").upper()] = "t2313";
    m_mmcuLookup[QString("ATMEGA168").upper()] = "m168";
    m_mmcuLookup[QString("ATMEGA88").upper()] = "m88";
    m_mmcuLookup[QString("ATMEGA48").upper()] = "m48";
    m_mmcuLookup[QString("ATTINY26").upper()] = "t26";
    m_mmcuLookup[QString("ATMEGA8535").upper()] = "m8535";
    m_mmcuLookup[QString("ATMEGA8515").upper()] = "m8515";
    m_mmcuLookup[QString("ATMEGA8").upper()] = "m8";
    m_mmcuLookup[QString("ATMEGA161").upper()] = "m161";
    m_mmcuLookup[QString("ATMEGA32").upper()] = "m32";
    m_mmcuLookup[QString("ATMEGA6490").upper()] = "m6490";
    m_mmcuLookup[QString("ATMEGA649").upper()] = "m649";
    m_mmcuLookup[QString("ATMEGA3290").upper()] = "m3290";
    m_mmcuLookup[QString("ATMEGA329").upper()] = "m329";
    m_mmcuLookup[QString("ATMEGA169").upper()] = "m169";
    m_mmcuLookup[QString("ATMEGA163").upper()] = "m163";
    m_mmcuLookup[QString("ATMEGA162").upper()] = "m162";
    m_mmcuLookup[QString("ATMEGA644").upper()] = "m644";
    m_mmcuLookup[QString("ATMEGA324").upper()] = "m324";
    m_mmcuLookup[QString("ATMEGA164").upper()] = "m164";
    m_mmcuLookup[QString("ATMEGA16").upper()] = "m16";
    m_mmcuLookup[QString("AT90CAN128").upper()] = "c128";
    m_mmcuLookup[QString("ATMEGA128").upper()] = "m128";
    m_mmcuLookup[QString("ATMEGA64").upper()] = "m64";
    m_mmcuLookup[QString("ATMEGA103").upper()] = "m103";
    m_mmcuLookup[QString("AT90S8535").upper()] = "8535";
    m_mmcuLookup[QString("AT90S8515").upper()] = "8515";
    m_mmcuLookup[QString("AT90S4434").upper()] = "4434";
    m_mmcuLookup[QString("AT90S4433").upper()] = "4433";
    m_mmcuLookup[QString("AT90S2343").upper()] = "2343";
    m_mmcuLookup[QString("AT90S2333").upper()] = "2333";
    m_mmcuLookup[QString("AT90S2313").upper()] = "2313";
    m_mmcuLookup[QString("AT90S4414").upper()] = "4414";
    m_mmcuLookup[QString("AT90S1200").upper()] = "1200";
    m_mmcuLookup[QString("ATtiny15").upper()] = "t15";
    m_mmcuLookup[QString("ATtiny13").upper()] = "t13";
    m_mmcuLookup[QString("ATtiny12").upper()] = "t12";
}


KLProgrammerAVRDUDE::~KLProgrammerAVRDUDE()
{
}


bool KLProgrammerAVRDUDE::erase( )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " -e", this );
    return true;
}


bool KLProgrammerAVRDUDE::upload( const KUrl & url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " -U flash:w:" + validPath(url) + ":i", this );
    return true;
}


bool KLProgrammerAVRDUDE::verify( const KUrl & url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " -U flash:v:" + validPath(url) + ":i", this );
    return true;
}


bool KLProgrammerAVRDUDE::download( const KUrl & url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " -U flash:r:" + validPath(url) + ":i", this );
    return true;
}


bool KLProgrammerAVRDUDE::ignite( const KUrl & url )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + " -U flash:w:" + validPath(url) + ":i", this );
    return true;
}


bool KLProgrammerAVRDUDE::programFuses( QMap< QString, QString > fuses )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    QString cmd = formBaseCommand();
    m_readFusesBackannotate = 0;
    m_processManager->setDontAbortQueueOnError(true);
    if ( fuses.contains( FUSE_L ) )
    {
        m_readFusesBackannotate = 2;
        m_processManager->addProcess( formBaseCommand() + " -U lfuse:w:" + fuses[ FUSE_L ] + ":m", this );
        m_processManager->addProcess( formBaseCommand() + " -U fuse:w:" + fuses[ FUSE_L ] + ":m", this );
    }
    if ( fuses.contains( FUSE_H ) )
    {
        m_readFusesBackannotate++;
        m_processManager->addProcess( formBaseCommand() + " -U hfuse:w:" + fuses[ FUSE_H ] + ":m", this );
    }
    if ( fuses.contains( FUSE_E ) )
    {
        m_readFusesBackannotate++;
        m_processManager->addProcess( formBaseCommand() + " -U efuse:w:" + fuses[ FUSE_E ] + ":m", this );;
    }
    if ( fuses.contains( FUSE_LOCK ) )
    {
        m_readFusesBackannotate++;
        m_processManager->addProcess( formBaseCommand() + " -U lock:w:" + fuses[ FUSE_LOCK ] + ":m", this );
    }

    return true;
}


QString KLProgrammerAVRDUDE::attribute( const QString & attr, const QString confKey ) const
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


QString KLProgrammerAVRDUDE::formBaseCommand( ) const
{
    QString cmd = m_configuration[ PROGRAMMER_COMMAND ] + " ";
    QString mmcu = m_mmcuLookup[m_project->currentCpuFeatures().name().upper()];
    if ( (!mmcu.isEmpty()) && (!mmcu.isNull()) )
        cmd += " -p " + mmcu;
    cmd += attribute( "-b ", AVRDUDE_OVERRIDE_BAUD_RATE );
    cmd += attribute( "-B ", AVRDUDE_SPECIFY_BIT_CLOCK );
    cmd += attribute( "-c ", AVRDUDE_PROGRAMMER_TYPE );
    cmd += attribute( "-P ", AVRDUDE_CONNECTION_PORT );
    cmd += attribute( "-C ", AVRDUDE_EXTERNAL_CONFIG_FILE );
    if ( m_configuration[ AVRDUDE_DISABLE_AUTO_ERASE ] == TRUE_STRING )
        cmd += " -D";
    if ( m_configuration[ AVRDUDE_TEST_MODE ] == TRUE_STRING )
        cmd += " -n";
    if ( m_configuration[ AVRDUDE_OVERRIDE_INVALID_SIGNATURE ] == TRUE_STRING )
        cmd += " -F";
    if ( m_configuration[ AVRDUDE_COUNT_ERASE ] == TRUE_STRING )
        cmd += " -y";
    // Disable safemode:
    return cmd + " -u ";
}


void KLProgrammerAVRDUDE::setEraseCounter( int cnt )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_processManager->addProcess( formBaseCommand() + QString(" -Y %1").arg(cnt), this );
}


bool KLProgrammerAVRDUDE::readFuses( )
{
    m_runningProc = true;
    m_project->setProgrammerBusy( true );

    m_readFusesBackannotate = true;
    m_processManager->setCWD( m_project->projectBaseURL() );
    KUrl fuseURL( m_project->projectBaseURL() );
    fuseURL.addPath( ".fuse" );
    KUrl lfuseURL( m_project->projectBaseURL() );
    lfuseURL.addPath( ".lfuse" );
    KUrl hfuseURL( m_project->projectBaseURL() );
    hfuseURL.addPath( ".hfuse" );
    KUrl efuseURL( m_project->projectBaseURL() );
    efuseURL.addPath( ".efuse" );
    KUrl lockURL( m_project->projectBaseURL() );
    lockURL.addPath( ".lock" );
    // Remove all old fuse files:
    QFile fuseFile( fuseURL.path() );
    fuseFile.remove();
    QFile lfuseFile( lfuseURL.path() );
    lfuseFile.remove();
    QFile hfuseFile( hfuseURL.path() );
    hfuseFile.remove();
    QFile efuseFile( efuseURL.path() );
    efuseFile.remove();
    QFile lockFile( lockURL.path() );
    lockFile.remove();
    // Now let them be read:
    m_readFusesBackannotate = 5;
    m_processManager->setDontAbortQueueOnError( true );
    m_processManager->addProcess( formBaseCommand() + " -U fuse:r:.fuse:r", this );
    m_processManager->addProcess( formBaseCommand() + " -U lfuse:r:.lfuse:r", this );
    m_processManager->addProcess( formBaseCommand() + " -U hfuse:r:.hfuse:r", this );
    m_processManager->addProcess( formBaseCommand() + " -U efuse:r:.efuse:r", this );
    m_processManager->addProcess( formBaseCommand() + " -U lock:r:.lock:r", this );

    return true;
}


QMap< QString, QString > KLProgrammerAVRDUDE::parseFuses( const QString & )
{
    QMap< QString, QString > retVal;
    
    KUrl fuseURL( m_project->projectBaseURL() );
    fuseURL.addPath( ".fuse" );
    KUrl lfuseURL( m_project->projectBaseURL() );
    lfuseURL.addPath( ".lfuse" );
    KUrl hfuseURL( m_project->projectBaseURL() );
    hfuseURL.addPath( ".hfuse" );
    KUrl efuseURL( m_project->projectBaseURL() );
    efuseURL.addPath( ".efuse" );
    KUrl lockURL( m_project->projectBaseURL() );
    lockURL.addPath( ".lock" );
    QFile fuseFile( fuseURL.path() );
    QFile lfuseFile( lfuseURL.path() );
    QFile hfuseFile( hfuseURL.path() );
    QFile efuseFile( efuseURL.path() );
    QFile lockFile( lockURL.path() );
    
    readFromFile( fuseFile, FUSE_L, retVal );
    readFromFile( lfuseFile, FUSE_L, retVal );
    readFromFile( hfuseFile, FUSE_H, retVal );
    readFromFile( efuseFile, FUSE_E, retVal );
    readFromFile( lockFile, FUSE_LOCK, retVal );

    return retVal;
}


void KLProgrammerAVRDUDE::readFromFile( QFile & file, const QString & key, QMap< QString, QString > & map )
{
    QFileInfo finfo( file );

    // qDebug( "fsize is %d", finfo.size());
    if ( file.exists() && (finfo.size()==1) )
    {
        // qDebug("EXISTS: %s", file.name().ascii());
        if (file.open( IO_ReadOnly ))
        {
            int ch = file.getch();
            if ( ch >= 0 )
                map[ key ] = QString("0x%1").arg( ch, 0, 16 );
            // qDebug("%s is %s", key.ascii(), map[key].ascii());
            file.close();
        }
    }
}

QString KLProgrammerAVRDUDE::getPortGUIStringFor( const QString port )
{
    QString retVal;

    retVal = getParallelPortGUIStringFor( port );
    if ( getSerialPortGUIStringFor( port ) != port )
        retVal = getSerialPortGUIStringFor( port );
    // Is it is still not found, return the argument:
    if ( retVal.isEmpty() || retVal.isNull() )
        retVal = port;
    return retVal;
}

QString KLProgrammerAVRDUDE::getPortFor( const QString & port )
{
    QString retVal;

    retVal = getParallelPortFor( port );
    if ( retVal.isEmpty() || retVal.isNull() )
        retVal = getSerialPortFor( port );
    // Is it is still not found, return the argument:
    if ( retVal.isEmpty() || retVal.isNull() )
        retVal = port;
    return retVal;
}


