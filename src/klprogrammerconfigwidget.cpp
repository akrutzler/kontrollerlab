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
#include "klprogrammerconfigwidget.h"
#include "klprogrammerinterface.h"
#include "klprogrammeruisp.h"
#include "klprogrammeravrdude.h"
#include "kontrollerlab.h"

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kapplication.h>
#include <qregexp.h>
#include <ksharedconfig.h>


KLProgrammerConfigWidget::KLProgrammerConfigWidget(KontrollerLab *parent, const char *name)
    :QDialog(parent), ui(new Ui::KLProgrammerConfigWidgetBase)
{
    ui->setupUi(this);
    setObjectName(name);

    connect(ui->cbAVRDUDESpecifyBitClock,SIGNAL(toggled(bool)),ui->cbBitClockPeriodUS,SLOT(setEnabled(bool)));
    connect(ui->cbAVRDUDEOverrideBaudRate, SIGNAL(toggled(bool)), ui->cbBAUDRate, SLOT(setEnabled(bool)));
    connect(ui->cbAVRDUDESpecifyProgrammerType, SIGNAL(toggled(bool)), ui->cbAVRDUDEProgrammerType, SLOT(setEnabled(bool)));
    connect(ui->cbAVRDUDESpecifyConnectionPort, SIGNAL(toggled(bool)), ui->cbAVRDUDEConnectionPort, SLOT(setEnabled(bool)));
    connect(ui->cbAVRDUDESpecifyExternalConfigFile, SIGNAL(toggled(bool)), ui->kurlExternalConfigFile, SLOT(setEnabled(bool)));
    connect(ui->pbOK, SIGNAL(clicked()), this, SLOT(slotOK()));
    connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
    connect(ui->pbSetDefault, SIGNAL(clicked()), this, SLOT(slotSetDefault()));
    connect(ui->pbSetEraseCount, SIGNAL(clicked()), this, SLOT(slotSetEraseCounter()));
    connect(ui->pbUISPReadValues, SIGNAL(clicked()), this, SLOT(slotReadSTK500()));
    connect(ui->pbUISPWriteValues, SIGNAL(clicked()), this, SLOT(slotWriteSTK500()));

    m_parent = parent;
    setModal( true );

    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );
    KLProgrammerAVRDUDE* avrdude = (KLProgrammerAVRDUDE*) m_parent->getProgrammer( AVRDUDE );

    QStringList list = uisp->programmerTypesKeyList();
    for ( unsigned int i = 0; i < list.count(); i++ )
        ui->cbUISPProgrammer->addItem( uisp->getProgrammerTypeGUIStringFor( list[i] ) );

    list = avrdude->programmerTypesKeyList();
    for ( unsigned int i = 0; i < list.count(); i++ )
        ui->cbAVRDUDEProgrammerType->addItem( avrdude->getProgrammerTypeGUIStringFor( list[i] ) );

    list = uisp->parallelPortsKeyList();
    for ( unsigned int i = 0; i < list.count(); i++ )
    {
        ui->cbUISPParallelPort->addItem( uisp->getParallelPortGUIStringFor( list[i] ) );
    }
    list = avrdude->parallelPortsKeyList();
    for ( unsigned int i = 0; i < list.count(); i++ )
    {
        ui->cbAVRDUDEConnectionPort->addItem( avrdude->getParallelPortGUIStringFor( list[i] ) );
    }

    list = uisp->serialPortsKeyList();
    for ( unsigned int i = 0; i < list.count(); i++ )
    {
        ui->cbUISPSerialPort->addItem( uisp->getSerialPortGUIStringFor( list[i] ) );
    }
    list = avrdude->serialPortsKeyList();
    for ( unsigned int i = 0; i < list.count(); i++ )
    {
        ui->cbAVRDUDEConnectionPort->addItem( avrdude->getSerialPortGUIStringFor( list[i] ) );
    }

    m_configuration = m_parent->programmerConfig();
    updateGUIFromConfig();
}

void KLProgrammerConfigWidget::slotCancel()
{
    hide();
}


void KLProgrammerConfigWidget::slotOK()
{

    updateConfigFromGUI();
    m_parent->setProgrammerConfig( m_configuration );

    hide();
}

