/***************************************************************************
 *   Copyright (C) 2006 by Mario Boikov and Martin Strasser                *
 *   squeeze  a t  cadmaniac  d o t  org                                   *
 *   strasser  a t  cadmaniac  d o t  org                                  *
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
#ifndef KLSERIALTERMINALWIDGET_H
#define KLSERIALTERMINALWIDGET_H

#include "ui_klserialterminalwidgetbase.h"

#include <QDialog>
#include "klserialport.h"
#include <kconfig.h>
#include <vector>
#include "klcharvector.h"
#include <ksharedconfig.h>


class KLSerialPortListener;
class KontrollerLab;

#define CONFIG_RTSCTS "CONFIG_RTSCTS"
#define CONFIG_XONXOFF "CONFIG_XONXOFF"
#define CONFIG_SERIAL_DEVICE "CONFIG_SERIAL_DEVICE"
#define CONFIG_SERIAL_BAUD "CONFIG_SERIAL_BAUD"
#define CONFIG_DISCONNECT_WHEN_ACTIVE "CONFIG_DISCONNECT_WHEN_ACTIVE"
#ifndef TRUE_STRING
#define TRUE_STRING "TRUE"
#endif
#ifndef FALSE_STRING
#define FALSE_STRING "FALSE"
#endif

#define ASCII_ENTRY 0
#define HEX_ENTRY 1
#define SCOPE_ENTRY 2
#define HISTOGRAM_ENTRY 3


class KLSerialTerminalWidget: public QDialog {
Q_OBJECT
public:
    KLSerialTerminalWidget(KontrollerLab *parent = 0, const char *name = 0);
    virtual ~KLSerialTerminalWidget();

    void setProgrammerBusy( bool val );
    // Use this routine to activate the terminal's function as
    // interface to the microcontroller for the in circuit simulator:
    void setTerminalToICDMode( bool val )
    { m_terminalInICDMode = val; }
    void setTerminalToNormalMode( bool val )
    { m_terminalInICDMode = !val; }
    bool terminalInICDMode() const { return m_terminalInICDMode; }
    bool terminalInNormalMode() const { return !m_terminalInICDMode; }
    void saveProperties(KSharedConfig::Ptr conf );
    void readProperties(KSharedConfig::Ptr config );

    QMap< QString, int > baudConsts() const { return m_baudConsts; }
    QStringList baudRates() const { return m_baudRates; }
    QStringList ports() const;

private:
    Ui::KLSerialTerminalWidgetBase *ui;


public slots:
    virtual void slotShowAsHEX(bool);
    virtual void slotShowAsAscii(bool);
    virtual void slotSend();

    virtual void slotReceived( const KLCharVector& what );
    virtual void slotOpenClose();
    virtual void slotClearReceiver();

    virtual void slotSendICDData( const std::vector< unsigned char >& data );
    virtual void slotOpenICD( unsigned int baudRate, bool rtscts=false, bool xonxoff=false );
    virtual void slotCloseICD();
    virtual void slotShowAsChanged(int current);

signals:
    void receivedICDData( const KLCharVector& data );
protected:
    bool isHexDigit( unsigned char ch );
    unsigned char hexToUChar( unsigned char ch );
    virtual void customEvent( QCustomEvent* ev );
    virtual void paintEvent( QPaintEvent* ev );

    static QString asciiToHEX( const KLCharVector& inp );
    static QString convertCharVectorToAscii( const KLCharVector& inp );

    KLSerialPort* m_serialPort;
    QMap<QString, int> m_baudConsts;
    QStringList m_baudRates;
    KontrollerLab *m_parent;
    KLSerialPortListener *m_listener;
    KLCharVector m_receiveBuffer;
    
    class KLScope* m_scope;
    class KLHistogram* m_histogram;
    
    bool m_displayIsHEX;
    bool m_programmerBusy;
    bool m_wasOpenedBeforeProgramming;
    bool m_terminalInICDMode;
};

#endif
