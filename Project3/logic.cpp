#include "logic.h"
#include "ui_calc.h"

// Holds current value of calculation
int value = 0x0;

// Will define if this was the last math button clicked
bool isAdd = false;
bool isMul = false;

// Constructor
Calc::Calc(QWidget *parent) :

    // Call the QMainWindow constructor
    QMainWindow(parent),

    // Create the UI class and assign it to the ui member
    ui(new Ui::Calc)
{

    // Setup the UI
    ui->setupUi(this);

    // Put 0.0 in Display
    ui->Display->setText(QString::number(value));

    // Will hold references to all the number buttons
    QPushButton *numButtons[10];

    // Cycle through locating the buttons
    for(int i = 0; i < 10; ++i){
        QString butName = "Button" + QString::number(i);

        // Get the buttons by name and add to array
        numButtons[i] = Calc::findChild<QPushButton *>(butName);

        // When the button is released call num_pressed()
        connect(numButtons[i], SIGNAL(released()), this,
                SLOT(NumPressed()));
    }

    // Connect signals and slots for math buttons
    connect(ui->Add, SIGNAL(released()), this,
            SLOT(MathButtonPressed()));
    connect(ui->Divide, SIGNAL(released()), this,
            SLOT(MathButtonPressed()));

    // Connect equals button
    connect(ui->Equals, SIGNAL(released()), this,
            SLOT(EqualButtonPressed()));


}

Calc::~Calc()
{
    delete ui;
}

void Calc::NumPressed(){
    bool ok;
    
    // Sender returns a pointer to the button pressed
    QPushButton *button = (QPushButton *)sender();

    // Get number on button
    QString butVal = button->text();

    // Get the value in the display
    QString displayVal = ui->Display->text();

    if(displayVal.toInt(&ok, 16) == 0x0){

        // value = butVal.toDouble();
        ui->Display->setText(butVal);

    } else {
        // Put the new number to the right of whats there
        QString newVal = displayVal + butVal;
        bool ok;

        // Double version of number
        int hexVal = newVal.toInt(&ok, 16);

        // value = newVal.toDouble();

        // Set value in display and allow up to 16
        // digits before using exponents
        ui->Display->setText(QString::number(hexVal, 'g', 16));

    }
}

void Calc::MathButtonPressed(){

    // Cancel out previous math button clicks
    isMul = false;
    isAdd = false;
    bool ok;
    
    // Store current value in Display
    QString displayVal = ui->Display->text();
    value = displayVal.toInt(&ok, 16);

    // Sender returns a pointer to the button pressed
    QPushButton *button = (QPushButton *)sender();

    // Get math symbol on the button
    QString butVal = button->text();

    if(QString::compare(butVal, "*", Qt::CaseInsensitive) == 0){
        isMul = true;
    } else if(QString::compare(butVal, "+", Qt::CaseInsensitive) == 0){
        isAdd = true;
    }

    // Clear display
    ui->Display->setText("");

}

void Calc::EqualButtonPressed(){

    // Holds new calculation
    int output = 0x0;
    bool ok;

    // Get value in display
    QString displayVal = ui->Display->text();
    int dblDisplayVal = displayVal.toInt(&ok, 16); 
    // TODO: double to hex!

    // Make sure a math button was pressed
    if(isAdd || isMul ){
        if(isAdd){
            output = value + dblDisplayVal;
        } else {
            output = value * dblDisplayVal;
        }
    }

    // Put output in display
    ui->Display->setText(QString::number(output, 16));

}
