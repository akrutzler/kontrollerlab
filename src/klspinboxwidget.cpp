#include "klspinboxwidget.h"

KLSpinBoxWidget::KLSpinBoxWidget(QWidget *parent) :
    QSpinBox(parent)
{
    setRange(0, 255);
    m_validator = new QRegExpValidator(QRegExp("[0-9]{1,8}"), this);
    m_base = 10;
}

void KLSpinBoxWidget::setValidator(QRegExpValidator *validator)
{
    if(m_validator)
        delete m_validator;

    m_validator = validator;
}

QValidator::State KLSpinBoxWidget::validate(QString &text, int &pos) const
{
    return m_validator->validate(text, pos);
}

int KLSpinBoxWidget::valueFromText(const QString &text) const
{
    return text.toInt(0, m_base);
}

QString KLSpinBoxWidget::textFromValue(int value) const
{
    return QString::number(value, m_base).toUpper();
}

void KLSpinBoxWidget::setBase(int base)
{
    m_base = base;
}

