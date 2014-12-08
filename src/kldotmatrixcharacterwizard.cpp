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
#include "kldotmatrixcharacterwizard.h"
#include "klproject.h"
#include "kldocument.h"
#include "kleditdotswidget.h"
#include <qlayout.h>
#include <qradiobutton.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qlineedit.h>
#include "kldocumentview.h"
#include <ktexteditor/cursor.h>
        
#define WIZARD_START "// WIZARD CODE"
#define WIZARD_END   "// END OF WIZARD CODE"

KLDotMatrixCharacterWizard::KLDotMatrixCharacterWizard(QWidget *parent, const char *name,
                KLProject* project, KLDocument* doc)
    :QDialog(parent, name),
      ui(new Ui::KLDotMatrixCharacterWizardBase)
{
    ui->setupUi(this);
    m_project = project;
    m_document = doc;
    
    // For the character generator wizard:
    m_editDots = new KLEditDotsWidget( 5, 8, ui->frmCharacter );
    m_frmGridLayout = new QGridLayout(ui->frmCharacter, 1, 1, 5);
    m_editDots->setBackgroundColor( QColor( 50, 200, 50 ) );
    m_editDots->setPaletteForegroundColor( QColor( 0, 20, 0 ) );
    m_frmGridLayout->addWidget( m_editDots, 0, 0 );
    connect( m_editDots, SIGNAL(dotsChanged()), this, SLOT(slotCharacterChanged()) );
    
    // Here starts the reread code:
    m_whiteSpace = "";
    
    m_startLine = m_stopLine = -1;
    // Now we search in the document for the start and the stop, if any:
    if ( m_document->lastActiveView() )
    {
        if ( m_document->lastActiveView()->view() )
        {
            int curLine = m_document->lastActiveView()->view()->cursor().pos().x();
            m_stopLine = m_startLine = curLine;
            bool foundStart = false, foundStop = false;
            while ( m_startLine >= 0 )
            {
                if ( m_startLine != curLine )
                    if (m_document->kateDoc()->line( m_startLine ).upper().stripWhiteSpace().startsWith( WIZARD_END ))
                        break;
                if (!m_document->kateDoc()->line( m_startLine ).upper().stripWhiteSpace().startsWith( WIZARD_START ))
                    m_startLine--;
                else
                {
                    foundStart = true;
                    break;
                }
            }
            while ( m_stopLine < (int) m_document->kateDoc()->lines() )
            {
                if ( m_stopLine != curLine )
                    if (m_document->kateDoc()->line( m_stopLine ).upper().stripWhiteSpace().startsWith( WIZARD_START ))
                        break;
                if (!m_document->kateDoc()->line( m_stopLine ).upper().stripWhiteSpace().startsWith( WIZARD_END ))
                    m_stopLine++;
                else
                {
                    foundStop = true;
                    break;
                }
            }
            if (!( foundStart && foundStop ))
                m_startLine = m_stopLine = -1;
        }
    }

    if ( underReedit() )
    {
        // Seek the line with the // WIZARD SEVEN_SEGMENT_WIZARD in it.
        int curLine = m_startLine;
        bool ok;
        while ( curLine < m_stopLine )
        {
            curLine++;
            if ( m_document->kateDoc()->line( curLine ).upper().stripWhiteSpace().startsWith( "// WIZARD DOT_MATRIX_CHARACTER_WIZARD" ) )
            {
                QString charSizeString = m_document->kateDoc()->line( curLine ).upper().stripWhiteSpace();
                QStringList slist = QStringList::split( " ", charSizeString );
                charSizeString = slist[3];
                if ( charSizeString.upper() == "5X8" )
                    ui->rb5x8->setChecked( true );
                else
                    ui->rb5x10->setChecked( true );
                break;
            }
        }
        curLine++;
        QString allLinesBelow;
        while (curLine < m_stopLine)
        {
            int ws = 0;
            QString current = m_document->kateDoc()->line( curLine );
            if ( current.contains( "=" ) )
            {
                ws = current.left( current.find("=")+1 ).length() -
                        current.left( current.find("=")+1 ).stripWhiteSpace().length();
                m_whiteSpace = current.left( ws );
            }
            allLinesBelow += current.stripWhiteSpace();
            curLine++;
        }
        QString theName = allLinesBelow.mid( 0, allLinesBelow.find( "=" ) ).stripWhiteSpace();
        QString theType = theName.left( theName.findRev( " " ) ).stripWhiteSpace();
        ui->leVarType->setText( theType );
        theName = theName.right( theName.length() - theName.findRev( " " ) );
        theName = theName.replace("[", "").replace("]", "");
        ui->leVarName->setText( theName.stripWhiteSpace() );
        allLinesBelow = allLinesBelow.mid( allLinesBelow.find( "{" )+1,
                                           allLinesBelow.findRev( "}" ) - allLinesBelow.find( "{" ) - 1 );
        allLinesBelow = allLinesBelow.replace( ",", " " );
        QStringList list = QStringList::split(" ", allLinesBelow, false);
        QList< int > masks;
        for ( QStringList::Iterator it=list.begin(); it != list.end(); ++it )
        {
            if ( (*it).upper().startsWith("0X") )
            {
                int addMe = (*it).right((*it).length()-2).toInt(&ok, 16);
                if (!ok)
                    break;
                masks.append( addMe );
            }
            else
            {
                int addMe = (*it).toInt(&ok);
                if (!ok)
                    break;
                masks.append( addMe );
            }
        }
        if ( masks.size() > 0 )
        {
            ui->lvCharacters->clear();
            KLDotMatrixCharacterListViewItem *item;
            int size=8;
            if ( ui->rb5x10->isChecked() )
                size=10;
            QList< int > oneChar;
            for ( unsigned int i=0; i<masks.size(); i++ )
            {
                oneChar.append( masks[i] );
                if ( (i+1) % size == 0 )
                {
                    item = new KLDotMatrixCharacterListViewItem( i/size, oneChar, ui->lvCharacters, "dmChar" );
                    oneChar.clear();
                }
            }
            ui->lvCharacters->setCurrentItem( ui->lvCharacters->firstChild() );
            slotSelectedCharacterChanged( ui->lvCharacters->firstChild() );
        }
    }
    else
    {
       // new KLDotMatrixCharacterListViewItem( 0, m_editDots->get8BitMask(), lvCharacters, "dmChar" );
    }

}


