#ifndef LOGIC_H
#define LOGIC_H

#include <QMainWindow>

// Use the standard UI namespace which is tied to the .ui file
namespace Ui {
class Calc;
}

class Calc : public QMainWindow
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    
    // Declare a constructor and by passing 0 we state this widget
    // has no parent
    explicit Calc(QWidget *parent = 0);
    ~Calc();

private:
    Ui::Calc *ui;

    // These slots are executed when a signal is
    // submitted (Ex. Number button is clicked)
private slots :
    void NumPressed();
    void MathButtonPressed();
    void EqualButtonPressed();
    void ChangeNumberSign();
};

#endif