void KLProgrammerConfigWidget::slotSerialPortChanged( const QString& val )
{
    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );

    QString buffer = uisp->getSerialPortFor( val );
    if ( (buffer == USER_KEY) || (buffer.isEmpty() || buffer.isNull() ) )
        ui->cbUISPSerialPort->setEditable( true );
    else
        ui->cbUISPSerialPort->setEditable( false );
}


void KLProgrammerConfigWidget::slotParallelPortChanged( const QString& val )
{
    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );

    QString buffer = uisp->getParallelPortFor( val );
    if ( (buffer == USER_KEY) || (buffer.isEmpty() || buffer.isNull() ) )
        ui->cbUISPParallelPort->setEditable( true );
    else
        ui->cbUISPParallelPort->setEditable( false );
}

void KLProgrammerConfigWidget::updateGUIFromConfig( )
{
    if ( config( PROGRAMMER_NAME, UISP ) == UISP )
    {
        ui->rbUISP->setChecked( true );
        ui->leUISPCommand->setText( config(PROGRAMMER_COMMAND, "uisp") );
        ui->leUISPCommand->setFocus( );
    }
    else
    {
        ui->rbAVRDUDE->setChecked( true );
        ui->leAVRDUDECommand->setText( config(PROGRAMMER_COMMAND, "avrdude") );
        ui->leAVRDUDECommand->setFocus( );
    }

    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );
    KLProgrammerAVRDUDE* avrdude = (KLProgrammerAVRDUDE*) m_parent->getProgrammer( AVRDUDE );
    bool ok;
    
    // Get the unique shell args for the user visible strings:
    setComboBoxText(ui->cbUISPProgrammer, uisp->getProgrammerTypeGUIStringFor( config( UISP_PROGRAMMER_TYPE ) ));
    setComboBoxText(ui->cbUISPParallelPort, uisp->getParallelPortGUIStringFor( config( UISP_PARALLEL_PORT ) ) );
    setComboBoxText(ui->cbUISPSerialPort, uisp->getSerialPortGUIStringFor( config( UISP_SERIAL_PORT ) ) );
    
    ui->cbSpecifyPart->setChecked( config( UISP_SPECIFY_PART ) == TRUE_STRING );
    ui->cbNoDataPolling->setChecked( config( UISP_PARALLEL_NO_DATA_POLLING ) == TRUE_STRING );
    ui->cbDisableRetries->setChecked( config( UISP_PARALLEL_DISABLE_RETRIES ) == TRUE_STRING );
    ui->cbAT89SProgramming->setChecked( config( UISP_PARALLEL_AT89S ) == TRUE_STRING );
    ui->sbUISPVoltage->setValue( config( UISP_PARALLEL_VOLTAGE ).toDouble( &ok ) );
    ui->sbUISPHighLowTime->setValue( config( UISP_PARALLEL_SCK_HIGH_LOW_TIME ).toDouble( &ok ) );
    ui->sbUISPFlashMaxWriteDelay->setValue( config( UISP_PARALLEL_FLASH_MAX_WRITE_DELAY ).toDouble( &ok ) );
    ui->sbUISPEEPROMMaxWriteDelay->setValue( config( UISP_PARALLEL_EEPROM_MAX_WRITE_DELAY ).toDouble( &ok ) );
    ui->sbUISPResetHighTime->setValue( config( UISP_PARALLEL_RESET_HIGH_TIME ).toDouble( &ok ) );
    setComboBoxText(ui->cbUISPSerialSpeed, config( UISP_SERIAL_SPEED, "9600" ) );
    ui->cbUISPUseHighVoltage->setChecked( config( UISP_STK500_USE_HIGH_VOLTAGE ) == TRUE_STRING );
    ui->sbUISPARef->setValue( config( UISP_STK500_AREF_VOLTAGE ).toDouble( &ok ) );
    ui->sbUISPVtarget->setValue( config( UISP_STK500_VTARGET_VOLTAGE ).toDouble( &ok ) );
    ui->sbOscillatorFrequency->setValue( config( UISP_STK500_OSCILLATOR_FREQUENCY ).toDouble( &ok ) );
    
    // AVRDUDE STUFF:
    
    ui->cbAVRDUDEOverrideBaudRate->setChecked( config( AVRDUDE_OVERRIDE_BAUD_RATE, "_unset_" ) != "_unset_" );
    ui->cbAVRDUDESpecifyBitClock->setChecked( config( AVRDUDE_SPECIFY_BIT_CLOCK, "_unset_" ) != "_unset_" );
    ui->cbAVRDUDESpecifyProgrammerType->setChecked( config( AVRDUDE_PROGRAMMER_TYPE, "_unset_" ) != "_unset_" );
    ui->cbAVRDUDESpecifyConnectionPort->setChecked( config( AVRDUDE_CONNECTION_PORT, "_unset_" ) != "_unset_" );
    ui->cbAVRDUDESpecifyExternalConfigFile->setChecked( config( AVRDUDE_EXTERNAL_CONFIG_FILE, "_unset_" ) != "_unset_" );
    ui->cbAVRDUDEDisableAutoErase->setChecked( config( AVRDUDE_DISABLE_AUTO_ERASE, "_unset_" ) == TRUE_STRING );
    ui->cbAVRDUDEDoNotWriteAnything->setChecked( config( AVRDUDE_TEST_MODE, "_unset_" ) == TRUE_STRING );
    ui->cbAVRDUDEOverrideInvalidSignatureCheck->setChecked( config( AVRDUDE_OVERRIDE_INVALID_SIGNATURE, "_unset_" ) == TRUE_STRING );
    ui->cbAVRDUDECountErases->setChecked( config( AVRDUDE_COUNT_ERASE, "_unset_" ) == TRUE_STRING );
    
    if ( ui->cbAVRDUDEOverrideBaudRate->isChecked() )
        setComboBoxText(ui->cbBAUDRate, config( AVRDUDE_OVERRIDE_BAUD_RATE, "9600" ) );
    if ( ui->cbAVRDUDESpecifyBitClock->isChecked() )
        ui->cbBitClockPeriodUS->setValue( config( AVRDUDE_SPECIFY_BIT_CLOCK, "5.0" ).toDouble( &ok ) );
    if ( ui->cbAVRDUDESpecifyProgrammerType->isChecked() )
        setComboBoxText(ui->cbAVRDUDEProgrammerType,
                        avrdude->getProgrammerTypeGUIStringFor( config( AVRDUDE_PROGRAMMER_TYPE, "" ) ) );
    if ( ui->cbAVRDUDESpecifyConnectionPort->isChecked() )
        setComboBoxText(ui->cbAVRDUDEConnectionPort,
                        avrdude->getPortGUIStringFor( config( AVRDUDE_CONNECTION_PORT, "" ) ) );
    if ( ui->cbAVRDUDESpecifyExternalConfigFile->isChecked() )
        ui->kurlExternalConfigFile->setUrl( config( AVRDUDE_EXTERNAL_CONFIG_FILE, "" ) );
}


