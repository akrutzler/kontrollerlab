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
#include "klprogramfuseswidget.h"
#include "klcpufuses.h"
#include "klproject.h"
#include "klprogrammerinterface.h"
#include <qcheckbox.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qcombobox.h>
#include "kldocumentview.h"


KLProgramFusesWidget::KLProgramFusesWidget(QWidget *parent, KLProject* project, const char *name)
    :QDialog(parent, name), m_project( project ), ui(new Ui::KLProgramFusesWidgetBase)
{
    ui->setupUi(this);
    m_fuses = m_project->getFusesFor( m_project->cpu() );
    
    m_highByte = m_lowByte = m_extByte = m_lockByte = 0xFF;
    updateGUIFromData();
    
    ui->cbCPU->insertStringList( m_project->cpus() );
}

void KLProgramFusesWidget::slotClose()
{
    close();
}


void KLProgramFusesWidget::slotWrite()
{
    updateDataFromGUI();

    int retVal = KMessageBox::questionYesNo( this,
                                             i18n("Do You really want do write the fuses? "
                                                  "This can damage the CPU irreparably."),
                                             i18n("Fuse bits"),
                                             KStandardGuiItem::yes(), KStandardGuiItem::no(),
                                             "kontrollerlab_write_fuses" );

    if ( retVal == KMessageBox::Yes )
    {
        QMap< QString, QString > fuses;
        fuses[ FUSE_L ] = QString("0x%1").arg( m_lowByte, 0, 16 );
        fuses[ FUSE_H ] = QString("0x%1").arg( m_highByte, 0, 16 );
        fuses[ FUSE_E ] = QString("0x%1").arg( m_extByte, 0, 16 );
        fuses[ FUSE_LOCK ] = QString("0x%1").arg( m_lockByte, 0, 16 );
        m_project->programFuses( fuses, ui->cbCPU->currentText() );
    }

}


void KLProgramFusesWidget::slotRead()
{
    m_project->readFuses( ui->cbCPU->currentText() );
}


