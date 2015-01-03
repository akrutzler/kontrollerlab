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
#include "klserialterminalwidget.h"
#include <qtextedit.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <kiconloader.h>
#include "kontrollerlab.h"
#include <klocale.h>
#include "klserialportlistener.h"
#include <qregexp.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include "klscope.h"
#include "klhistogram.h"
#include <kconfiggroup.h>


KLSerialTerminalWidget::KLSerialTerminalWidget(KontrollerLab *parent, const char *name)
    :QDialog(parent, name),
      ui(new Ui::KLSerialTerminalWidgetBase)
{
    ui->setupUi(this);
    m_parent = parent;
    m_programmerBusy = false;
    m_terminalInICDMode = false;
    m_wasOpenedBeforeProgramming = false;
    m_serialPort = new KLSerialPort;
    
    QHBoxLayout *lay = new QHBoxLayout( ui->frmScopeHisto, 2 );
    lay->setAutoAdd( true );
    m_scope = new KLScope( ui->frmScopeHisto, "scope", 128, -127, true, false );
    m_histogram = new KLHistogram( ui->frmScopeHisto, "histogram", 128, -127, true, false );
    
    /// @TODO Add a config window!
    
    QString curBRate;
    
    curBRate = "110";
    m_baudConsts[ curBRate ] = B110;
    m_baudRates.append( curBRate );

    curBRate = "300";
    m_baudConsts[ curBRate ] = B300;
    m_baudRates.append( curBRate );

    curBRate = "600";
    m_baudConsts[ curBRate ] = B600;
    m_baudRates.append( curBRate );

    curBRate = "1200";
    m_baudConsts[ curBRate ] = B1200;
    m_baudRates.append( curBRate );

    curBRate = "2400";
    m_baudConsts[ curBRate ] = B2400;
    m_baudRates.append( curBRate );

    curBRate = "4800";
    m_baudConsts[ curBRate ] = B4800;
    m_baudRates.append( curBRate );

    curBRate = "9600";
    m_baudConsts[ curBRate ] = B9600;
    m_baudRates.append( curBRate );

    curBRate = "19200";
    m_baudConsts[ curBRate ] = B19200;
    m_baudRates.append( curBRate );

    curBRate = "38400";
    m_baudConsts[ curBRate ] = B38400;
    m_baudRates.append( curBRate );

    curBRate = "57600";
    m_baudConsts[ curBRate ] = B57600;
    m_baudRates.append( curBRate );

    curBRate = "115200";
    m_baudConsts[ curBRate ] = B115200;
    m_baudRates.append( curBRate );

    ui->cbBaud->insertStringList( m_baudRates );
    
    QStringList ports;
    ports.append( "/dev/ttyS0" );
    ports.append( "/dev/ttyS1" );
    ports.append( "/dev/ttyS2" );
    ports.append( "/dev/ttyS3" );
    ports.append( "/dev/ttyUSB0" );

    ui->cbDevice->insertStringList( ports );
    
    KIconLoader ico;
    ui->tbOpenClose->setIconSet( ico.loadIcon( "document-open", KIconLoader::Panel ) );
    
    ui->tbClear->setIconSet( ico.loadIcon( "edit-clear-list", KIconLoader::Panel ) );
    
    m_listener = 0L;
    m_displayIsHEX = false;
    ui->frmScopeHisto->hide();

}


void KLSerialTerminalWidget::slotSend()
{
    if ( terminalInNormalMode() )
    {
        const QString outData = ui->teSendData->text();
        const QString outData_lower = outData.lower();
        std::vector< unsigned char > data;

        unsigned int i = 0;

        while ( i < outData.length() )
        {
            bool isHex = false;
            unsigned char lenOfThisChar = 1;
            unsigned char hexVal = 0;

            if ( outData.at( i ) == QChar('#') && ( i+1 < outData.length() )
                 && isHexDigit( (unsigned char) outData.at( i+1 ).latin1() ) )
            {
                // This is in fact a hex value!
                isHex = true;
                hexVal = hexToUChar( (unsigned char) outData_lower.at( i+1 ).latin1() );
                lenOfThisChar = 2;
                if ( ( i+2 < outData.length() ) && isHexDigit( (unsigned char) outData.at( i+2 ).latin1() ) )
                {
                    // qDebug( "Val is %d", hexToUChar( (unsigned char) outData_lower.at( i+2 ).latin1() ) );
                    hexVal = (hexVal * 16) + hexToUChar( (unsigned char) outData_lower.at( i+2 ).latin1() );
                    lenOfThisChar = 3;
                }
            }
            if ( !isHex )
                data.push_back( (unsigned char) outData.at( i ).latin1() );
            else
                data.push_back( hexVal );
            i += lenOfThisChar;
        }
        m_serialPort->write( data );
    }
}

