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
#include "klmemoryviewwidget.h"
#include "klmemorycelllistboxitem.h"
#include <knuminput.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <klocale.h>
#include "kontrollerlab.h"
#include "kldebugger.h"
#include "klcpufeatures.h"
#include "klproject.h"
#include "kldocumentview.h"


KLMemoryViewWidget::KLMemoryViewWidget(KontrollerLab *parent, const char *name)
    :QDialog(parent,name), m_updateTimer( this, "timer" ),
    ui(new Ui::KLMemoryViewWidgetBase())
{
    ui->setupUi(this);
    m_parent = parent;
    m_nextUpdateIn=0;
    m_enableAutomaticGUIUpdates = true;
    m_ramEnd = 0;
    setRamEnd( 8192 );
    // By default, don't allow a memory cell to be set.
    allowSetMemoryCell( true );
    connect( &m_updateTimer, SIGNAL(timeout()), this, SLOT(timeoutOfTimer()) );
}

void KLMemoryViewWidget::slotSelectionChanged()
{
}


void KLMemoryViewWidget::slotUpdateEveryChanged( int value )
{
    m_updateTimer.changeInterval( value );
}


void KLMemoryViewWidget::slotUpdateEnable( bool value )
{
    if (value)
    {
        // Single shot only!
        m_updateTimer.start( ui->tniUpdateRegularly->value(), TRUE );
    }
    else
    {
        m_updateTimer.stop();
    }
}


void KLMemoryViewWidget::slotBinaryCBChanged()
{
    int current = 0;
    current |= ui->cbBit7->isChecked()?1<<7:0;
    current |= ui->cbBit6->isChecked()?1<<6:0;
    current |= ui->cbBit5->isChecked()?1<<5:0;
    current |= ui->cbBit4->isChecked()?1<<4:0;
    current |= ui->cbBit3->isChecked()?1<<3:0;
    current |= ui->cbBit2->isChecked()?1<<2:0;
    current |= ui->cbBit1->isChecked()?1<<1:0;
    current |= ui->cbBit0->isChecked()?1   :0;
    
    setGUIValueTo( current );
}


void KLMemoryViewWidget::slotBinaryChanged( int value )
{
    setGUIValueTo( value );
}


void KLMemoryViewWidget::slotCharChanged( const QString& value )
{
    setGUIValueTo( value.at( 0 ).unicode() );
}


void KLMemoryViewWidget::slotDecChanged( int value )
{
    setGUIValueTo( value );
}


void KLMemoryViewWidget::slotHexChanged( int value )
{
    setGUIValueTo( value );
}


void KLMemoryViewWidget::slotNameChanged( const QString& )
{
}


void KLMemoryViewWidget::slotUpdate()
{
    KLMemoryCellListBoxItem* item =
            dynamic_cast<KLMemoryCellListBoxItem*>( ui->lbMemory->item( ui->lbMemory->currentItem() ) );

    // Fix for crash bug: When item is 0 because no element was selected,
    // simply do nothing instead of violating the segment:
    if ( m_parent && m_parent->debugger() && item )
        m_parent->debugger()->readMemoryCell( item->address() );
}


void KLMemoryViewWidget::slotSetMemoryCell()
{
    KLMemoryCellListBoxItem* item =
            dynamic_cast<KLMemoryCellListBoxItem*>( ui->lbMemory->item( ui->lbMemory->currentItem() ) );

    if (item)
    {
        item->setValue( ui->kisDec->value() );
        item->setName( ui->leName->text() );
        ui->lbMemory->updateContents();
        emit( memoryCellSet( item->address(), item->value() ) );
    }
}


void KLMemoryViewWidget::setGUIValueTo(int value)
{
    if ( ui->kisDec->value() != value )
        ui->kisDec->setValue( value );
    if ( ui->kisBinary->value() != value )
        ui->kisBinary->setValue( value );
    if ( ui->kisHex->value() != value )
        ui->kisHex->setValue( value );
    
    if ( ui->cbBit7->isChecked() != ((bool) (value & 0x80)) )
        ui->cbBit7->setChecked( value & 0x80 );
    if ( ui->cbBit6->isChecked() != ((bool) (value & 0x40)) )
        ui->cbBit6->setChecked( value & 0x40 );
    if ( ui->cbBit5->isChecked() != ((bool) (value & 0x20)) )
        ui->cbBit5->setChecked( value & 0x20 );
    if ( ui->cbBit4->isChecked() != ((bool) (value & 0x10)) )
        ui->cbBit4->setChecked( value & 0x10 );
    if ( ui->cbBit3->isChecked() != ((bool) (value & 0x08)) )
        ui->cbBit3->setChecked( value & 0x08 );
    if ( ui->cbBit2->isChecked() != ((bool) (value & 0x04)) )
        ui->cbBit2->setChecked( value & 0x04 );
    if ( ui->cbBit1->isChecked() != ((bool) (value & 0x02)) )
        ui->cbBit1->setChecked( value & 0x02 );
    if ( ui->cbBit0->isChecked() != ((bool) (value & 0x01)) )
        ui->cbBit0->setChecked( value & 0x01 );

    if ( ui->leChar->text() != QChar( value ) )
        ui->leChar->setText( QChar( value ) );
}

