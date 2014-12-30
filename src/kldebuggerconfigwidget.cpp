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
#include "kldebuggerconfigwidget.h"
#include "kontrollerlab.h"
#include "kldebugger.h"
#include "klserialterminalwidget.h"
#include "klproject.h"
#include <qspinbox.h>
#include <knuminput.h>
#include <klocale.h>
#include "klmemoryviewwidget.h"
#include <kapplication.h>
#include <math.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <ktempdir.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kio/copyjob.h>
#include <kconfiggroup.h>

#include "kldebuggermemorymapping.h"
#include "kldebuggermemmappinglistviewitem.h"

#include "kldocumentview.h"


KLDebuggerConfigWidget::KLDebuggerConfigWidget(KontrollerLab *parent, const char *name)
    :QDialog(parent,name), ui(new Ui::KLDebuggerConfigWidgetBase)
{
    ui->setupUi(this);

    ui->lvMappings->addColumn(i18n("From"));
    ui->lvMappings->addColumn(i18n("To"));

    m_noCallback = false;
    m_parent = parent;
    if (m_parent)
    {
        ui->cbBAUD->insertStringList( m_parent->serialTerminalWidget()->baudRates() );
        ui->cbPort->insertStringList( m_parent->serialTerminalWidget()->ports() );
        KLMemoryViewWidget* memView = m_parent->memoryViewWidget();
        ui->sbMemoryViewByteCount->setValue( memView->ramEnd() );
    }

    updateGUIFromSettings();
}

void KLDebuggerConfigWidget::slotUBRRChanged( int i )
{
    // qDebug("%d", i);
    bool ok;
    int val = ui->cbBAUD->currentText().toInt( &ok );
    if ( ok )
    {
        double clk = m_parent->project()->clock();
        int ubrr = i;
        if ( ubrr > 255 )
            ubrr = 255;
        double ubrr_exact = clk / ( 16 * val ) - 1;
        double err = fabs(ubrr / ubrr_exact - 1) * 100.0;
        if ( err >= 2.0 )
            ui->tlError->setText( i18n(" Error <b><font color=\"red\">%1%</font></b>").arg( err, 0, 'f', 2 ) );
        else
            ui->tlError->setText( i18n(" Error <b>%1%</b>").arg( err, 0, 'f', 2 ) );
    }
}


void KLDebuggerConfigWidget::slotBAUDRateChanged( const QString& value )
{
    bool ok;
    int val = value.toInt( &ok );
    if ( ok )
    {
        double clk = m_parent->project()->clock();
        int ubrr = (int) (clk / ( 16 * val ) - 1);
        if ( ubrr > 255 )
            ubrr = 255;
        // double ubrr_exact = clk / ( 16 * val ) - 1;
        ui->sbUBRR->setValue( ubrr );
        // double err = fabs(ubrr / ubrr_exact - 1) * 100.0;
        // if ( err >= 2.0 )
        //     tlError->setText( i18n(" Error <b><font color=\"red\">%1%</font></b>").arg( err, 0, 'f', 2 ) );
        // else
        //     tlError->setText( i18n(" Error <b>%1%</b>").arg( err, 0, 'f', 2 ) );
    }
}


void KLDebuggerConfigWidget::slotBuildAndDownload()
{
    KTempDir temp;
    KUrl destURL( temp.name() );
    m_tempURL = destURL;
    // qDebug( "copying to %s", temp.name().ascii() );
    KStandardDirs *dirs = KGlobal::dirs();
    QString resPathMS = dirs->findResource( "data",
        QString("kontrollerlab/templates/%1").arg( "monitor.c" ) );
    QString resPathUS = dirs->findResource( "data",
        QString("kontrollerlab/templates/%1").arg( "uart.c" ) );
    QString resPathUH = dirs->findResource( "data",
        QString("kontrollerlab/templates/%1").arg( "uart.h" ) );

    m_buildUrlList.clear();
    m_buildUrlList.append( KUrl( resPathMS ) );
    m_buildUrlList.append( KUrl( resPathUS ) );
    m_buildUrlList.append( KUrl( resPathUH ) );

    KIO::CopyJob* job = KIO::copy( m_buildUrlList, destURL );
    connect( job, SIGNAL( copyingDone( KIO::Job *, const KUrl &, const KUrl &, time_t, bool, bool ) ),
             this, SLOT(copyMonitorFilesDone( KIO::Job *, const KUrl &, const KUrl &, time_t, bool, bool ) ) );
}


