#ifndef KLSPINBOXWIDGET_H
#define KLSPINBOXWIDGET_H

#include <QSpinBox>

class QRegExpValidator;

class KLSpinBoxWidget : public QSpinBox
{
    Q_OBJECT
public:
    explicit KLSpinBoxWidget(QWidget *parent = 0);
    void setValidator(QRegExpValidator *validator);
    void setBase(int base);

protected:
    virtual QValidator::State validate(QString &text, int &pos) const;
    virtual int valueFromText(const QString &text) const;
    virtual QString textFromValue(int value) const;

private:
    QRegExpValidator *m_validator;
    int m_base;

signals:

public slots:

};

#endif // KLSPINBOXWIDGET_H