void KLMemoryViewWidget::slotCurrentItemChanged(Q3ListBoxItem *item_ )
{
    KLMemoryCellListBoxItem* item =
            dynamic_cast<KLMemoryCellListBoxItem*>( item_ );

    if (item)
    {
        setGUIValueTo( item->value() );
        ui->gbMemoryCell->setTitle( QString( i18n("Memory cell 0x%1") ).arg( item->address(), 0, 16 ) );
        ui->leName->setText( item->name() );
        ui->tlDescription->setText( item->description() );
    }
}

void KLMemoryViewWidget::allowSetMemoryCell( bool value )
{
    if ( value )
        ui->gbMemoryCell->show();
    else
        ui->gbMemoryCell->hide();
}


void KLMemoryViewWidget::slotMemoryReadCompleted( unsigned int adr, unsigned char val )
{
    KLMemoryCellListBoxItem* item =
            dynamic_cast<KLMemoryCellListBoxItem*>( ui->lbMemory->item( adr ) );

    if (item)
    {
        item->setValue( val );
        ui->lbMemory->repaint( ui->lbMemory->itemRect( item ) );
    }
    if ( m_nextUpdateIn > 0 )
        m_nextUpdateIn--;
    if ( (m_nextUpdateIn == 0) && m_enableAutomaticGUIUpdates )
    {
        ui->lbMemory->updateContents();
        if ( ui->cbUpdateRegularly->isChecked() )
            // Single shot only!
            m_updateTimer.start( ui->tniUpdateRegularly->value(), TRUE );
    }
}

void KLMemoryViewWidget::timeoutOfTimer( )
{
    // qDebug("timeout");
    m_nextUpdateIn = 0;
    if ( m_parent->debugger()->requestCount() != 0 )
        return;
    for ( unsigned int i=0; i < (unsigned int) m_ramEnd; i++ )
    {
        if ( ui->lbMemory->isSelected( i ) )
        {
            // qDebug("req %d", i);
            m_parent->debugger()->readMemoryCell( i );
            m_nextUpdateIn++;
        }
    }
}

void KLMemoryViewWidget::updateGUI( )
{
    ui->lbMemory->updateContents();
}


void KLMemoryViewWidget::setAllValuesToZero( )
{
    for ( unsigned int i=0; i < ui->lbMemory->count(); i++ )
    {
        KLMemoryCellListBoxItem* item =
                dynamic_cast<KLMemoryCellListBoxItem*>( ui->lbMemory->item( i ) );
        item->setValue( 0 );
    }
}

void KLMemoryViewWidget::setRamEnd( const int & theValue )
{
    if (m_ramEnd < theValue)
    {
        unsigned int i;
        for (i=m_ramEnd; i<32; i++)
        {
            ui->lbMemory->insertItem( new KLMemoryCellListBoxItem( i, 0,
                                  QString("r%1").arg(i) ) );
        }
        m_ramEnd = theValue;
        for (; i<(unsigned int)m_ramEnd; i++)
        {
            ui->lbMemory->insertItem( new KLMemoryCellListBoxItem( i, 0, "" ) );
        }
    }
    else
    {
        m_ramEnd = theValue;
        ui->lbMemory->clear();
        unsigned int i;
        for (i=0; i<32; i++)
        {
            ui->lbMemory->insertItem( new KLMemoryCellListBoxItem( i, 0,
                                QString("r%1").arg(i) ) );
        }
        for (; i<(unsigned int)m_ramEnd; i++)
        {
            ui->lbMemory->insertItem( new KLMemoryCellListBoxItem( i, 0, "" ) );
        }
    }
}


void KLMemoryViewWidget::slotCPUNameChanged(const QString &)
{
    // qDebug("Changing to CPU %s", newName.ascii() );
    if ( m_parent && m_parent->project() )
    {
        KLCPUFeatures cpuFeat = m_parent->project()->currentCpuFeatures();
        KLCPURegisterDescriptionList rdl = cpuFeat.registerDescriptions();

        // Reset the old descriptions and names:
        for ( KLCPURegisterDescriptionList::iterator it = m_currentlyDisplayedRdl.begin();
              it != m_currentlyDisplayedRdl.end(); ++it )
        {
            KLMemoryCellListBoxItem *lbItem =
                    dynamic_cast<KLMemoryCellListBoxItem*> (
                    ui->lbMemory->item( (*it).location() ) );
            lbItem->setName( "" );
            lbItem->setDescription( "" );
        }
        // Set the new ones:
        for ( KLCPURegisterDescriptionList::iterator it = rdl.begin();
              it != rdl.end(); ++it )
        {
            KLMemoryCellListBoxItem *lbItem =
                    dynamic_cast<KLMemoryCellListBoxItem*> (
                        ui->lbMemory->item( (*it).location() ) );
            lbItem->setName( (*it).name() );
            lbItem->setDescription( (*it).description() );
        }
        m_currentlyDisplayedRdl = rdl;
        ui->lbMemory->triggerUpdate( true );
        ui->tlDescription->setText("");
    }
}