QString KLProgrammerConfigWidget::config( const QString & key, const QString & defVal )
{
    if ( m_configuration.find( key ) != m_configuration.end() )
        return m_configuration[ key ];
    else
    {
        KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );

        QString val = group.readEntry( key, "" );
        if ( (!val.isEmpty()) && (!val.isNull()) )
            return val;
        else
            return defVal;
    }
}


void KLProgrammerConfigWidget::slotSetDefault( )
{
    updateConfigFromGUI();
    
    QMap<QString, QString>::Iterator it;
    {
        KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );

        group.deleteEntry(AVRDUDE_OVERRIDE_BAUD_RATE);
        group.deleteEntry(AVRDUDE_SPECIFY_BIT_CLOCK);
        group.deleteEntry(AVRDUDE_PROGRAMMER_TYPE);
        group.deleteEntry(AVRDUDE_CONNECTION_PORT);
        group.deleteEntry(AVRDUDE_EXTERNAL_CONFIG_FILE);

        for ( it = m_configuration.begin(); it != m_configuration.end(); ++it )
        {
            group.writeEntry( it.key(), it.value() );
        }
        group.sync();
    }
}

void KLProgrammerConfigWidget::updateConfigFromGUI( )
{
    m_configuration.clear();

    if ( ui->rbUISP->isChecked() )
    {
        m_configuration[PROGRAMMER_NAME] = UISP;
        m_configuration[PROGRAMMER_COMMAND] = ui->leUISPCommand->text();
    }
    if ( ui->rbAVRDUDE->isChecked() )
    {
        m_configuration[PROGRAMMER_NAME] = AVRDUDE;
        m_configuration[PROGRAMMER_COMMAND] = ui->leAVRDUDECommand->text();
    }

    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );
    KLProgrammerAVRDUDE* avrdude = (KLProgrammerAVRDUDE*) m_parent->getProgrammer( AVRDUDE );


    // Get the unique shell args for the user visible strings:
    m_configuration[UISP_PROGRAMMER_TYPE] = uisp->getProgrammerTypeFor( ui->cbUISPProgrammer->currentText() );
    m_configuration[UISP_PARALLEL_PORT] = uisp->getParallelPortFor( ui->cbUISPParallelPort->currentText() );
    m_configuration[UISP_SERIAL_PORT] = uisp->getSerialPortFor( ui->cbUISPSerialPort->currentText() );

    m_configuration[UISP_SPECIFY_PART] = ui->cbSpecifyPart->isChecked() ? TRUE_STRING : FALSE_STRING;
    m_configuration[UISP_PARALLEL_NO_DATA_POLLING] = ui->cbNoDataPolling->isChecked() ? TRUE_STRING : FALSE_STRING;
    m_configuration[UISP_PARALLEL_DISABLE_RETRIES] = ui->cbDisableRetries->isChecked() ? TRUE_STRING : FALSE_STRING;
    m_configuration[UISP_PARALLEL_AT89S] = ui->cbAT89SProgramming->isChecked() ? TRUE_STRING : FALSE_STRING;
    m_configuration[UISP_PARALLEL_VOLTAGE] = QString( "%1" ).arg( ui->sbUISPVoltage->value() );
    m_configuration[UISP_PARALLEL_SCK_HIGH_LOW_TIME] = QString( "%1" ).arg( ui->sbUISPHighLowTime->value() );
    m_configuration[UISP_PARALLEL_FLASH_MAX_WRITE_DELAY] = QString( "%1" ).arg( ui->sbUISPFlashMaxWriteDelay->value() );
    m_configuration[UISP_PARALLEL_EEPROM_MAX_WRITE_DELAY] = QString( "%1" ).arg( ui->sbUISPEEPROMMaxWriteDelay->value() );
    m_configuration[UISP_PARALLEL_RESET_HIGH_TIME] = QString( "%1" ).arg( ui->sbUISPResetHighTime->value() );
    m_configuration[UISP_SERIAL_SPEED] = ui->cbUISPSerialSpeed->currentText();
    m_configuration[UISP_STK500_USE_HIGH_VOLTAGE] = ui->cbUISPUseHighVoltage->isChecked() ? TRUE_STRING : FALSE_STRING;
    m_configuration[UISP_STK500_AREF_VOLTAGE] = QString( "%1" ).arg( ui->sbUISPARef->value() );
    m_configuration[UISP_STK500_VTARGET_VOLTAGE] = QString( "%1" ).arg( ui->sbUISPVtarget->value() );
    m_configuration[UISP_STK500_OSCILLATOR_FREQUENCY] = QString( "%1" ).arg( ui->sbOscillatorFrequency->value() );

    // AVRDUDE STUFF:

    m_configuration[ AVRDUDE_DISABLE_AUTO_ERASE ] = ui->cbAVRDUDEDisableAutoErase->isChecked()? TRUE_STRING:FALSE_STRING;
    m_configuration[ AVRDUDE_TEST_MODE ] = ui->cbAVRDUDEDoNotWriteAnything->isChecked()? TRUE_STRING:FALSE_STRING;
    m_configuration[ AVRDUDE_OVERRIDE_INVALID_SIGNATURE ] = ui->cbAVRDUDEOverrideInvalidSignatureCheck->isChecked()? TRUE_STRING:FALSE_STRING;
    m_configuration[ AVRDUDE_COUNT_ERASE ] = ui->cbAVRDUDECountErases->isChecked()? TRUE_STRING:FALSE_STRING;

    if ( ui->cbAVRDUDEOverrideBaudRate->isChecked() )
        m_configuration[ AVRDUDE_OVERRIDE_BAUD_RATE ] = ui->cbBAUDRate->currentText();
    if ( ui->cbAVRDUDESpecifyBitClock->isChecked() )
        m_configuration[ AVRDUDE_SPECIFY_BIT_CLOCK ] = QString("%1").arg(ui->cbBitClockPeriodUS->value());
    if ( ui->cbAVRDUDESpecifyProgrammerType->isChecked() )
        m_configuration[ AVRDUDE_PROGRAMMER_TYPE ] =
                avrdude->getProgrammerTypeFor( ui->cbAVRDUDEProgrammerType->currentText() );
    if ( ui->cbAVRDUDESpecifyConnectionPort->isChecked() )
        m_configuration[ AVRDUDE_CONNECTION_PORT ] =
                avrdude->getPortFor( ui->cbAVRDUDEConnectionPort->currentText() );
    if ( ui->cbAVRDUDESpecifyExternalConfigFile->isChecked() )
        m_configuration[ AVRDUDE_EXTERNAL_CONFIG_FILE ] = ui->kurlExternalConfigFile->text();

}