void KLProgramFusesWidget::updateGUIFromData()
{
    ui->cbExt0->setEnabled( m_fuses.extCanBeChanged()[0] );
    ui->cbExt1->setEnabled( m_fuses.extCanBeChanged()[1] );
    ui->cbExt2->setEnabled( m_fuses.extCanBeChanged()[2] );
    ui->cbExt3->setEnabled( m_fuses.extCanBeChanged()[3] );
    ui->cbExt4->setEnabled( m_fuses.extCanBeChanged()[4] );
    ui->cbExt5->setEnabled( m_fuses.extCanBeChanged()[5] );
    ui->cbExt6->setEnabled( m_fuses.extCanBeChanged()[6] );
    ui->cbExt7->setEnabled( m_fuses.extCanBeChanged()[7] );
    ui->cbExt0->setText( m_fuses.extNames()[0] );
    ui->cbExt1->setText( m_fuses.extNames()[1] );
    ui->cbExt2->setText( m_fuses.extNames()[2] );
    ui->cbExt3->setText( m_fuses.extNames()[3] );
    ui->cbExt4->setText( m_fuses.extNames()[4] );
    ui->cbExt5->setText( m_fuses.extNames()[5] );
    ui->cbExt6->setText( m_fuses.extNames()[6] );
    ui->cbExt7->setText( m_fuses.extNames()[7] );

    ui->cbLow0->setEnabled( m_fuses.lowCanBeChanged()[0] );
    ui->cbLow1->setEnabled( m_fuses.lowCanBeChanged()[1] );
    ui->cbLow2->setEnabled( m_fuses.lowCanBeChanged()[2] );
    ui->cbLow3->setEnabled( m_fuses.lowCanBeChanged()[3] );
    ui->cbLow4->setEnabled( m_fuses.lowCanBeChanged()[4] );
    ui->cbLow5->setEnabled( m_fuses.lowCanBeChanged()[5] );
    ui->cbLow6->setEnabled( m_fuses.lowCanBeChanged()[6] );
    ui->cbLow7->setEnabled( m_fuses.lowCanBeChanged()[7] );
    ui->cbLow0->setText( m_fuses.lowNames()[0] );
    ui->cbLow1->setText( m_fuses.lowNames()[1] );
    ui->cbLow2->setText( m_fuses.lowNames()[2] );
    ui->cbLow3->setText( m_fuses.lowNames()[3] );
    ui->cbLow4->setText( m_fuses.lowNames()[4] );
    ui->cbLow5->setText( m_fuses.lowNames()[5] );
    ui->cbLow6->setText( m_fuses.lowNames()[6] );
    ui->cbLow7->setText( m_fuses.lowNames()[7] );

    ui->cbHigh0->setEnabled( m_fuses.highCanBeChanged()[0] );
    ui->cbHigh1->setEnabled( m_fuses.highCanBeChanged()[1] );
    ui->cbHigh2->setEnabled( m_fuses.highCanBeChanged()[2] );
    ui->cbHigh3->setEnabled( m_fuses.highCanBeChanged()[3] );
    ui->cbHigh4->setEnabled( m_fuses.highCanBeChanged()[4] );
    ui->cbHigh5->setEnabled( m_fuses.highCanBeChanged()[5] );
    ui->cbHigh6->setEnabled( m_fuses.highCanBeChanged()[6] );
    ui->cbHigh7->setEnabled( m_fuses.highCanBeChanged()[7] );
    ui->cbHigh0->setText( m_fuses.highNames()[0] );
    ui->cbHigh1->setText( m_fuses.highNames()[1] );
    ui->cbHigh2->setText( m_fuses.highNames()[2] );
    ui->cbHigh3->setText( m_fuses.highNames()[3] );
    ui->cbHigh4->setText( m_fuses.highNames()[4] );
    ui->cbHigh5->setText( m_fuses.highNames()[5] );
    ui->cbHigh6->setText( m_fuses.highNames()[6] );
    ui->cbHigh7->setText( m_fuses.highNames()[7] );
    
    ui->cbChecked->setPaletteForegroundColor( foregroundColor() );
    ui->cbUnchecked->setPaletteForegroundColor( foregroundColor() );
    
    int mask = 0;
    for ( int i = 0; i < 8; i++ )
    {
        mask = 1 << i;
        if ( 0 == i )
            ui->cbHigh0->setChecked( !(mask & m_highByte) );
        else if ( 1 == i )
            ui->cbHigh1->setChecked( !(mask & m_highByte) );
        else if ( 2 == i )
            ui->cbHigh2->setChecked( !(mask & m_highByte) );
        else if ( 3 == i )
            ui->cbHigh3->setChecked( !(mask & m_highByte) );
        else if ( 4 == i )
            ui->cbHigh4->setChecked( !(mask & m_highByte) );
        else if ( 5 == i )
            ui->cbHigh5->setChecked( !(mask & m_highByte) );
        else if ( 6 == i )
            ui->cbHigh6->setChecked( !(mask & m_highByte) );
        else if ( 7 == i )
            ui->cbHigh7->setChecked( !(mask & m_highByte) );
    
        if ( 0 == i )
            ui->cbLow0->setChecked( !(mask & m_lowByte) );
        else if ( 1 == i )
            ui->cbLow1->setChecked( !(mask & m_lowByte) );
        else if ( 2 == i )
            ui->cbLow2->setChecked( !(mask & m_lowByte) );
        else if ( 3 == i )
            ui->cbLow3->setChecked( !(mask & m_lowByte) );
        else if ( 4 == i )
            ui->cbLow4->setChecked( !(mask & m_lowByte) );
        else if ( 5 == i )
            ui->cbLow5->setChecked( !(mask & m_lowByte) );
        else if ( 6 == i )
            ui->cbLow6->setChecked( !(mask & m_lowByte) );
        else if ( 7 == i )
            ui->cbLow7->setChecked( !(mask & m_lowByte) );
    
        if ( 0 == i )
            ui->cbExt0->setChecked( !(mask & m_extByte) );
        else if ( 1 == i )
            ui->cbExt1->setChecked( !(mask & m_extByte) );
        else if ( 2 == i )
            ui->cbExt2->setChecked( !(mask & m_extByte) );
        else if ( 3 == i )
            ui->cbExt3->setChecked( !(mask & m_extByte) );
        else if ( 4 == i )
            ui->cbExt4->setChecked( !(mask & m_extByte) );
        else if ( 5 == i )
            ui->cbExt5->setChecked( !(mask & m_extByte) );
        else if ( 6 == i )
            ui->cbExt6->setChecked( !(mask & m_extByte) );
        else if ( 7 == i )
            ui->cbExt7->setChecked( !(mask & m_extByte) );
    
        if ( 0 == i )
            ui->cbLock0->setChecked( !(mask & m_lockByte) );
        else if ( 1 == i )
            ui->cbLock1->setChecked( !(mask & m_lockByte) );
        else if ( 2 == i )
            ui->cbLock2->setChecked( !(mask & m_lockByte) );
        else if ( 3 == i )
            ui->cbLock3->setChecked( !(mask & m_lockByte) );
        else if ( 4 == i )
            ui->cbLock4->setChecked( !(mask & m_lockByte) );
        else if ( 5 == i )
            ui->cbLock5->setChecked( !(mask & m_lockByte) );
        else if ( 6 == i )
            ui->cbLock6->setChecked( !(mask & m_lockByte) );
        else if ( 7 == i )
            ui->cbLock7->setChecked( !(mask & m_lockByte) );
    }
}


