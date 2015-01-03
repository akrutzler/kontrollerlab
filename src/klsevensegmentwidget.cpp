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
#include "klsevensegmentwidget.h"
#include <qtoolbutton.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include "kldocument.h"
#include "kldocumentview.h"

#define WIZARD_START "// WIZARD CODE"
#define WIZARD_END   "// END OF WIZARD CODE"

KLSevenSegmentWidget::KLSevenSegmentWidget(QWidget *parent, const char *name, KLDocument* doc)
    :QDialog(parent, name), ui(new Ui_KLSevenSegmentWidgetBase)
{
    ui->setupUi(this);

    fillSegmentList();

    QList< QRadioButton*> helper;
    helper.append( ui->rbA0 );
    helper.append( ui->rbA1 );
    helper.append( ui->rbA2 );
    helper.append( ui->rbA3 );
    helper.append( ui->rbA4 );
    helper.append( ui->rbA5 );
    helper.append( ui->rbA6 );
    helper.append( ui->rbA7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbB0 );
    helper.append( ui->rbB1 );
    helper.append( ui->rbB2 );
    helper.append( ui->rbB3 );
    helper.append( ui->rbB4 );
    helper.append( ui->rbB5 );
    helper.append( ui->rbB6 );
    helper.append( ui->rbB7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbC0 );
    helper.append( ui->rbC1 );
    helper.append( ui->rbC2 );
    helper.append( ui->rbC3 );
    helper.append( ui->rbC4 );
    helper.append( ui->rbC5 );
    helper.append( ui->rbC6 );
    helper.append( ui->rbC7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbD0 );
    helper.append( ui->rbD1 );
    helper.append( ui->rbD2 );
    helper.append( ui->rbD3 );
    helper.append( ui->rbD4 );
    helper.append( ui->rbD5 );
    helper.append( ui->rbD6 );
    helper.append( ui->rbD7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbE0 );
    helper.append( ui->rbE1 );
    helper.append( ui->rbE2 );
    helper.append( ui->rbE3 );
    helper.append( ui->rbE4 );
    helper.append( ui->rbE5 );
    helper.append( ui->rbE6 );
    helper.append( ui->rbE7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbF0 );
    helper.append( ui->rbF1 );
    helper.append( ui->rbF2 );
    helper.append( ui->rbF3 );
    helper.append( ui->rbF4 );
    helper.append( ui->rbF5 );
    helper.append( ui->rbF6 );
    helper.append( ui->rbF7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbG0 );
    helper.append( ui->rbG1 );
    helper.append( ui->rbG2 );
    helper.append( ui->rbG3 );
    helper.append( ui->rbG4 );
    helper.append( ui->rbG5 );
    helper.append( ui->rbG6 );
    helper.append( ui->rbG7 );
    m_radios.append( helper );
    helper.clear();
    helper.append( ui->rbDP0 );
    helper.append( ui->rbDP1 );
    helper.append( ui->rbDP2 );
    helper.append( ui->rbDP3 );
    helper.append( ui->rbDP4 );
    helper.append( ui->rbDP5 );
    helper.append( ui->rbDP6 );
    helper.append( ui->rbDP7 );
    m_radios.append( helper );
    helper.clear();
    m_document = doc;
    m_dontReactOnToolButtons=false;
    // Here starts the reread code:
    m_whiteSpace = "";
    
    m_startLine = m_stopLine = -1;
    // Now we search in the document for the start and the stop, if any:
    if ( m_document->lastActiveView() )
    {
        if ( m_document->lastActiveView()->view() )
        {
            int curLine = m_document->lastActiveView()->view()->cursorPosition().line();
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
    
    QRadioButton *bit;
    QList< QRadioButton*> seg;
    for ( unsigned int i=0; i<m_radios.count(); i++ )
    {
        seg=m_radios[i];
        foreach ( bit,seg )
        {
            connect( bit, SIGNAL(toggled( bool )), this, SLOT(slotBitSegmentAssocChanged()) );
        }
    }
    // Now set the bit to segment assignment list.
    // The meaning is: m_segmentIsBit[2] gives the bit mask (not bit number) for segment C
    
    m_segmentIsBit.append(0x1);
    m_segmentIsBit.append(0x2);
    m_segmentIsBit.append(0x4);
    m_segmentIsBit.append(0x8);
    m_segmentIsBit.append(0x10);
    m_segmentIsBit.append(0x20);
    m_segmentIsBit.append(0x40);
    m_segmentIsBit.append(0x80);
    
    if ( underReedit() )
    {
        // Seek the line with the // WIZARD SEVEN_SEGMENT_WIZARD in it.
        int curLine = m_startLine;
        bool ok;
        while ( curLine < m_stopLine )
        {
            curLine++;
            if ( m_document->kateDoc()->line( curLine ).upper().stripWhiteSpace().startsWith( "// WIZARD SEVEN_SEGMENT_WIZARD" ) )
            {
                QString bitAssignmentString = m_document->kateDoc()->line( curLine ).upper().stripWhiteSpace();
                QStringList slist = QStringList::split( " ", bitAssignmentString );
                bitAssignmentString = slist[3];
                for (int i=0; i<8; i++)
                {
                    int segmentNr = bitAssignmentString.mid(i,1).toInt( &ok );
                    if (!ok)
                        break;
                    m_segmentIsBit[segmentNr] = 1<<i;
                }
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
        theName = theName.right( theName.length() - theName.findRev( " " ) );
        theName = theName.replace("[", "").replace("]", "");
        ui->leArrayName->setText( theName.stripWhiteSpace() );
        allLinesBelow = allLinesBelow.mid( allLinesBelow.find( "{" )+1,
                                           allLinesBelow.findRev( "}" ) - allLinesBelow.find( "{" ) - 1 );
        allLinesBelow = allLinesBelow.replace( ",", " " );
        QStringList list = QStringList::split(" ", allLinesBelow, false);
        QList< int > segs, sortedSegs;
        for ( QStringList::Iterator it=list.begin(); it != list.end(); ++it )
        {
            if ( (*it).upper().startsWith("0X") )
            {
                int addMe = (*it).right((*it).length()-2).toInt(&ok, 16);
                if (!ok)
                    break;
                segs.append( addMe );
            }
            else
            {
                int addMe = (*it).toInt(&ok);
                if (!ok)
                    break;
                segs.append( addMe );
            }
        }
        for ( unsigned int i=0; i<segs.size(); i++ )
        {
            int addMe = 0;
            for ( int j=0; j<8; j++ )
            {
                if ( segs[i] & m_segmentIsBit[j] )
                    addMe |= 1<<j;
            }
            sortedSegs.append( addMe );
        }
        if ( sortedSegs.size() > 0 )
        {
            ui->lvSegments->clear();
            Q3ListViewItem *item;
            for ( unsigned int i=0; i<sortedSegs.size(); i++ )
            {
                item = new Q3ListViewItem(ui->lvSegments, formStr( i ) );
                for ( int j=0; j<8; j++ )
                    item->setText( j+1, (1<<j)&sortedSegs[i] ? "1": "0" );
                item->setPixmap( 0, generatePixmapFor( getSegmentsForListItem( item ) ) );
                ui->lvSegments->insertItem( item );
            }
        }
    }

    m_prohibitRecursion=true;
    setRadiosFromBitSegmentAssignment( m_segmentIsBit );
    m_prohibitRecursion=false;
    ui->lvSegments->setSelected( ui->lvSegments->firstChild(), true );
    slotCurrentItemChanged( ui->lvSegments->firstChild() );
    
    Q3ListViewItem * myChild = ui->lvSegments->firstChild();
    while( myChild ) {
        myChild->setPixmap( 0, generatePixmapFor( getSegmentsForListItem( myChild ) ) );
        myChild = myChild->nextSibling();
    }
    ui->lvSegments->setSorting(0);
}

void KLSevenSegmentWidget::slotBitSegmentAssocChanged()
{
    if ( m_prohibitRecursion )
        return;
    m_prohibitRecursion=true;
    QList< int > newMask;

    newMask = readBitSegmentAssignmentFromRadios();
    // Find out the row which has changed:
    int changedRow=-1;
    
    for ( unsigned int i=0; i<newMask.count(); i++ )
    {
        if ( newMask[i] != m_segmentIsBit[i] )
        {
            changedRow=i;
            break;
        }
    }
    if ( changedRow<0 )
    {
        qWarning("ERROR: changedRow<0 %d", changedRow);
    }
    
    // Check if the user deactivated the previously checked radio button:
    if ( newMask[changedRow] == 0 )
    {
        // Set the old mask and exit:
        setRadiosFromBitSegmentAssignment( m_segmentIsBit );
        m_prohibitRecursion = false;
        return;
    }
    // Find out, how the mask should be set according to the user:
    int maskShouldBe = newMask[changedRow] & ( newMask[changedRow] ^ m_segmentIsBit[changedRow] );
    // Check which mask has to be changed now!
    int neccessarilyChangeRow = -1;
    for ( unsigned int i=0; i<newMask.count(); i++ )
    {
        if ( m_segmentIsBit[i] == maskShouldBe )
        {
            neccessarilyChangeRow = i;
            break;
        }
    }
    if ( neccessarilyChangeRow<0 )
    {
        qWarning("ERROR: neccessarilyChangeRow<0 %d", neccessarilyChangeRow);
    }
    // Set the neccessarily changed row to the mask which was set in the changedRow before:
    newMask[neccessarilyChangeRow] = m_segmentIsBit[changedRow];
    // Then set the new mask in the changedRow:
    newMask[changedRow] = maskShouldBe;
    setRadiosFromBitSegmentAssignment( newMask );
    // These should stay the last 2 assignments:
    m_prohibitRecursion = false;
    m_segmentIsBit = readBitSegmentAssignmentFromRadios();
}


void KLSevenSegmentWidget::slotDP()
{
    ui->tbDP->setPalette( ui->tbDP->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotG()
{
    ui->tbG->setPalette( ui->tbG->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotF()
{
    ui->tbF->setPalette( ui->tbF->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotE()
{
    ui->tbE->setPalette( ui->tbE->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotD()
{
    ui->tbD->setPalette( ui->tbD->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotC()
{
    ui->tbC->setPalette( ui->tbC->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotB()
{
    ui->tbB->setPalette( ui->tbB->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotA()
{
    ui->tbA->setPalette( ui->tbA->isOn() ? QPalette( Qt::red, Qt::red ) : palette() );
    changeCurrentListItem();
}


void KLSevenSegmentWidget::slotCancel()
{
    close();
}


void KLSevenSegmentWidget::slotOK()
{
    QList< int > bitmasks = generateBitmasks();
    QString varDef;
    QString lineStart;
    QString out(WIZARD_START);
    out += " Do not edit the lines below.\n";
    out += "// Otherwise the wizard may be unable to reread the settings.\n";
    out += "// WIZARD SEVEN_SEGMENT_WIZARD ";
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            if (m_segmentIsBit[j] == (1<<i))
                out += QString("%1").arg(j);
        }
    }
    out += "\n";
    varDef = "char " + ui->leArrayName->text() + "[] = {";
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


void KLSevenSegmentWidget::slotDown()
{
    Q3ListViewItem *next,*item=ui->lvSegments->currentItem();
    bool ok;
    
    next = item->nextSibling();
    if (!next)
        return;
    ui->lvSegments->takeItem( next );
    ui->lvSegments->takeItem( item );
    item->setText( 0, formStr( item->text( 0 ).mid(2,2).toInt( &ok, 16 )+1) );
    next->setText( 0, formStr( next->text( 0 ).mid(2,2).toInt( &ok, 16 )-1) );
    ui->lvSegments->insertItem( next );
    ui->lvSegments->insertItem( item );
    ui->lvSegments->setCurrentItem( item );
}


void KLSevenSegmentWidget::slotUp()
{
    Q3ListViewItem *prev,*item=ui->lvSegments->currentItem();
    bool ok;
    
    prev=ui->lvSegments->firstChild();
    if ( prev == item )
        return;
    while(prev->nextSibling()!=item)
        prev=prev->nextSibling();
    ui->lvSegments->takeItem( prev );
    ui->lvSegments->takeItem( item );
    item->setText( 0, formStr( item->text( 0 ).mid(2,2).toInt( &ok, 16 )-1 ) );
    prev->setText( 0, formStr( prev->text( 0 ).mid(2,2).toInt( &ok, 16 )+1 ) );
    ui->lvSegments->insertItem( prev );
    ui->lvSegments->insertItem( item );
    ui->lvSegments->setCurrentItem( item );
}


void KLSevenSegmentWidget::slotRemove()
{
    if (ui->lvSegments->childCount() == 1)
        return;
    ui->lvSegments->takeItem( ui->lvSegments->currentItem() );
    ui->lvSegments->setSelected( ui->lvSegments->currentItem(), true );
}


void KLSevenSegmentWidget::slotAdd()
{
    Q3ListViewItem *item=ui->lvSegments->currentItem();
    if (ui->lvSegments->childCount() > 255)
        return;
    while(item->nextSibling())
        item=item->nextSibling();
    item = new Q3ListViewItem(ui->lvSegments, formStr( ui->lvSegments->childCount() ), "0", "0", "0", "0", "0", "0", "0");
    item->setText( 9, "0" );
    item->setPixmap( 0, generatePixmapFor( getSegmentsForListItem( item ) ) );
    ui->lvSegments->insertItem( item );
    ui->lvSegments->setSelected( item, true );
}

void KLSevenSegmentWidget::fillSegmentList()
{
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "Item", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "A", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "B", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "C", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "D", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "E", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "F", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "G", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->addColumn(QApplication::translate("KLSevenSegmentWidgetBase", "DP", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setClickEnabled(false, ui->lvSegments->header()->count() - 1);
    ui->lvSegments->header()->setResizeEnabled(true, ui->lvSegments->header()->count() - 1);

    ui->lvSegments->header()->setLabel(0, QApplication::translate("KLSevenSegmentWidgetBase", "Item", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(1, QApplication::translate("KLSevenSegmentWidgetBase", "A", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(2, QApplication::translate("KLSevenSegmentWidgetBase", "B", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(3, QApplication::translate("KLSevenSegmentWidgetBase", "C", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(4, QApplication::translate("KLSevenSegmentWidgetBase", "D", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(5, QApplication::translate("KLSevenSegmentWidgetBase", "E", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(6, QApplication::translate("KLSevenSegmentWidgetBase", "F", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(7, QApplication::translate("KLSevenSegmentWidgetBase", "G", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->header()->setLabel(8, QApplication::translate("KLSevenSegmentWidgetBase", "DP", 0, QApplication::UnicodeUTF8));
    ui->lvSegments->clear();

    Q3ListViewItem *__item = new Q3ListViewItem(ui->lvSegments);
    __item->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x00", 0, QApplication::UnicodeUTF8));
    __item->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item1 = new Q3ListViewItem(ui->lvSegments);
    __item1->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x01", 0, QApplication::UnicodeUTF8));
    __item1->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item1->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item1->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item1->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item1->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item1->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item1->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item1->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item2 = new Q3ListViewItem(ui->lvSegments);
    __item2->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x02", 0, QApplication::UnicodeUTF8));
    __item2->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item2->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item2->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item2->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item2->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item2->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item2->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item2->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item3 = new Q3ListViewItem(ui->lvSegments);
    __item3->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x03", 0, QApplication::UnicodeUTF8));
    __item3->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item3->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item3->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item3->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item3->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item3->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item3->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item3->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item4 = new Q3ListViewItem(ui->lvSegments);
    __item4->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x04", 0, QApplication::UnicodeUTF8));
    __item4->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item4->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item4->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item4->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item4->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item4->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item4->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item4->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item5 = new Q3ListViewItem(ui->lvSegments);
    __item5->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x05", 0, QApplication::UnicodeUTF8));
    __item5->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item5->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item5->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item5->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item5->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item5->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item5->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item5->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item6 = new Q3ListViewItem(ui->lvSegments);
    __item6->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x06", 0, QApplication::UnicodeUTF8));
    __item6->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item6->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item6->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item6->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item6->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item6->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item6->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item6->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item7 = new Q3ListViewItem(ui->lvSegments);
    __item7->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x07", 0, QApplication::UnicodeUTF8));
    __item7->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item7->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item7->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item7->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item7->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item7->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item7->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item7->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item8 = new Q3ListViewItem(ui->lvSegments);
    __item8->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x08", 0, QApplication::UnicodeUTF8));
    __item8->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item8->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));

    Q3ListViewItem *__item9 = new Q3ListViewItem(ui->lvSegments);
    __item9->setText(0, QApplication::translate("KLSevenSegmentWidgetBase", "0x09", 0, QApplication::UnicodeUTF8));
    __item9->setText(1, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item9->setText(2, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item9->setText(3, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item9->setText(4, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item9->setText(5, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
    __item9->setText(6, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item9->setText(7, QApplication::translate("KLSevenSegmentWidgetBase", "1", 0, QApplication::UnicodeUTF8));
    __item9->setText(8, QApplication::translate("KLSevenSegmentWidgetBase", "0", 0, QApplication::UnicodeUTF8));
}

void KLSevenSegmentWidget::slotCurrentItemChanged(Q3ListViewItem *cur )
{
    int segs = getSegmentsForListItem( cur );

    m_dontReactOnToolButtons=true;
    ui->tbA->setOn(segs&0x01);
    ui->tbB->setOn(segs&0x02);
    ui->tbC->setOn(segs&0x04);
    ui->tbD->setOn(segs&0x08);
    ui->tbE->setOn(segs&0x10);
    ui->tbF->setOn(segs&0x20);
    ui->tbG->setOn(segs&0x40);
    ui->tbDP->setOn(segs&0x80);
    m_dontReactOnToolButtons=false;
}

QList < int > KLSevenSegmentWidget::readBitSegmentAssignmentFromRadios()
{
    QList < int > retVal;
    QRadioButton *bit;
    QList< QRadioButton*> seg;
    for ( unsigned int i=0; i<m_radios.count(); i++ )
    {
        int j=0;
        int mask=0;
        seg=m_radios[i];
        foreach ( bit, seg )
        {
            if (bit->isChecked())
                mask|=(1<<j);
            j++;
        }
        retVal.append( mask );
    }
    if ( retVal.count() != m_radios.count() )
    {
        qWarning( "retVal.count() %d != m_radios.count() %d", retVal.count(), m_radios.count() );
    }
    return retVal;
}

void KLSevenSegmentWidget::setRadiosFromBitSegmentAssignment(QList<int> assign )
{
    if ( assign.count() != m_radios.count() )
    {
        qWarning( "assign.count() %d != m_radios.count() %d", assign.count(), m_radios.count() );
        return;
    }
    QRadioButton *bit;
    QList< QRadioButton*> seg;
    for ( unsigned int i=0; i<m_radios.count(); i++ )
    {
        int j=0;
        seg=m_radios[i];
        foreach ( bit, seg )
        {
            bit->setChecked( assign[i] & (1<<j) );
            j++;
        }
    }
}

void KLSevenSegmentWidget::changeCurrentListItem()
{
    if (m_dontReactOnToolButtons)
        return;
    int bits = 0;
    if ( ui->tbA->isOn() )
        bits |= 0x01;
    if ( ui->tbB->isOn() )
        bits |= 0x02;
    if ( ui->tbC->isOn() )
        bits |= 0x04;
    if ( ui->tbD->isOn() )
        bits |= 0x08;
    if ( ui->tbE->isOn() )
        bits |= 0x10;
    if ( ui->tbF->isOn() )
        bits |= 0x20;
    if ( ui->tbG->isOn() )
        bits |= 0x40;
    if ( ui->tbDP->isOn() )
        bits |= 0x80;
    Q3ListViewItem *item = ui->lvSegments->currentItem();
    item->setPixmap( 0, generatePixmapFor( bits ) );
    setListItemForSegments( item, bits );
}

QPixmap KLSevenSegmentWidget::generatePixmapFor( int segs )
{
    int vsize=14, hsize=7;
    QPixmap retVal(hsize+2,vsize);
    retVal.fill( ui->lvSegments->backgroundColor() );
    QPainter paint(&retVal);
    paint.setPen( Qt::red );
    if (segs&0x01)
        paint.drawLine( 1, 0, hsize-2, 0 );
    if (segs&0x02)
        paint.drawLine( hsize-1, 1, hsize-1, vsize/2-1 );
    if (segs&0x04)
        paint.drawLine( hsize-1, vsize/2+1, hsize-1, vsize-2 );
    if (segs&0x08)
        paint.drawLine( 1, vsize-1, hsize-2, vsize-1 );
    if (segs&0x10)
        paint.drawLine( 1, vsize/2+1, 1, vsize-2 );
    if (segs&0x20)
        paint.drawLine( 1, 1, 1, vsize/2-1 );
    if (segs&0x40)
        paint.drawLine( 1, vsize/2, hsize-2, vsize/2 );
    if (segs&0x80)
        paint.drawPoint( hsize+1, vsize-1 );
    return retVal;
}

int KLSevenSegmentWidget::getSegmentsForListItem( Q3ListViewItem* item )
{
    int retVal=0;
    for (int i=0; i<8; i++)
    {
        if (item->text( i+1 ) == "1")
            retVal |= (1<<i);
    }
    return retVal;
}


void KLSevenSegmentWidget::setListItemForSegments(Q3ListViewItem *item, int segs )
{
    for (int i=0; i<8; i++)
    {
        item->setText(i+1, segs & ( 1<<i )? "1" : "0");
    }
}

QString KLSevenSegmentWidget::formStr( int nr ) const
{
    QString retVal;
    retVal = QString("%1").arg(nr, 0, 16);
    if( retVal.length() == 1 )
        retVal = "0" + retVal;
    return "0x" + retVal;
}

QList< int > KLSevenSegmentWidget::generateBitmasks( )
{
    QList< int > retVal;
    int current;
    Q3ListViewItem *item=ui->lvSegments->firstChild();
    
    while(item)
    {
        current=0;
        int segList = getSegmentsForListItem( item );
        for (int i=0; i<8; i++)
        {
            if ( segList&(1<<i) )
            {
                current |= m_segmentIsBit[i];
            }
        }
        retVal.append( current );
        item=item->nextSibling();
    }
    return retVal;
}

KLSevenSegmentWidget::~ KLSevenSegmentWidget( )
{
}