void KLProgrammerConfigWidget::showEvent( QShowEvent * )
{
    m_configuration = m_parent->programmerConfig();
    updateGUIFromConfig();
}

void KLProgrammerConfigWidget::slotWriteSTK500()
{
    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );
    QMap< QString, QString > oldConfig = uisp->configuration();

    bool uispSelected = ui->rbUISP->isChecked();
    ui->rbUISP->setChecked( true );
    updateConfigFromGUI();
    uisp->configuration() = m_configuration;
    uisp->writeSTK500( ui->sbUISPARef->value(),
                       ui->sbUISPVtarget->value(),
                       ui->sbOscillatorFrequency->value() );
    ui->rbUISP->setChecked( uispSelected );
    ui->rbAVRDUDE->setChecked( !uispSelected );
    uisp->configuration() = oldConfig;
}


void KLProgrammerConfigWidget::slotReadSTK500()
{
    KLProgrammerUISP* uisp = (KLProgrammerUISP*) m_parent->getProgrammer( UISP );
    QMap< QString, QString > oldConfig = uisp->configuration();

    bool uispSelected = ui->rbUISP->isChecked();
    ui->rbUISP->setChecked( true );
    updateConfigFromGUI();
    uisp->configuration() = m_configuration;
    uisp->readSTK500( );
    ui->rbUISP->setChecked( uispSelected );
    ui->rbAVRDUDE->setChecked( !uispSelected );
    uisp->configuration() = oldConfig;
}


