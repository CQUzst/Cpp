#ifndef HOMEADDR_H
#define HOMEADDR_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QGridLayout>
class Homeaddr : public QWidget
{
    Q_OBJECT
public:
    explicit Homeaddr(QWidget *parent = 0);

signals:

public slots:
private:
    QLabel *NationalLabel;
    QComboBox *NationalComboBox;
    QLabel *ProvinceLabel;
    QComboBox *ProvinceComboBox;
    QLabel *CityLabel;
    QLineEdit *CityLineEdit;
    QLabel *IntroductLabel;
    QTextEdit *IntroductTextEdit;
    QGridLayout *mainLayout;
};

#endif // HOMEADDR_H
