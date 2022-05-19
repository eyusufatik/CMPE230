#include "Calculator.h"

Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void Calculator::digitClicked()
{
    if (waitingEqual && display->text() != "" && display->text()[0] != '-')
    {
        display->clear();
    }

    Button *clickedButton = qobject_cast<Button *>(sender());

    if (display->text() == "0")
        display->clear();

    display->setText(display->text() + clickedButton->text());
}

std::string Calculator::signedLongToHexStr(long x)
{

    if(x == 0){
        return "0";
    }

    std::string s;

    std::string pre;
    if (x < 0)
    {
        pre = "-";
        x = abs(x);
    }
    else
    {
        pre = "";
    }

    while (x != 0)
    {
        int remainder = x % 16;
        if (remainder == 10)
        {
            s = "A" + s;
        }
        else if (remainder == 11)
        {
            s = "B" + s;
        }
        else if (remainder == 12)
        {
            s = "C" + s;
        }
        else if (remainder == 13)
        {
            s = "D" + s;
        }
        else if (remainder == 14)
        {
            s = "E" + s;
        }
        else if (remainder == 15)
        {
            s = "F" + s;
        }
        else
        {
            s = std::to_string(remainder) + s;
        }

        x /= 16;
    }

    return pre + s;
}

long Calculator::calculate()
{
    if (queue.size() < 3)
    {
        return 0;
    }

    long result = std::stol(queue[0], nullptr, 16);
    long temp = 0;
    int op = -1; // 1 for - 0 for +
    for (int i = 1; i < queue.size(); ++i)
    {
        std::string s = queue[i];
        if (s.compare("+") != 0 && s.compare("-") != 0)
        {
            temp = std::stol(s, nullptr, 16);
            if (op == 0)
            {
                result += temp;
            }
            else if (op == 1)
            {
                result -= temp;
            }
        }
        else
        {
            if (s.compare("+") == 0)
                op = 0;
            if (s.compare("-") == 0)
                op = 1;
        }
    }

    std::string resstr = signedLongToHexStr(result);
    std::transform(resstr.begin(), resstr.end(), resstr.begin(), ::toupper);
    display->setText(QString::fromStdString(resstr));
    waitingEqual = false;
    return result;
}
void Calculator::equalClicked()
{
    queue.push_back(display->text().toStdString());
    if (waitingEqual)
    {
        calculate();
        queue.clear();
        waitingEqual = false;
    }
}
void Calculator::plusClicked()
{
    queue.push_back(display->text().toStdString());
    queue.push_back("+");
    display->clear();
    waitingEqual = true;
}
void Calculator::minusClicked()
{
    if (display->text() == "" || display->text() == "0")
    {
        display->setText(QString::fromStdString("-"));
        return;
    }
    queue.push_back(display->text().toStdString());
    queue.push_back("-");
    display->clear();
    waitingEqual = true;
}
void Calculator::clearClicked()
{
    waitingEqual = false;
    queue.clear();
    display->clear();
}

Calculator::Calculator(QWidget *parent) : QWidget(parent)
{
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    for (int i = 0; i < 10; ++i)
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));

    digitButtons[10] = createButton(QString("A"), SLOT(digitClicked()));
    digitButtons[11] = createButton(QString("B"), SLOT(digitClicked()));
    digitButtons[12] = createButton(QString("C"), SLOT(digitClicked()));
    digitButtons[13] = createButton(QString("D"), SLOT(digitClicked()));
    digitButtons[14] = createButton(QString("E"), SLOT(digitClicked()));
    digitButtons[15] = createButton(QString("F"), SLOT(digitClicked()));

    plusButton = createButton(QString("+"), SLOT(plusClicked()));
    minusButton = createButton(QString("-"), SLOT(minusClicked()));
    equalButton = createButton(QString("="), SLOT(equalClicked()));
    clearButton = createButton(QString("Clr"), SLOT(clearClicked()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout->addWidget(display, 0, 0, 1, 4);

    mainLayout->addWidget(plusButton, 1, 0);
    mainLayout->addWidget(minusButton, 1, 1);
    mainLayout->addWidget(equalButton, 1, 2);
    mainLayout->addWidget(clearButton, 1, 3);

    for (int i = 0; i < 16; ++i)
    {
        int row = 2 + i / 4;
        int column = i % 4;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    setLayout(mainLayout);

    setWindowTitle(tr("Calculator"));
}