void KLDotMatrixCharacterWizard::slotCharacterChanged()
{
    KLDotMatrixCharacterListViewItem* lviSel =
            static_cast< KLDotMatrixCharacterListViewItem* > (ui->lvCharacters->currentItem());
    if (lviSel)
    {
        if (lviSel->masks() != m_editDots->get8BitMask() )
            lviSel->update( m_editDots->get8BitMask() );
    }
}


void KLDotMatrixCharacterWizard::slotOK()
{
    QList< int > bitmasks;
    
    KLDotMatrixCharacterListViewItem* it =
        static_cast< KLDotMatrixCharacterListViewItem* > (ui->lvCharacters->firstChild());
    while ( it )
    {
        QList< int > curMasks = it->masks();
        for ( QList< int >::iterator itM=curMasks.begin(); itM != curMasks.end(); ++itM )
            bitmasks.append( *itM );
        it = it->nextSibling();
    }
    
    QString varDef;
    QString lineStart;
    QString out(WIZARD_START);
    out += " Do not edit the lines below.\n";
    out += "// Otherwise the wizard may be unable to reread the settings.\n";
    out += "// WIZARD DOT_MATRIX_CHARACTER_WIZARD ";
    
    if ( ui->rb5x8->isChecked() )
        out += "5x8";
    else
        out += "5x10";
    
    out += "\n";
    varDef = ui->leVarType->text() + " " + ui->leVarName->text() + "[] = {";
    lineStart = QString("%1").arg( " ", varDef.length() );
    out += varDef;
    int counter=0;
    for (unsigned int i=0; i<bitmasks.count(); i++)
    {
        out += " " + QString("0x%1").arg(bitmasks[i], 0, 16) + ",";
        counter++;
        if (counter > 6)
        {
            counter=0;
            out += "\n"+lineStart;
        }
    }
    out = out.left( out.length()-1 );
    out += " };\n";
    out += WIZARD_END;
    out += "\n";
    int line=0;
    if (m_document->lastActiveView())
        if (m_document->lastActiveView()->view())
            line=m_document->lastActiveView()->view()->cursorPosition().line();
    QStringList lines;
    lines = lines.split( "\n", out );
    if ( underReedit() )
    {
        for ( int curLine=m_stopLine; curLine >= m_startLine; curLine-- )
            m_document->kateDoc()->removeLine( curLine );
        line = m_startLine;
    }
    for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it )
    {
        m_document->kateDoc()->insertLine( line, m_whiteSpace + (*it) );
        line++;
    }
    close();
}


void KLDotMatrixCharacterWizard::slotSizeChanged()
{
    if (ui->rb5x8->isChecked())
    {
        m_editDots->resizeDots( 5, 8 );
    }
    else
    {
        m_editDots->resizeDots( 5, 10 );
    }
}


void KLDotMatrixCharacterWizard::slotSelectedCharacterChanged(Q3ListViewItem *item )
{
    if (!item)
        return;
    KLDotMatrixCharacterListViewItem* lviSel =
        static_cast< KLDotMatrixCharacterListViewItem* > (item);
    if (lviSel)
    {
        if (lviSel->masks() != m_editDots->get8BitMask() )
            m_editDots->setFrom8BitMask( lviSel->masks() );
    }
}