void KLSerialTerminalWidget::slotShowAsHEX(bool val)
{
    if (!val)
        return;
    if ( !m_displayIsHEX )
    {
        ui->teReceiveData->setText( asciiToHEX(m_receiveBuffer) );
        m_displayIsHEX = true;
    }
}


void KLSerialTerminalWidget::slotShowAsAscii(bool val)
{
    if (!val)
        return;
    if ( m_displayIsHEX )
    {
        ui->teReceiveData->setText( convertCharVectorToAscii( m_receiveBuffer ) );
        m_displayIsHEX = false;
    }
}


void KLSerialTerminalWidget::slotReceived( const KLCharVector& what_ )
{
//     teReceiveData->setCursorPosition( teReceiveData->paragraphs(),
//                                       teReceiveData->length() );
    if ( terminalInNormalMode() )
    {
        KLCharVector what = what_;
        if ( ( ui->cbShowAs->currentItem() == HEX_ENTRY ) || ( ui->cbShowAs->currentItem() == ASCII_ENTRY ) )
        {
            for ( unsigned int i=0; i < what.size(); i++ )
                m_receiveBuffer.push_back( what[ i ] );
            bool hasBeenCut = false;
            if ( m_receiveBuffer.size() > 1024 )
            {
                hasBeenCut = true;
                KLCharVector buffer;
                for ( unsigned int i=m_receiveBuffer.size()-1024; i < m_receiveBuffer.size(); i++ )
                    buffer.push_back( m_receiveBuffer[ i ] );
                m_receiveBuffer = buffer;
            }
            QString whatString = convertCharVectorToAscii( m_receiveBuffer );
            if ( ui->cbShowAs->currentItem() == HEX_ENTRY )
                whatString = asciiToHEX( what );
            if ( (!hasBeenCut) && ui->teReceiveData->text().endsWith("\n") )
            {
                ui->teReceiveData->append( whatString );
            }
            else
            {
                if ( ui->cbShowAs->currentItem() == HEX_ENTRY )
                    ui->teReceiveData->setText( asciiToHEX( m_receiveBuffer ) );
                else
                    ui->teReceiveData->setText( whatString );
            }
            ui->teReceiveData->scrollToBottom();
        }
        else
        {
            vector< int > data;
            for ( unsigned int i = 0; i < what_.size(); i++ )
            {
                int out = what[ i ];
                if (out < 0)
                    out+=256;
                data.push_back( out );
            }
            m_scope->addData( data );
            m_histogram->addData( data );
        }
    }
    else
    {
        emit( receivedICDData( what_ ) );
    }
}

void KLSerialTerminalWidget::slotOpenClose()
{
    if ( !m_serialPort->isOpen() )
    {
        m_serialPort->setRtscts( ui->rbRTSCTS->isChecked() );
        m_serialPort->setXonxoff( ui->rbXONXOFF->isChecked() );
        if ( !m_serialPort->open( ui->cbDevice->currentText(), m_baudConsts[ ui->cbBaud->currentText() ] ) )
        {
            m_parent->slotMessageBox( 1, i18n("Could not open port %1.").arg(ui->cbDevice->currentText()) );
        }
        else if ( m_listener == 0L )
        {
            m_listener = new KLSerialPortListener( this, m_serialPort, "serialPortListener" );
            m_listener->setReceiverActive(true);
        }
    }
    else
    {
        m_listener->setReceiverActive( false );
        m_serialPort->close();
        m_listener->wait();
        delete m_listener;
        m_listener = 0L;
    }
    KIconLoader ico;
    // Check again if it is open now:
    if ( m_serialPort->isOpen() )
    {
        ui->tbOpenClose->setIconSet( ico.loadIcon( "connect_established", KIconLoader::Panel ) );
        ui->cbDevice->setEnabled(false);
        ui->cbBaud->setEnabled(false);
        ui->pbSend->setEnabled(true);

        ui->rbRTSCTS->setEnabled( false );
        ui->rbXONXOFF->setEnabled( false );
        ui->rbNone->setEnabled( false );
    }
    else
    {
        ui->tbOpenClose->setIconSet( ico.loadIcon( "connect_no", KIconLoader::Panel ) );
        ui->cbDevice->setEnabled(true);
        ui->cbBaud->setEnabled(true);
        ui->pbSend->setEnabled(false);

        ui->rbRTSCTS->setEnabled( true );
        ui->rbXONXOFF->setEnabled( true );
        ui->rbNone->setEnabled( true );
    }
}