void KLDebuggerConfigWidget::slotCheckState()
{
}


void KLDebuggerConfigWidget::slotCancel()
{
    close();
}


void KLDebuggerConfigWidget::slotOK()
{
    KLMemoryViewWidget* memView = m_parent->memoryViewWidget();
    memView->setRamEnd( ui->sbMemoryViewByteCount->value() );
    updateSettingsFromGUI();
    if ( m_parent )
    {
        bool changed = false;
        QMap<QString, QString> comp = m_parent->project()->debuggerSettings();
        for ( QMap<QString, QString>::iterator it = comp.begin();
              it != comp.end(); ++it )
        {
            if ( it.data() != m_settings[ it.key() ] )
            {
                changed = true;
                break;
            }
        }
        if ( changed )
            m_parent->project()->setUnsaved();
        m_parent->project()->setDebuggerSettings( m_settings );
    }
    close();
}

void KLDebuggerConfigWidget::slotSetDefault( )
{
    KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );
    QMap<QString, QString>::Iterator it;

    updateSettingsFromGUI();

    group.deleteEntry( DEBUGGER_OBJDUMP_COMMAND );
    group.deleteEntry( DEBUGGER_COM_PORT );
    group.deleteEntry( DEBUGGER_COM_BAUD );
    group.deleteEntry( DEBUGGER_COM_UBRR );
    group.deleteEntry( DEBUGGER_MONITOR_CATCH_INTERRUPTS );
    group.deleteEntry( DEBUGGER_INTERRUPT_COUNT );
    group.deleteEntry( DEBUGGER_MEMORY_VIEW_BYTECOUNT );
    group.deleteEntry( DEBUGGER_MAPPINGS );

    for ( it = m_settings.begin(); it != m_settings.end(); ++it )
    {
        // qDebug("%s = %s", it.key().ascii(), it.data().ascii() );
        group.writeEntry( it.key(), it.data() );
    }
    group.sync();
}

void KLDebuggerConfigWidget::showEvent( QShowEvent * )
{
    if ( m_parent && m_parent->debugger() )
    {
        m_settings = m_parent->project()->debuggerSettings();
        updateGUIFromSettings();
    }
}

void KLDebuggerConfigWidget::updateSettingsFromGUI( )
{
    m_settings[ DEBUGGER_OBJDUMP_COMMAND ] = ui->leObjdump->text();
    m_settings[ DEBUGGER_COM_PORT ] = ui->cbPort->currentText();
    m_settings[ DEBUGGER_COM_BAUD ] = ui->cbBAUD->currentText();
    m_settings[ DEBUGGER_COM_UBRR ] = QString::number( ui->sbUBRR->value() );
    m_settings[ DEBUGGER_MONITOR_CATCH_INTERRUPTS ] = ui->cbCatchInterrupts->isChecked() ? TRUE_STRING : FALSE_STRING;
    m_settings[ DEBUGGER_INTERRUPT_COUNT ] = QString::number( ui->sbInterruptCount->value() );
    m_settings[ DEBUGGER_MEMORY_VIEW_BYTECOUNT ] = QString::number( ui->sbMemoryViewByteCount->value() );
    QString mapping = "";
    KLDebuggerMemoryMappingList lst = mappingList();
    m_settings[ DEBUGGER_MAPPINGS ] = lst.toString();
}

void KLDebuggerConfigWidget::updateGUIFromSettings( )
{
    ui->leObjdump->setText( conf( DEBUGGER_OBJDUMP_COMMAND , "avr-objdump" ) );
    ui->cbPort->setCurrentText( conf( DEBUGGER_COM_PORT, "/dev/ttyS0" ) );
    ui->cbBAUD->setCurrentText( conf( DEBUGGER_COM_BAUD, "19200" ) );
    bool ok;
    unsigned int ubrr = conf( DEBUGGER_COM_UBRR, "" ).toInt( &ok );
    if ( ok )
        ui->sbUBRR->setValue( ubrr );
    ui->cbCatchInterrupts->setChecked( conf( DEBUGGER_MONITOR_CATCH_INTERRUPTS, TRUE_STRING ) == TRUE_STRING );
    ui->sbInterruptCount->setValue( conf( DEBUGGER_INTERRUPT_COUNT, "0" ).toInt( &ok ) );
    ui->sbMemoryViewByteCount->setValue( conf( DEBUGGER_MEMORY_VIEW_BYTECOUNT, "0" ).toInt( &ok ) );

    Q3ListView *lv = ui->lvMappings;
    lv->clear();
    QString mappings = m_settings[ DEBUGGER_MAPPINGS ];
    KLDebuggerMemoryMappingList lst;
    lst.fromString( mappings );
    foreach ( const KLDebuggerMemoryMapping it , lst )
    {
        lv->insertItem( new KLDebuggerMemMappingListViewItem( lv, it.from(), it.to() ) );
    }
}

