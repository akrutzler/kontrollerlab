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

#include "klprojectconfigwidget.h"
#include "klprojectmanagerwidget.h"
#include <qlayout.h>
#include "klproject.h"
#include <qcombobox.h>
#include <qlabel.h>
#include <klocale.h>
#include <knuminput.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlistview.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>


KLProjectConfigWidget::KLProjectConfigWidget(KLProject* project, QWidget *parent, const char *name)
    :QDialog(parent, name), m_validator(QRegExp("0x[0-9a-fA-F]{1,4}"), this, "reValidator"),
      ui(new Ui_KLProjectConfigWidgetBase)
{
    ui->setupUi(this);
    m_project = project;
    m_projectManagerInConfigWidget =
            new KLProjectManagerWidget( project, ui->tab_common, "managerInConfig" );
    m_projectManagerInConfigWidget->setConfigButtonVisible( false );
    ui->layProjectManager->addWidget( m_projectManagerInConfigWidget );
    ui->cbCPU->clear();
    // Add all the CPUs:
    ui->cbCPU->insertStringList( project->cpus() );
    // Update the project files list:
    project->update();
    // Add linker flags columns
    ui->lvLinkerFlags->addColumn(i18n("Purpose"));
    ui->lvLinkerFlags->addColumn(i18n("Flags"));
    // Fill the linker flags list:
    m_possibleLinkerFlags["-Wl,-lm"] = i18n("Link with math library");
    m_possibleLinkerFlags["-Wl,-lprintf"] = i18n("Link with printf library");
    m_possibleLinkerFlags["-Wl,-lprintf_min"] = i18n("Link with printf_min library");
    m_possibleLinkerFlags["-Wl,-u,vfprintf -Wl,-lprintf_flt"] = i18n("Link with floating point printf library");
    QStringList keys = m_possibleLinkerFlags.keys();
    keys.sort();

    for ( QStringList::iterator it = keys.begin(); it != keys.end(); ++it )
    {
        Q3CheckListItem* cur = new Q3CheckListItem( ui->lvLinkerFlags, m_possibleLinkerFlags[*it], Q3CheckListItem::CheckBox );
        cur->setText( 1, *it );
        m_linkerFlagsCBs.append( cur );
    }
    updateGUIFromSettings();
    updateCPUInfo( ui->cbCPU->currentText() );

    connect(ui->cbCPU,SIGNAL(currentIndexChanged(QString)),this,SLOT(slotCPUChanged(QString)));
    connect(ui->pbOK,SIGNAL(clicked()),this,SLOT(slotOK()));
    connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(slotCancel()));
    connect(ui->pbSetDefault,SIGNAL(clicked()),this,SLOT(slotSetDefault()));

    //Setup spinboxes with hex-in and output
    ui->kisbStartOfText->setValidator(&m_validator);
    ui->kisbStartOfText->setBase(16);

    ui->kisbStartOfData->setValidator(&m_validator);
    ui->kisbStartOfData->setBase(16);

    ui->kisbStartOfBSS->setValidator(&m_validator);
    ui->kisbStartOfBSS->setBase(16);

    ui->kisbStartOfHeap->setValidator(&m_validator);
    ui->kisbStartOfHeap->setBase(16);

    ui->kisbEndOfHeap->setValidator(&m_validator);
    ui->kisbEndOfHeap->setBase(16);
}

void KLProjectConfigWidget::slotCancel()
{
    hide();
}