void KLSerialTerminalWidget::paintEvent( QPaintEvent* )
{
    if (m_listener)
        slotReceived( m_listener->fetchOutData() );
}

QString KLSerialTerminalWidget::asciiToHEX( const KLCharVector & inp )
{
    QString retVal = "";
    if (inp.size()==0)
        return "";
    for (unsigned int i=0; i<inp.size(); i++)
    {
        int out = inp[ i ];
        if (out < 0)
            out+=256;
        QString thisChar = QString("%1 ").arg( out, -1, 16 );
        if ( out < 16 )
            thisChar = "0" + thisChar;
        retVal += thisChar;
    }
    return retVal;
}

void KLSerialTerminalWidget::slotClearReceiver()
{
    m_receiveBuffer.clear();
    ui->teReceiveData->setText( "" );
    m_scope->clearData();
    m_histogram->clearData();
}

void KLSerialTerminalWidget::setProgrammerBusy( bool isBusy )
{
    m_programmerBusy = isBusy;
    if ( isBusy && ui->cbDisconnectWhileProgrammerActive->isChecked() )
    {
        m_wasOpenedBeforeProgramming = m_serialPort->isOpen();
        if ( m_wasOpenedBeforeProgramming )
            slotOpenClose();
    }
    else
    {
        if ( m_wasOpenedBeforeProgramming )
            slotOpenClose();
        m_wasOpenedBeforeProgramming = false;
    }
}

KLSerialTerminalWidget::~ KLSerialTerminalWidget( )
{
    m_serialPort->close();
}


void KLSerialTerminalWidget::slotSendICDData( const std::vector< unsigned char > & data )
{
    m_serialPort->write( data );
}

void KLSerialTerminalWidget::slotOpenICD( unsigned int baudRate, bool rtscts, bool xonxoff )
{
    if ( !terminalInICDMode() )
    {
        qWarning( "Tried to connect in ICD mode, but the terminal is in normal mode! %s:%d",
                  __FILE__, __LINE__ );
        return;
    }
    // If the port is open, close it first:
    if ( m_serialPort->isOpen() )
    {
        m_listener->setReceiverActive( false );
        m_serialPort->close();
        m_listener->wait();
        delete m_listener;
        m_listener = 0L;
    }
    m_serialPort->setRtscts( rtscts );
    m_serialPort->setXonxoff( xonxoff );
    if ( !m_serialPort->open( ui->cbDevice->currentText(), m_baudConsts[ QString::number(baudRate) ] ) )
    {
        m_parent->slotMessageBox( 1, i18n("Could not open port %1 for ICD mode.").arg(ui->cbDevice->currentText()) );
    }
    else if ( m_listener == 0L )
    {
        m_listener = new KLSerialPortListener( this, m_serialPort, "serialPortListener" );
        m_listener->setReceiverActive(true);
    }
}


void KLSerialTerminalWidget::slotCloseICD( )
{
    if ( m_serialPort->isOpen() )
    {
        m_listener->setReceiverActive( false );
        m_serialPort->close();
        m_listener->wait();
        delete m_listener;
        m_listener = 0L;
    }
}

void KLSerialTerminalWidget::customEvent( QCustomEvent * )
{
    if (m_listener)
        slotReceived( m_listener->fetchOutData() );
}