QString KLDebuggerConfigWidget::conf( const QString & confKey, const QString & defval ) const
{
    /// @TODO Unify all conf functions in KontrollerLab!
    if ( m_settings.contains( confKey ) )
    {
        // qDebug("found for %s : %s", confKey.ascii(), m_settings[ confKey ].ascii());
        return m_settings[ confKey ];
    }
    else
    {
        KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );
        QString val = group.readEntry( confKey, "" );
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

void KLDebuggerConfigWidget::slotAdd( )
{
    Q3ListView *lv = ui->lvMappings;
    lv->insertItem( new KLDebuggerMemMappingListViewItem( lv, ui->sbFrom->value(), ui->sbTo->value() ) );

    /*
    lv->insertItem( new Q3ListViewItem( lv,
                    "0x" + QString::number( ui->sbFrom->value(), 16 ),
                    "0x" + QString::number( ui->sbTo->value(), 16 ) ) );
    */

}


void KLDebuggerConfigWidget::copyMonitorFilesDone(KIO::Job *, const KUrl &from, const KUrl &, time_t, bool, bool )
{
    if ( !m_parent )
        return;
    if ( !m_parent->project() )
        return;
    m_buildUrlList.remove( from );
    if ( m_buildUrlList.count() > 0 )
        return;
    QStringList list;
    list.append( "monitor.c" );
    list.append( "uart.c" );
    m_parent->project()->buildAndDownloadExternal( "monitor.hex", m_tempURL.path(), list );
}

void KLDebuggerConfigWidget::slotToChanged( int val )
{
    if (m_noCallback)
        return;
    Q3ListView *lv = ui->lvMappings;
    KLDebuggerMemMappingListViewItem *sel = dynamic_cast< KLDebuggerMemMappingListViewItem* >( lv->selectedItem() );
    if (sel)
    {
        if ( ( (int) sel->to() ) != val )
        {
            sel->setTo( (unsigned int) val );
        }
    }
    if ( ui->sbFrom->value() > val )
        ui->sbFrom->setValue( val );
}


void KLDebuggerConfigWidget::slotFromChanged( int val )
{
    if (m_noCallback)
        return;
    Q3ListView *lv = ui->lvMappings;
    KLDebuggerMemMappingListViewItem *sel = dynamic_cast< KLDebuggerMemMappingListViewItem* >( lv->selectedItem() );
    if (sel)
    {
        if ( ( (int ) sel->from() ) != val )
        {
            sel->setFrom( (unsigned int) val );
        }
    }
    if ( ui->sbTo->value() < val )
        ui->sbTo->setValue( val );
}

void KLDebuggerConfigWidget::slotSelectedMappingChanged( Q3ListViewItem* item )
{
    m_noCallback = true;
    KLDebuggerMemMappingListViewItem *sel = dynamic_cast< KLDebuggerMemMappingListViewItem* >( item );
    if (!sel)
        return;
    if ( ui->sbTo->value() != ( (int) sel->to() ) )
        ui->sbTo->setValue( sel->to() );
    if ( ui->sbFrom->value() != ( (int) sel->from() ) )
        ui->sbFrom->setValue( sel->from() );
    m_noCallback = false;
}


KLDebuggerMemoryMappingList KLDebuggerConfigWidget::mappingList( )
{
    KLDebuggerMemoryMappingList lst;
    Q3ListView *lv = ui->lvMappings;
    Q3ListViewItem* it = lv->firstChild();
    for ( ; it; it = it->nextSibling() )
    {
        KLDebuggerMemMappingListViewItem* map = dynamic_cast< KLDebuggerMemMappingListViewItem* >( it );
        if ( !map )
            continue;
        lst.append( map->mapping() );
    }
    return lst;
}

void KLDebuggerConfigWidget::slotRemove()
{
    Q3ListView *lv = ui->lvMappings;
    if ( !lv->selectedItem() )
    {
        return;
    }
    KLDebuggerMemMappingListViewItem* map = dynamic_cast< KLDebuggerMemMappingListViewItem* >( lv->selectedItem() );
    if ( !map )
    {
        return;
    }
    lv->removeItem( map );
}


