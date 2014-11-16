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
#ifndef KLPROGRAMMERCONFIGWIDGET_H
#define KLPROGRAMMERCONFIGWIDGET_H

#include "ui_klprogrammerconfigwidgetbase.h"
#include <qmap.h>

class KontrollerLab;


class KLProgrammerConfigWidget: public QDialog {
Q_OBJECT
public:
    KLProgrammerConfigWidget(KontrollerLab *parent = 0, const char *name = 0);
    void updateGUIFromConfig();
    void updateConfigFromGUI();
    void backannotateSTK500( const QString & stdout );
private:
    Ui_KLProgrammerConfigWidgetBase *ui;
public slots:
    virtual void slotCancel();
    virtual void slotOK();

    QMap< QString, QString > configuration() const { return m_configuration; }
    virtual void slotSerialPortChanged( const QString& );
    virtual void slotParallelPortChanged( const QString& );
    virtual void slotSetDefault();
    virtual void slotWriteSTK500();
    virtual void slotReadSTK500();
    virtual void slotSetEraseCounter();
protected:
    virtual void showEvent( QShowEvent* e );
    QString config( const QString& key, const QString& defVal = "" );
    
    QMap<QString, QString> m_configuration;
    KontrollerLab* m_parent;
};

#endif
