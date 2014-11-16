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
#ifndef KLWIZARDHEADERSETTING_H
#define KLWIZARDHEADERSETTING_H

#include <qstringlist.h>


/**
This class stores a header setting for a wizard.
For example, it stores, that there is a XTAL variable to be set as string
in the header file of the HD44780 wizard.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLWizardHeaderSetting{
public:
    typedef enum { NoType=0,
                   IntType=1,
                   StringType=2,
                   StringListType=3 } SettingType;
    KLWizardHeaderSetting();
    KLWizardHeaderSetting(const QString& grpName, const QString& defName,
                          const QString& description,
                          int value, int min, int max, int base=10);
    KLWizardHeaderSetting(const QString& grpName, const QString& defName,
                          const QString& description,
                          const QString& value);
    KLWizardHeaderSetting(const QString& grpName, const QString& defName,
                          const QString& description,
                          const QString& value, const QStringList possibleValues);

    ~KLWizardHeaderSetting();

    SettingType type() const { return m_type; }
    QString defName() const { return m_defName; }
    int intValue() const { return m_intValue; }
    void setIntValue(const int& theValue) { m_intValue = theValue; }
    void setValue( const QString& val );
    int min() const { return m_min; }
    int max() const { return m_max; }
    int base() const { return m_base; }
    void setStrValue(const QString& theValue)
    { m_strValue = theValue; }
    QString strValue() const { return m_strValue; }
    QString stringValue() const { return m_strValue; }
    void setPossibleValues(const QStringList& theValue)
    { m_possibleValues = theValue; }
    QStringList possibleValues() const { return m_possibleValues; }
    QString description() const { return m_description; }
    QString valueAsString() const; 
    QString groupName() const { return m_groupName; }

protected:
    SettingType m_type;
    QString m_defName;
    int m_intValue, m_min, m_max, m_base;
    QString m_strValue;
    QStringList m_possibleValues;
    QString m_description;
    // This is the name of the group this setting belongs to:
    QString m_groupName;
};

#endif
