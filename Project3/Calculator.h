#ifndef CALC
#define CALC
#include <QWidget>
#include <QLineEdit>
#include <QLayout>
#include <QGridLayout>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include "Button.h"

class Calculator : public QWidget
{
    Q_OBJECT

    public:
        Calculator(QWidget *parent = nullptr);

    private slots:
        void digitClicked();
        void equalClicked();
        void plusClicked();
        void minusClicked();
        void clearClicked();
    private:
        Button *createButton(const QString &text, const char *member);
        QLineEdit *display;
        Button *digitButtons[16];
        Button *plusButton;
        Button *minusButton;
        Button *equalButton;
        Button *clearButton;
        bool waitingEqual = false;
        bool rightAfterEqual = false;
        std::vector<std::string> queue;
        long calculate();
        std::string signedLongToHexStr(long);
};

#endif