void KLProjectConfigWidget::slotSetDefault()
{
    QMap<QString, QString>::Iterator it;

    KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );

    updateSettingsFromGUI();
    // m_settings = settings();

    group.deleteEntry( PRJ_CPU );
    group.deleteEntry( PRJ_CLOCKSPEED );
    group.deleteEntry( PRJ_EXTERNAL_RAM );
    group.deleteEntry( PRJ_HEX_FILE );
    group.deleteEntry( PRJ_MAP_FILE );
    group.deleteEntry( PRJ_COMPILER_COMMAND );
    group.deleteEntry( PRJ_COMPILER_CALL_PROLOGUES );
    group.deleteEntry( PRJ_COMPILER_STRICT_PROTOTYPES );
    group.deleteEntry( PRJ_COMPILER_WALL );
    group.deleteEntry( PRJ_COMPILER_GDEBUG );
    group.deleteEntry( PRJ_COMPILER_OPT_LEVEL );
    group.deleteEntry( PRJ_LINKER_COMMAND );
    group.deleteEntry( PRJ_OBJCOPY_COMMAND );
    group.deleteEntry( PRJ_LINKER_START_TEXT );
    group.deleteEntry( PRJ_LINKER_START_DATA );
    group.deleteEntry( PRJ_LINKER_START_BSS );
    group.deleteEntry( PRJ_LINKER_START_HEAP );
    group.deleteEntry( PRJ_LINKER_END_HEAP );
    group.deleteEntry( PRJ_LINKER_FLAGS );
    group.deleteEntry( PRJ_ASSEMBLER_COMMAND );
    group.deleteEntry( PRJ_COMPILER_F_CPU );
    for ( it = m_settings.begin(); it != m_settings.end(); ++it )
    {
        // qDebug("%s = %s", it.key().ascii(), it.data().ascii() );
        group.writeEntry( it.key(), it.data() );
    }
    group.sync();
}


void KLProjectConfigWidget::slotOK()
{
    updateSettingsFromGUI();
    m_project->setSettings( m_settings );
    hide();
}


void KLProjectConfigWidget::slotCPUChanged( const QString& name )
{
    updateCPUInfo( name );
}


void KLProjectConfigWidget::updateCPUInfo( const QString& name )
{
    KLCPUFeatures feat = m_project->cpuFeaturesFor( name );
    ui->lblInternalRAMBytes->setText( i18n("%1 Bytes").arg(feat.internalRAMSize()) );
    ui->lblClockMHz->setText( i18n("%1 MHz").arg(feat.maxClockSpeedHz()/1e6) );
    ui->lblEEPROMBytes->setText( i18n("%1 Bytes").arg(feat.EEPROMSize()) );
    ui->lblFLASHEEPROMBytes->setText( i18n("%1 Bytes").arg(feat.flashEEPROMSize()) );
}


void KLProjectConfigWidget::updateSettingsFromGUI( )
{
    QString buffer;
    QMap< QString, QString > retVal;
    retVal[ PRJ_CPU ] = ui->cbCPU->currentText();
    retVal[ PRJ_CLOCKSPEED ] = buffer.setNum( ui->kdsClock->value() );
    if ( ui->cbExternalRAM->isChecked() )
        retVal[ PRJ_EXTERNAL_RAM ] = buffer.setNum( ui->sbSize->value() );
    retVal[ PRJ_HEX_FILE ] = ui->leHEXFile->text();
    retVal[ PRJ_MAP_FILE ] = ui->leMAPFile->text();
    retVal[ PRJ_COMPILER_COMMAND ] = ui->leCompilerCommand->text();
    retVal[ PRJ_COMPILER_CALL_PROLOGUES ] = ui->cbCallPrologues->isChecked() ? TRUE_STRING : FALSE_STRING;
    retVal[ PRJ_COMPILER_STRICT_PROTOTYPES ] = ui->cbStrictPrototypes->isChecked() ? TRUE_STRING : FALSE_STRING;
    retVal[ PRJ_COMPILER_WALL ] = ui->cbAll->isChecked() ? TRUE_STRING : FALSE_STRING;
    retVal[ PRJ_COMPILER_GDEBUG ] = ui->cbGDebug->isChecked() ? TRUE_STRING : FALSE_STRING;
    retVal[ PRJ_COMPILER_F_CPU ] = ui->cbFCPU->isChecked() ? TRUE_STRING : FALSE_STRING;
    retVal[ PRJ_COMPILER_OPT_LEVEL ] = ui->cbOptimizationLevel->currentText().left(1).lower();
    retVal[ PRJ_LINKER_COMMAND ] = ui->leLinkerCommand->text();
    retVal[ PRJ_OBJCOPY_COMMAND ] = ui->leObjectCopyCommand->text();
    if ( ui->cbStartOfText->isChecked() )
        retVal[ PRJ_LINKER_START_TEXT ] = QString("%1").arg(ui->kisbStartOfText->value(), 0, 16);
    if ( ui->cbStartOfData->isChecked() )
        retVal[ PRJ_LINKER_START_DATA ] = QString("%1").arg(ui->kisbStartOfData->value(), 0, 16);
    if ( ui->cbStartOfBSS->isChecked() )
        retVal[ PRJ_LINKER_START_BSS ] = QString("%1").arg(ui->kisbStartOfBSS->value(), 0, 16);
    if ( ui->cbStartOfHeap->isChecked() )
        retVal[ PRJ_LINKER_START_HEAP ] = QString("%1").arg(ui->kisbStartOfHeap->value(), 0, 16);
    if ( ui->cbEndOfHeap->isChecked() )
        retVal[ PRJ_LINKER_END_HEAP ] = QString("%1").arg(ui->kisbEndOfHeap->value(), 0, 16);
    retVal[ PRJ_ASSEMBLER_COMMAND ] = ui->leAssemblerCommand->text();
    
    QString allFlags;
    foreach ( Q3CheckListItem* it, m_linkerFlagsCBs )
    {
        if ( it->state() == Q3CheckListItem::On )
        {
            allFlags += it->text( 1 ) + "#";
        }
    }
    if ( allFlags.length() > 0 )
        allFlags = allFlags.left( allFlags.length()-1 );
    // qDebug("allFlags is %s", allFlags.ascii());
    retVal[ PRJ_LINKER_FLAGS ] = allFlags;

    retVal[ PRJ_BUILD_SYSTEM ] = PRJ_BUILT_IN_BUILD_SYSTEM;
    if ( ui->rbExecuteMake->isChecked() )
        retVal[ PRJ_BUILD_SYSTEM ] = PRJ_EXECUTE_MAKE;

    retVal[ PRJ_MAKE_COMMAND ] = ui->leMake->text();
    retVal[ PRJ_MAKE_DEFAULT_TARGET ] = ui->leDefaultTarget->text();
    retVal[ PRJ_MAKE_CLEAN_TARGET ] = ui->leCleanTarget->text();

    m_settings = retVal;
}


