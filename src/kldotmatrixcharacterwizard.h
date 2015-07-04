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
#ifndef KLDOTMATRIXCHARACTERWIZARD_H
#define KLDOTMATRIXCHARACTERWIZARD_H

#include "ui_kldotmatrixcharacterwizardbase.h"
#include "kleditdotswidget.h"
#include <qpixmap.h>

class KLProject;
class KLDocument;

class KLDotMatrixCharacterListViewItem : public Q3ListViewItem
{
public:
    KLDotMatrixCharacterListViewItem( int number, QList<int> masks,
                                      Q3ListView *parent, const char *name );
    void update( QList<int> masks );
    QList< int > masks() const { return m_masks; }

    void setNumber(const int& theValue) { m_number = theValue; updateNumberString(); }
    int number() const { return m_number; }
    KLDotMatrixCharacterListViewItem* nextSibling();
protected:
    QPixmap generatePixmapFor(QList<int> masks );
    QList<int> m_masks;
    void updateNumberString();
    int m_number;
};


class KLDotMatrixCharacterWizard: public QDialog {
    Q_OBJECT
public:
    KLDotMatrixCharacterWizard(QWidget *parent, const char *name, KLProject* project, KLDocument* doc);
private:
    Ui_KLDotMatrixCharacterWizardBase *ui;
public slots:
    virtual void slotCharacterChanged();
    virtual void slotOK();
    virtual void slotSizeChanged();
    virtual void slotSelectedCharacterChanged( Q3ListViewItem* );
    virtual void slotDown();
    virtual void slotUp();
    virtual void slotRemove();
    virtual void slotAdd();
    virtual void slotCancel();
protected:
    bool underReedit() const { return (m_startLine>=0)&&(m_stopLine>=0); }
    int m_startLine, m_stopLine;
    QString m_whiteSpace;

    void reNumber();
    KLProject* m_project;
    KLDocument* m_document;
    QGridLayout* m_frmGridLayout;
    KLEditDotsWidget* m_editDots;
};

#endif
