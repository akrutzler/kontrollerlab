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
#include "kldotmatrixwidget.h"
#include "kldocument.h"
#include <klocale.h>
#include <qlabel.h>
#include <knuminput.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include "klproject.h"
#include <kglobal.h>
#include <kstandarddirs.h>
#include "kontrollerlab.h"
#include <kmessagebox.h>
#include "kleditdotswidget.h"
#include "kldocumentview.h"


KLDotMatrixWidget::KLDotMatrixWidget(KontrollerLab *parent, const char *name, KLProject* prj, KLDocument* doc)
    :QDialog(parent, name), m_validator(QRegExp("0x[0-9a-fA-F]{1,2}"), this, "reValidator"),
      ui(new Ui_KLDotMatrixDisplayWizardBase)
{
    ui->setupUi(this);
    m_project = prj;
    m_document = doc;
    m_parent = parent;

    m_header = m_project->getDocumentNamed( WIZARD_HEADER_FILE );
    m_source = m_project->getDocumentNamed( WIZARD_SOURCE_FILE );
    m_settings = KLWizardHeaderSettings( m_header );

    //ui->sbValue->setValidator( &m_validator );

    m_settings.append( KLWizardHeaderSetting( i18n("General"), "XTAL",
                       i18n("The clock frequency. F_CPU can be set in the project configuration menu."),
                       "F_CPU"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_CONTROLLER_KS0073",
                       i18n("Use 0 for HD44780 controller, change to 1 for displays with KS0073 controller."),
                       0, 0, 1  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_LINES",
                       i18n("The number of lines."),
                       1, 1, 4  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_DISP_LENGTH",
                       i18n("The number of characters in one line."),
                       16, 1, 256 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_LINE_LENGTH",
                       i18n("The line length."),
                       0x40, 1, 256, 16 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_START_LINE1",
                       i18n("The start of line 1."),
                       0x00, 0, 256, 16 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_START_LINE2",
                       i18n("The start of line 2."),
                       0x40, 0, 256, 16 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_START_LINE3",
                       i18n("The start of line 3."),
                       0x14, 0, 256, 16 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_START_LINE4",
                       i18n("The start of line 4."),
                       0x54, 0, 256, 16 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("General"), "LCD_WRAP_LINES",
                       i18n("0: no wrap, 1: wrap at end of visibile line"),
                       0, 0, 1 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_IO_MODE",
                       i18n("0: memory mapped mode, 1: IO port mode"),
                       1, 0, 1 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_PORT",
                       i18n("The common LCD port"), "PORTA" ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA0_PORT",
                       i18n("The LCD DATA0 pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA1_PORT",
                       i18n("The LCD DATA1 pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA2_PORT",
                       i18n("The LCD DATA2 pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA3_PORT",
                       i18n("The LCD DATA3 pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA0_PIN",
                       i18n("The data 0 pin"),
                       0, 0, 7 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA1_PIN",
                       i18n("The data 1 pin"),
                       1, 0, 7 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA2_PIN",
                       i18n("The data 2 pin"),
                       2, 0, 7 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_DATA3_PIN",
                       i18n("The data 3 pin"),
                       3, 0, 7 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_RS_PORT",
                       i18n("The LCD RS pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_RS_PIN",
                       i18n("The RS pin"),
                       4, 0, 7 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_RW_PORT",
                       i18n("The LCD RW pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_RW_PIN",
                       i18n("The RW pin"),
                       5, 0, 7 ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_E_PORT",
                       i18n("The LCD E pin port"), "LCD_PORT"  ) );
    m_settings.append( KLWizardHeaderSetting( i18n("Interface"), "LCD_E_PIN",
                       i18n("The Enable pin"),
                       6, 0, 7 ) );
    
    // If there is a dm_lcd.h file already in the project,
    // read the values from there:
    if ( m_header )
    {
        m_settings.readValuesFromDocument();
    }
    updateLvValuesFrom( m_settings );
    //ui->lvValues->setCurrentItem( ui->lvValues->firstChild() );
    ui->tlValue->hide();
    ui->leValue->hide();
    ui->sbValue->hide();
    ui->cbValue->hide();
    if ( m_header )
    {
        // If there is a header, make the checkbox checked:
        ui->cbAddLibrary->setChecked( true );
    }
    else
    {
        ui->cbAddLibrary->setChecked( false );
    }
}

void KLDotMatrixWidget::slotOK()
{
    bool writeBackChanges = true;
    // First, check if we need to add the dm_lcd files:
    if ( (!m_header) && (!m_source) && ui->cbAddLibrary->isChecked() )
    {
        // Add the files!
        // Locate the right resource:
        KStandardDirs *dirs = KGlobal::dirs();
        QString resPathH = dirs->findResource( "data",
            QString("kontrollerlab/templates/%1").arg( WIZARD_HEADER_FILE ) );
        QString resPathC = dirs->findResource( "data",
            QString("kontrollerlab/templates/%1").arg( WIZARD_SOURCE_FILE ) );
        // Open it:
        if ( resPathH.isNull() || resPathH.isEmpty() )
        {
            qWarning( "Could not locate %s", WIZARD_HEADER_FILE );
            return;
        }
        if ( resPathC.isNull() || resPathC.isEmpty() )
        {
            qWarning( "Could not locate %s", WIZARD_SOURCE_FILE );
            return;
        }
        m_header = new KLDocument(m_parent);
        m_header->open( KUrl( resPathH ) );
        m_project->addDocument( m_header, true );
        KUrl newFileURL = m_project->projectBaseURL();
        newFileURL.addPath( WIZARD_HEADER_FILE );
        m_header->saveAs( newFileURL );
        m_source = new KLDocument(m_parent);
        m_source->open( KUrl( resPathC ) );
        m_project->addDocument( m_source, true );
        newFileURL = m_project->projectBaseURL();
        newFileURL.addPath( WIZARD_SOURCE_FILE );
        m_source->saveAs( newFileURL );
        writeBackChanges = true;
    }
    else if ( m_header && m_source && (!ui->cbAddLibrary->isChecked()) )
    {
        // Ask if it should really be removed:
        int retVal = KMessageBox::questionYesNo( this, i18n( "You have selected to remove the "
                "HD44780 library from the project. Do you really want "
                "to remove %1 and %2 from the project? All unsaved settings will be lost." )
                    .arg( WIZARD_HEADER_FILE )
                    .arg( WIZARD_SOURCE_FILE ) );
        if ( retVal == KMessageBox::Yes )
        {
            retVal = KMessageBox::questionYesNoCancel( this, i18n( "Do you want "
                        "to remove %1 and %2 from disk too?" )
                                .arg( WIZARD_HEADER_FILE )
                                .arg( WIZARD_SOURCE_FILE ) );
            if ( retVal == KMessageBox::Yes )
            {
                QFile fileH( m_header->url().path() );
                fileH.remove();
                QFile fileC( m_source->url().path() );
                fileC.remove();

                m_project->removeDocument( m_header );
                m_project->removeDocument( m_source );
            }
            else if ( retVal == KMessageBox::No )
            {
                m_project->removeDocument( m_header );
                m_project->removeDocument( m_source );
            }
            else
            {
                // Don't do anything
                return;
            }
        }
        else
        {
            // Don't do anything.
            return;
        }
        writeBackChanges = false;
    }
    if ( writeBackChanges )
    {
        if ( m_header )
        {
            m_settings.setDocument( m_header );
            m_header->makeLastActiveViewVisible();
            m_settings.writeValuesToDocument();
            m_header->save();
        }
        else
        {
            qWarning( "No header document in project." );
            close();
        }
    }
    close();
}


void KLDotMatrixWidget::slotAddHD44780Lib(bool val)
{
    ui->lvValues->setEnabled( val );
    ui->tlValue->setEnabled( val );
    ui->leValue->setEnabled( val );
    ui->sbValue->setEnabled( val );
    ui->cbValue->setEnabled( val );
}


void KLDotMatrixWidget::slotChangeValue(int val)
{
    KLWizardHeaderSetting set = m_settings.getSettingFor( ui->lvValues->text() );

    if (set.type() == KLWizardHeaderSetting::NoType)
        return;
    else if (set.type() == KLWizardHeaderSetting::IntType)
    {
        set.setIntValue( val );
        m_settings.setSetting( set );
    }
    updateListItem( set );
}


void KLDotMatrixWidget::slotChangeValue(const QString& val)
{
    KLWizardHeaderSetting set = m_settings.getSettingFor( ui->lvValues->text() );

    if (set.type() == KLWizardHeaderSetting::NoType)
        return;
    else if (set.type() == KLWizardHeaderSetting::StringType)
    {
        set.setStrValue( val );
        m_settings.setSetting( set );
    }
    updateListItem( set );
}

void KLDotMatrixWidget::slotCancel()
{
    close();
}


void KLDotMatrixWidget::slotSelectedValueChanged(Q3ListViewItem *item)
{
    KLWizardHeaderSetting set = m_settings.getSettingFor( item->text( 0 ) );
    
    Q3DictIterator< Q3ListViewItem > it( m_groups );
    
    for ( ; it.current(); ++it )
    {
        if ( it.current() == item )
        {
            ui->tlValue->hide();
            ui->leValue->hide();
            ui->sbValue->hide();
            ui->cbValue->hide();
            return;
        }
    }
    ui->tlValue->show();
    
    if (set.type() == KLWizardHeaderSetting::StringType)
    {
        ui->tlValue->setText( set.defName() );
        ui->leValue->setText( set.stringValue() );
        ui->leValue->show();
        ui->sbValue->hide();
        ui->cbValue->hide();
    }
    else if (set.type() == KLWizardHeaderSetting::IntType)
    {
        ui->tlValue->setText( set.defName() );
        ui->leValue->hide();
        ui->sbValue->show();
        ui->sbValue->setMinValue( set.min() );
        ui->sbValue->setMaxValue( set.max() );
        ui->sbValue->setValue( set.intValue() );
        //ui->sbValue->setBase( set.base() );
        if ( set.base() == 16 )
        {
            ui->sbValue->setPrefix("0x");
        }
        else
        {
            ui->sbValue->setPrefix("");
        }
        ui->cbValue->hide();
    }
}


void KLDotMatrixWidget::updateLvValuesFrom( const KLWizardHeaderSettings & set_ )
{
    KLWizardHeaderSettings set = set_;
    
    QList< KLWizardHeaderSetting >::iterator it;
    ui->lvValues->clear();
    // lvValues->setSorting( 0 );
    Q3ListViewItem* currentGrp=0L;
    
    for ( it = set.begin(); it != set.end(); ++it )
    {
        if ( m_groups[ (*it).groupName() ] )
        {
            currentGrp = m_groups[ (*it).groupName() ];
        }
        else
        {
            //currentGrp = new Q3ListViewItem(ui->lvValues, (*it).groupName());
            //currentGrp->setOpen( true );
            //m_groups.insert( (*it).groupName(), currentGrp );
        }

        if ( (*it).type() == KLWizardHeaderSetting::StringType )
        {
            new Q3ListViewItem(currentGrp, (*it).defName(),
                                  (*it).stringValue(),
                                  (*it).description() );
        }
        else if ( (*it).type() == KLWizardHeaderSetting::IntType )
        {
            if ( (*it).base() == 16 )
            {
                new Q3ListViewItem(currentGrp, (*it).defName(),
                                    "0x" + QString::number( (*it).intValue(),
                                            (*it).base() ),
                                    (*it).description() );
            }
            else
            {
                new Q3ListViewItem(currentGrp, (*it).defName(),
                                  QString::number( (*it).intValue(),
                                          (*it).base() ),
                                  (*it).description() );
            }
        }
        else if ( (*it).type() == KLWizardHeaderSetting::StringListType )
        {
            new Q3ListViewItem(currentGrp, (*it).defName(),
                                  (*it).stringValue(),
                                  (*it).description() );
        }
    }
}


void KLDotMatrixWidget::updateListItem( const KLWizardHeaderSetting & set )
{
    
    Q3DictIterator< Q3ListViewItem > itGrp( m_groups );
    
    for ( ; itGrp.current(); ++itGrp )
    {
        Q3ListViewItem *it = itGrp.current()->firstChild();
        
        while (it)
        {
            if ( it->text(0) == set.defName() )
                break;
            it = it->nextSibling();
        }
        if ( it )
        {
            if ( set.type() == KLWizardHeaderSetting::IntType )
            {
                if ( set.base() == 16 )
                    it->setText( 1, "0x" + QString::number( set.intValue(), 16 ) );
                else
                    it->setText( 1, QString::number( set.intValue(), set.base() ) );
            }
            else if ( set.type() == KLWizardHeaderSetting::StringType )
            {
                it->setText( 1, set.strValue() );
            }
        }
    }
}

