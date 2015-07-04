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
#ifndef KLPROGRAMFUSESWIDGET_H
#define KLPROGRAMFUSESWIDGET_H

#include "ui_klprogramfuseswidgetbase.h"
#include "klcpufuses.h"

class KLProject;


class KLProgramFusesWidget: public QDialog {
Q_OBJECT
public:
    KLProgramFusesWidget(QWidget *parent, KLProject* project, const char *name = 0);
private:
    Ui::KLProgramFusesWidgetBase *ui;
public slots:
    virtual void slotClose();
    virtual void slotWrite();
    virtual void slotRead();
    virtual void setFuseBits( unsigned char lowByte,
                              unsigned char highByte,
                              unsigned char extByte,
                              unsigned char lockByte );
    virtual void setFuseBits( const QString& lowByte,
                              const QString& highByte,
                              const QString& extByte,
                              const QString& lockByte );
    virtual void slotCPUChanged( const QString& );

    void setLowByte(unsigned char theValue)
    { m_lowByte = theValue; }
    unsigned char lowByte() const
    { return m_lowByte; }
    void setHighByte(unsigned char theValue)
    { m_highByte = theValue; }
    unsigned char highByte() const
    { return m_highByte; }
    void setExtByte(unsigned char theValue)
    { m_extByte = theValue; }
    unsigned char extByte() const
    { return m_extByte; }
    void setLockByte(unsigned char theValue)
    { m_lockByte = theValue; }
    unsigned char lockByte() const
    { return m_lockByte; }

protected:
    virtual void showEvent( QShowEvent* e );
    void updateGUIFromData();
    void updateDataFromGUI();
    KLProject* m_project;
    KLCPUFuses m_fuses;
    
    unsigned char m_lowByte, m_highByte, m_extByte, m_lockByte;
};

#endif