void KLSerialTerminalWidget::saveProperties(KSharedConfig::Ptr conf )
{
    KConfigGroup group( conf, "KontrollerLab" );
    group.writeEntry( CONFIG_RTSCTS, ui->rbRTSCTS->isChecked() ? TRUE_STRING : FALSE_STRING );
    group.writeEntry( CONFIG_XONXOFF, ui->rbXONXOFF->isChecked() ? TRUE_STRING : FALSE_STRING );
    group.writeEntry( CONFIG_SERIAL_DEVICE, ui->cbDevice->currentText() );
    group.writeEntry( CONFIG_SERIAL_BAUD, ui->cbBaud->currentText() );
    group.writeEntry( CONFIG_DISCONNECT_WHEN_ACTIVE,
                      ui->cbDisconnectWhileProgrammerActive->isChecked() ? TRUE_STRING : FALSE_STRING );
}


void KLSerialTerminalWidget::readProperties( KSharedConfig::Ptr config )
{
    KConfigGroup conf( config, "KontrollerLab" );
    QMap< QString, QString > map = conf.entryMap();
    if ( map[ CONFIG_RTSCTS ] == TRUE_STRING )
        ui->rbRTSCTS->setChecked( true );
    else if ( map[ CONFIG_XONXOFF ] == TRUE_STRING )
        ui->rbXONXOFF->setChecked( true );
    else
        ui->rbNone->setChecked( true );
    ui->cbDisconnectWhileProgrammerActive->setChecked(
            map[ CONFIG_DISCONNECT_WHEN_ACTIVE ] == TRUE_STRING );

    bool exists = false;
    QString serDev = map[ CONFIG_SERIAL_DEVICE ];
    if ( (!serDev.isEmpty()) && (!serDev.isNull()) )
    {
        for ( int i=0; i < ui->cbDevice->count(); i++ )
        {
            if ( ui->cbDevice->text( i ) == serDev )
            {
                exists = true;
                break;
            }
        }
        if ( !exists )
            ui->cbDevice->insertItem( serDev );
        ui->cbDevice->setCurrentText( serDev );
    }
    QString serBaud = map[ CONFIG_SERIAL_BAUD ];
    if ( (!serBaud.isEmpty()) && (!serBaud.isNull()) )
    {
        ui->cbBaud->setCurrentText( serBaud );
    }
}

QStringList KLSerialTerminalWidget::ports( ) const
{
    QStringList retVal;
    
    for ( int i=0; i < ui->cbDevice->count(); i++ )
        retVal.append( ui->cbDevice->text( i ) );
    return retVal;
}

void KLSerialTerminalWidget::slotShowAsChanged(int current)
{
    switch ( current )
    {
        case ASCII_ENTRY:
            ui->frmScopeHisto->hide();
            ui->teReceiveData->show();
            slotShowAsAscii( true );
            break;
        case HEX_ENTRY:
            ui->frmScopeHisto->hide();
            ui->teReceiveData->show();
            slotShowAsHEX( true );
            break;
        case SCOPE_ENTRY:
            ui->frmScopeHisto->show();
            m_scope->show();
            m_histogram->hide();
            ui->teReceiveData->hide();
            break;
        case HISTOGRAM_ENTRY:
            ui->frmScopeHisto->show();
            m_scope->hide();
            m_histogram->show();
            ui->teReceiveData->hide();
            break;
    }
}


QString KLSerialTerminalWidget::convertCharVectorToAscii( const KLCharVector & inp )
{
    QString whatString;
    for ( unsigned int i=0; i < inp.size(); i++ )
        whatString += QChar( inp[ i ] );
    return whatString;
}


bool KLSerialTerminalWidget::isHexDigit( unsigned char ch )
{
    if ( ( 'a' <= ch ) && ( ch <= 'f' ) )
        return true;
    if ( ( 'A' <= ch ) && ( ch <= 'F' ) )
        return true;
    if ( ( '0' <= ch ) && ( ch <= '9' ) )
        return true;
    return false;
}

unsigned char KLSerialTerminalWidget::hexToUChar( unsigned char ch )
{
    if ( ( 'a' <= ch ) && ( ch <= 'f' ) )
        return ch - 'a' + 10;
    if ( ( 'A' <= ch ) && ( ch <= 'F' ) )
        return ch - 'A' + 10;
    if ( ( '0' <= ch ) && ( ch <= '9' ) )
        return ch - '0';
    return 0;
}


