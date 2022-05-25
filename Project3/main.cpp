#include <QApplication>
#include "Calculator.h"

int main(int argc, char *argv[])
{   
    int rc;
    QApplication app(argc, argv);

    Calculator *calc = new Calculator();

    calc->show();

    rc = app.exec();
    return (rc);
}