void KLProgramFusesWidget::updateDataFromGUI()
{
    int mask = 0;
    m_highByte = m_lockByte = m_lowByte = m_extByte = 0;
    for ( int i = 0; i < 8; i++ )
    {
        mask = 1 << i;
        if ( ( 0 == i ) && ui->cbHigh0->isChecked() ) m_highByte |= mask;
        else if ( ( 1 == i ) && ui->cbHigh1->isChecked() ) m_highByte |= mask;
        else if ( ( 2 == i ) && ui->cbHigh2->isChecked() ) m_highByte |= mask;
        else if ( ( 3 == i ) && ui->cbHigh3->isChecked() ) m_highByte |= mask;
        else if ( ( 4 == i ) && ui->cbHigh4->isChecked() ) m_highByte |= mask;
        else if ( ( 5 == i ) && ui->cbHigh5->isChecked() ) m_highByte |= mask;
        else if ( ( 6 == i ) && ui->cbHigh6->isChecked() ) m_highByte |= mask;
        else if ( ( 7 == i ) && ui->cbHigh7->isChecked() ) m_highByte |= mask;
    
        if ( ( 0 == i ) && ui->cbLow0->isChecked() ) m_lowByte |= mask;
        else if ( ( 1 == i ) && ui->cbLow1->isChecked() ) m_lowByte |= mask;
        else if ( ( 2 == i ) && ui->cbLow2->isChecked() ) m_lowByte |= mask;
        else if ( ( 3 == i ) && ui->cbLow3->isChecked() ) m_lowByte |= mask;
        else if ( ( 4 == i ) && ui->cbLow4->isChecked() ) m_lowByte |= mask;
        else if ( ( 5 == i ) && ui->cbLow5->isChecked() ) m_lowByte |= mask;
        else if ( ( 6 == i ) && ui->cbLow6->isChecked() ) m_lowByte |= mask;
        else if ( ( 7 == i ) && ui->cbLow7->isChecked() ) m_lowByte |= mask;
    
        if ( ( 0 == i ) && ui->cbExt0->isChecked() ) m_extByte |= mask;
        else if ( ( 1 == i ) && ui->cbExt1->isChecked() ) m_extByte |= mask;
        else if ( ( 2 == i ) && ui->cbExt2->isChecked() ) m_extByte |= mask;
        else if ( ( 3 == i ) && ui->cbExt3->isChecked() ) m_extByte |= mask;
        else if ( ( 4 == i ) && ui->cbExt4->isChecked() ) m_extByte |= mask;
        else if ( ( 5 == i ) && ui->cbExt5->isChecked() ) m_extByte |= mask;
        else if ( ( 6 == i ) && ui->cbExt6->isChecked() ) m_extByte |= mask;
        else if ( ( 7 == i ) && ui->cbExt7->isChecked() ) m_extByte |= mask;
    
        if ( ( 0 == i ) && ui->cbLock0->isChecked() ) m_lockByte |= mask;
        else if ( ( 1 == i ) && ui->cbLock1->isChecked() ) m_lockByte |= mask;
        else if ( ( 2 == i ) && ui->cbLock2->isChecked() ) m_lockByte |= mask;
        else if ( ( 3 == i ) && ui->cbLock3->isChecked() ) m_lockByte |= mask;
        else if ( ( 4 == i ) && ui->cbLock4->isChecked() ) m_lockByte |= mask;
        else if ( ( 5 == i ) && ui->cbLock5->isChecked() ) m_lockByte |= mask;
        else if ( ( 6 == i ) && ui->cbLock6->isChecked() ) m_lockByte |= mask;
        else if ( ( 7 == i ) && ui->cbLock7->isChecked() ) m_lockByte |= mask;
    }
    m_highByte = ~m_highByte;
    m_lockByte = ~m_lockByte;
    m_lowByte = ~m_lowByte;
    m_extByte = ~m_extByte;
}


void KLProgramFusesWidget::showEvent( QShowEvent* )
{
    m_fuses = m_project->getFusesFor( m_project->cpu() );
    updateGUIFromData();
    ui->cbCPU->setCurrentText( m_project->cpu() );
}


void KLProgramFusesWidget::setFuseBits( unsigned char lowByte,
                                        unsigned char highByte,
                                        unsigned char extByte,
                                        unsigned char lockByte )
{
    m_lockByte = lockByte;
    m_lowByte = lowByte;
    m_highByte = highByte;
    m_extByte = extByte;
    updateGUIFromData();
}

void KLProgramFusesWidget::setFuseBits( const QString & lowByte,
                                        const QString & highByte,
                                        const QString & extByte,
                                        const QString & lockByte )
{
    unsigned char lowB, highB, extB, lockB;
    bool ok1, ok2, ok3, ok4;
    lowB = lowByte.toUShort( &ok1, 16 );
    highB = highByte.toUShort( &ok2, 16 );
    lockB = lockByte.toUShort( &ok3, 16 );
    extB = extByte.toUShort( &ok4, 16 );
    
    if ( ok1 && ok2 && ok3 && ok4 )
    {
        m_highByte = highB;
        m_lowByte = lowB;
        m_extByte = extB;
        m_lockByte = lockB;
        updateGUIFromData();
    }
    else
    {
        qWarning( "There was an error while converting the " \
                "fuse bit strings to hex values in %s:%d.", __FILE__, __LINE__ );
    }
}

void KLProgramFusesWidget::slotCPUChanged( const QString& name )
{
    m_fuses = m_project->getFusesFor( name );
    updateGUIFromData();
}