void KLProjectConfigWidget::updateGUIFromSettings()
{
    bool ok;

    for (int i=0; i<ui->cbCPU->count(); i++)
        if ( ui->cbCPU->text( i ) == conf( PRJ_CPU, "AT90S8515" ) )
            ui->cbCPU->setCurrentItem( i );
    ui->kdsClock->setValue( (int) conf( PRJ_CLOCKSPEED, "4000000" ).toDouble( &ok ) );
    
    ui->cbExternalRAM->setChecked( conf( PRJ_EXTERNAL_RAM, "unspec" ) != "unspec" );
    if ( ui->cbExternalRAM->isChecked() )
        ui->sbSize->setValue( conf( PRJ_EXTERNAL_RAM, "65356" ).toInt(&ok) );
    
    ui->leHEXFile->setText( conf( PRJ_HEX_FILE, "project.hex" ) );
    ui->leMAPFile->setText( conf( PRJ_MAP_FILE, "project.map" ) );
    ui->leCompilerCommand->setText( conf( PRJ_COMPILER_COMMAND, "avr-gcc" ) );
    ui->cbCallPrologues->setChecked( conf( PRJ_COMPILER_CALL_PROLOGUES, FALSE_STRING ) == TRUE_STRING );
    ui->cbStrictPrototypes->setChecked( conf( PRJ_COMPILER_STRICT_PROTOTYPES, FALSE_STRING ) == TRUE_STRING );
    ui->cbFCPU->setChecked( conf( PRJ_COMPILER_F_CPU, FALSE_STRING ) == TRUE_STRING );
    ui->cbAll->setChecked( conf( PRJ_COMPILER_WALL, FALSE_STRING ) == TRUE_STRING );
    ui->cbGDebug->setChecked( conf( PRJ_COMPILER_GDEBUG, FALSE_STRING ) == TRUE_STRING );
    QString optVal = conf( PRJ_COMPILER_OPT_LEVEL, "0" ).lower();
    for ( int i=0; i < ui->cbOptimizationLevel->count(); i++ )
        if ( ui->cbOptimizationLevel->text( i ).left(1) == optVal )
            ui->cbOptimizationLevel->setCurrentItem( i );

    ui->leLinkerCommand->setText( conf( PRJ_LINKER_COMMAND, "avr-gcc" ) );
    ui->leObjectCopyCommand->setText( conf( PRJ_OBJCOPY_COMMAND, "avr-objcopy" ) );

    ui->cbStartOfText->setChecked( conf( PRJ_LINKER_START_TEXT, "unspec" ) != "unspec" );
    if ( ui->cbStartOfText->isChecked() )
        ui->kisbStartOfText->setValue( conf( PRJ_LINKER_START_TEXT, "0" ).toInt(&ok, 16) );

    ui->cbStartOfData->setChecked( conf( PRJ_LINKER_START_DATA, "unspec" ) != "unspec" );
    if ( ui->cbStartOfData->isChecked() )
        ui->kisbStartOfData->setValue( conf( PRJ_LINKER_START_DATA, "0" ).toInt(&ok, 16) );

    ui->cbStartOfBSS->setChecked( conf( PRJ_LINKER_START_BSS, "unspec" ) != "unspec" );
    if ( ui->cbStartOfBSS->isChecked() )
        ui->kisbStartOfBSS->setValue( conf( PRJ_LINKER_START_BSS, "0" ).toInt(&ok, 16) );

    ui->cbStartOfHeap->setChecked( conf( PRJ_LINKER_START_HEAP, "unspec" ) != "unspec" );
    if ( ui->cbStartOfHeap->isChecked() )
        ui->kisbStartOfHeap->setValue( conf( PRJ_LINKER_START_HEAP, "0" ).toInt(&ok, 16) );

    ui->cbEndOfHeap->setChecked( conf( PRJ_LINKER_END_HEAP, "unspec" ) != "unspec" );
    if ( ui->cbEndOfHeap->isChecked() )
        ui->kisbEndOfHeap->setValue( conf( PRJ_LINKER_END_HEAP, "0" ).toInt( &ok, 16 ) );
    
    ui->leAssemblerCommand->setText( conf( PRJ_ASSEMBLER_COMMAND, "avr-gcc" ) );
    
    QStringList allFlags = QStringList::split( "#", conf( PRJ_LINKER_FLAGS, "" ) );
    // qDebug("allFlags is %s", allFlags.join("!!").ascii());
    QStringList::iterator it;
    for ( it = allFlags.begin(); it != allFlags.end(); ++it )
    {
        // qDebug("searching: %s", (*it).ascii() );
        foreach ( Q3CheckListItem* it2 , m_linkerFlagsCBs )
        {
            if ( it2->text( 1 ).stripWhiteSpace() == (*it).stripWhiteSpace() )
            {
                // qDebug( "Found" );
                it2->setState( Q3CheckListItem::On );
            }
        }
    }
    
    if ( conf( PRJ_BUILD_SYSTEM, PRJ_BUILT_IN_BUILD_SYSTEM ) == PRJ_BUILT_IN_BUILD_SYSTEM )
        ui->rbBuiltIn->setChecked( true );
    else
        ui->rbExecuteMake->setChecked( true );
    
    ui->leMake->setText( conf( PRJ_MAKE_COMMAND, "make" ) );
    ui->leDefaultTarget->setText( conf( PRJ_MAKE_DEFAULT_TARGET, "all" ) );
    ui->leCleanTarget->setText( conf( PRJ_MAKE_CLEAN_TARGET, "clean" ) );
}


QString KLProjectConfigWidget::conf( const QString & confKey, const QString & defval ) const
{
    if ( m_settings.contains( confKey ) )
    {
        // qDebug("found for %s : %s", confKey.ascii(), m_settings[ confKey ].ascii());
        return m_settings[ confKey ];
    }
    else
    {
        KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );

        QString val = group.readEntry( confKey, "" );
        // qDebug("%s empty=%d null=%d", val.ascii(), val.isEmpty(), val.isNull() );
        if ( (!val.isEmpty()) && (!val.isNull()) )
        {
            // qDebug("%s = %s", confKey.ascii(), val.ascii());
            return val;
        }
        else
        {
            // qDebug("returning defval %s for %s", defval.ascii(), confKey.ascii() );
            return defval;
        }
    }
}

void KLProjectConfigWidget::showEvent( QShowEvent * )
{
    m_settings = m_project->settings();
    updateGUIFromSettings();
}

void KLProjectConfigWidget::slotBuildSystemChanged()
{
    if ( ui->rbExecuteMake->isChecked() )
        ui->tlWarning->hide();
    else
        ui->tlWarning->show();
}