void KLProgrammerConfigWidget::slotSetEraseCounter()
{
    KLProgrammerAVRDUDE* avrdude = (KLProgrammerAVRDUDE*) m_parent->getProgrammer( AVRDUDE );

    QMap< QString, QString > oldConfig = avrdude->configuration();

    bool avrdudeSelected = ui->rbAVRDUDE->isChecked();
    ui->rbAVRDUDE->setChecked( true );
    updateConfigFromGUI();
    avrdude->configuration() = m_configuration;
    avrdude->setEraseCounter( ui->sbEraseCount->value() );
    ui->rbUISP->setChecked( !avrdudeSelected );
    ui->rbAVRDUDE->setChecked( avrdudeSelected );
    avrdude->configuration() = oldConfig;
}


void KLProgrammerConfigWidget::backannotateSTK500( const QString & stdout )
{
    QStringList list = stdout.split("\n");

    bool ok;

    QRegExp reARef( "AREF\\s*=\\s*([0-9]*\\.[0-9]*)\\s*V" );
    QRegExp reVSup( "VTARGET\\s*=\\s*([0-9]*\\.[0-9]*)\\s*V" );
    QRegExp reClk( "FREQUENCY\\s*=\\s*([0-9]*\\.[0-9]*)\\s*HZ" );

    for ( QStringList::iterator it = list.begin();
          it != list.end(); ++it )
    {
        QString line = (*it).toUpper();
        if ( reARef.indexIn( line ) != -1 )
            ui->sbUISPARef->setValue( reARef.cap( 1 ).toDouble( &ok ) );
        if ( reVSup.indexIn( line ) != -1 )
            ui->sbUISPVtarget->setValue( reVSup.cap( 1 ).toDouble( &ok ) );
        if ( reClk.indexIn( line ) != -1 )
            ui->sbOscillatorFrequency->setValue( reClk.cap( 1 ).toDouble( &ok ) );
    }
}


