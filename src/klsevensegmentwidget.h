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
#ifndef KLSEVENSEGMENTWIDGET_H
#define KLSEVENSEGMENTWIDGET_H

#include "ui_klsevensegmentwidgetbase.h"
#include <qlist.h>
#include <qradiobutton.h>
#include <qlistview.h>

class KLDocument;
        

class KLSevenSegmentWidget: public QDialog {
Q_OBJECT
public:
    KLSevenSegmentWidget(QWidget *parent, const char *name, KLDocument* doc);
    ~KLSevenSegmentWidget();
private:
    Ui::KLSevenSegmentWidgetBase *ui;
public slots:
    virtual void slotCurrentItemChanged( QTreeWidgetItem* cur, QTreeWidgetItem* previous );
    virtual void slotBitSegmentAssocChanged();
    virtual void slotDP();
    virtual void slotG();
    virtual void slotF();
    virtual void slotE();
    virtual void slotD();
    virtual void slotC();
    virtual void slotB();
    virtual void slotA();
    virtual void slotCancel();
    virtual void slotOK();
    virtual void slotDown();
    virtual void slotUp();
    virtual void slotRemove();
    virtual void slotAdd();
protected:
    inline void fillSegmentList();
    bool underReedit() const { return (m_startLine>=0)&&(m_stopLine>=0); }
    QString formStr( int nr ) const;
    QPixmap generatePixmapFor( int segs );
    QList< int > generateBitmasks();
    int getSegmentsForListItem(QTreeWidgetItem *item );
    int curSegments() { return getSegmentsForListItem( ui->lvSegments->currentItem() ); }
    void setListItemForSegments( QTreeWidgetItem* item, int segs );
    void changeCurrentListItem();
    QList < int > readBitSegmentAssignmentFromRadios();
    void setRadiosFromBitSegmentAssignment( QList < int > assign );
    QList < QList<QRadioButton*> > m_radios;
    QList < int > m_segmentIsBit;
    bool m_prohibitRecursion, m_dontReactOnToolButtons;
    KLDocument* m_document;
    // If we reedit something:
    int m_startLine, m_stopLine;
    QString m_whiteSpace;
};

#endif
