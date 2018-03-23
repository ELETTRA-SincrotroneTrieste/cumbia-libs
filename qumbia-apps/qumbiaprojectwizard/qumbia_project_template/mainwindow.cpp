#include "$HFILE$"
#include "ui_$HFILE$"

$MAINCLASS$::$MAINCLASS$(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::$UIFORMCLASS$)
{
    ui->setupUi(this);
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
