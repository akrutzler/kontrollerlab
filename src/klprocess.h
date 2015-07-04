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
#ifndef KLPROCESS_H
#define KLPROCESS_H


#include <qobject.h>
#include <qprocess.h>

class KLProcessHandler;

/**
This is a single process manager.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLProcess : public QObject
{
Q_OBJECT
public:
    KLProcess(QString command, QString workingDir = "",
              const QStringList& envVars = QStringList(),
              QObject *parent = 0, const char *name = 0);

    ~KLProcess();

    int exitStatus() { return m_otherError ? 1 : m_process->exitCode(); }
    void start();
    void kill();
    void setCallBack( KLProcessHandler* cb ) { m_callback = cb; }
    KLProcessHandler* callBack() const { return m_callback; }
    QString stdout() const { return m_stdout; }
    QString stderr() const { return m_stderr; }
    QString cmd() const { return m_cmd; }

public slots:
    void slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus);
    void readFromStdOut();
    void readFromStdErr();

signals:
    void processExited( KLProcess* );


protected:
    // E.g. Command not found leads to an other error:
    bool m_otherError;
    QString m_stdout;
    QString m_stderr;
    QString m_cmd;
    QString m_program;
    QProcess* m_process;
    QStringList m_arguments;
    QStringList m_envVars;
    KLProcessHandler* m_callback;
};

#endif