void KLDotMatrixCharacterWizard::slotDown()
{
    KLDotMatrixCharacterListViewItem* lviSel =
        static_cast< KLDotMatrixCharacterListViewItem* > (ui->lvCharacters->currentItem());
    KLDotMatrixCharacterListViewItem* next =
        static_cast< KLDotMatrixCharacterListViewItem* > (lviSel->nextSibling());
    
    if (lviSel && next)
    {
        int buf = lviSel->number();
        ui->lvCharacters->takeItem( lviSel );
        ui->lvCharacters->takeItem( next );
        lviSel->setNumber( next->number() );
        next->setNumber( buf );
        ui->lvCharacters->insertItem( lviSel );
        ui->lvCharacters->insertItem( next );
        ui->lvCharacters->setCurrentItem( lviSel );
    }
}


void KLDotMatrixCharacterWizard::slotUp()
{
    KLDotMatrixCharacterListViewItem* lviSel =
        static_cast< KLDotMatrixCharacterListViewItem* > (ui->lvCharacters->currentItem());
    KLDotMatrixCharacterListViewItem* prev = 
        static_cast< KLDotMatrixCharacterListViewItem* > (ui->lvCharacters->firstChild());
    
    if ( prev == lviSel )
        return;
    while ( prev->nextSibling() != lviSel )
    {
        prev = static_cast< KLDotMatrixCharacterListViewItem* > (prev->nextSibling());
    }
    
    if (lviSel && prev)
    {
        int buf = lviSel->number();
        ui->lvCharacters->takeItem( lviSel );
        ui->lvCharacters->takeItem( prev );
        lviSel->setNumber( prev->number() );
        prev->setNumber( buf );
        ui->lvCharacters->insertItem( lviSel );
        ui->lvCharacters->insertItem( prev );
        ui->lvCharacters->setCurrentItem( lviSel );
    }
}


void KLDotMatrixCharacterWizard::slotRemove()
{
    if (ui->lvCharacters->childCount() == 1)
        return;
    ui->lvCharacters->takeItem( ui->lvCharacters->currentItem() );
    ui->lvCharacters->setSelected( ui->lvCharacters->currentItem(), true );
    reNumber();
}


void KLDotMatrixCharacterWizard::slotAdd()
{
    KLDotMatrixCharacterListViewItem * newIt;

    newIt = new KLDotMatrixCharacterListViewItem( ui->lvCharacters->childCount(), m_editDots->get8BitMask(),
            ui->lvCharacters, "dmChar" );
    ui->lvCharacters->setCurrentItem( newIt );
    m_editDots->clear();
}

void KLDotMatrixCharacterWizard::slotCancel()
{
    close();
}

KLDotMatrixCharacterListViewItem::KLDotMatrixCharacterListViewItem(int number, QList<int> masks,
        Q3ListView *parent, const char * ) : Q3ListViewItem( parent )
{
    m_masks = masks;
    setNumber( number );
    setPixmap( 0, generatePixmapFor( masks ) );
}

QPixmap KLDotMatrixCharacterListViewItem::generatePixmapFor( QList< int > masks )
{
    QPixmap retVal(10, masks.count()*2);
    QPainter paint( &retVal );
    retVal.fill( listView()->backgroundColor() );
    QColor fg = listView()->foregroundColor();
    QList< int >::iterator it;
    int y = 0;
    paint.setPen( fg );
    for ( it = masks.begin(); it != masks.end(); ++it )
    {
        for (int x=0; x<5; x++)
        {
            if ( (*it) & (1<<(4-x)) )
            {
                paint.drawRect(x*2,y*2,2,2);
            }
        }
        y++;
    }
    return retVal;
}

void KLDotMatrixCharacterListViewItem::update(QList<int> masks )
{
    m_masks = masks;
    setPixmap( 0, generatePixmapFor( masks ) );
}

void KLDotMatrixCharacterListViewItem::updateNumberString( )
{
    QString displayNumber = QString::number( m_number, 16 );
    if (displayNumber.length() < 2)
        displayNumber = "0" + displayNumber;
    displayNumber = "0x" + displayNumber;
    setText( 0, displayNumber );
}

void KLDotMatrixCharacterWizard::reNumber( )
{
    KLDotMatrixCharacterListViewItem* it = static_cast< KLDotMatrixCharacterListViewItem* >
            (ui->lvCharacters->firstChild());
    KLDotMatrixCharacterListViewItem* oldIt;
    int inc = it->number() > 0 ? -1: 1;
    int i = it->number() > 0 ?ui->lvCharacters->childCount() -1: 0;
    while (it)
    {
        it->setNumber( i );
        i += inc;
        oldIt = it;
        it = it->nextSibling();
    }
    ui->lvCharacters->sort();
}

KLDotMatrixCharacterListViewItem * KLDotMatrixCharacterListViewItem::nextSibling( )
{
    return static_cast< KLDotMatrixCharacterListViewItem* >( Q3ListViewItem::nextSibling() );
}

