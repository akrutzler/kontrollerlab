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
#ifndef KLDOTMATRIXWIDGET_H
#define KLDOTMATRIXWIDGET_H

#include "ui_kldotmatrixdisplaywizardbase.h"
#include "klwizardheadersettings.h"
#include <q3dict.h>
#include <qvalidator.h>
#include <Q3ListViewItem>

class KLEditDotsWidget;

#define WIZARD_HEADER_FILE "dm_lcd.h"
#define WIZARD_SOURCE_FILE "dm_lcd.c"


class KLDocument;
class KLProject;
class KontrollerLab;


class KLDotMatrixWidget: public QDialog {
Q_OBJECT
public:
    KLDotMatrixWidget(KontrollerLab *parent, const char *name, KLProject* prj, KLDocument* doc);
private:
    Ui_KLDotMatrixDisplayWizardBase *ui;
public slots:
    virtual void slotOK();
    virtual void slotCancel();
    void slotSelectedValueChanged(Q3ListViewItem * item);
    virtual void slotAddHD44780Lib(bool val);
    virtual void slotChangeValue(int val);
    virtual void slotChangeValue(const QString& val);
protected:
    void updateListItem( const KLWizardHeaderSetting& set );
    void updateLvValuesFrom( const KLWizardHeaderSettings& set );
    KLDocument* m_document;
    KLDocument* m_header, *m_source;
    KLProject* m_project;
    KontrollerLab *m_parent;
    KLWizardHeaderSettings m_settings;
    QGridLayout* m_frmGridLayout;
    KLEditDotsWidget* m_editDots;
    Q3Dict<Q3ListViewItem> m_groups;
    QRegExpValidator m_validator;
};

#endif
