#include "taskwindow.h"
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskWindow)
{
    ui->setupUi(this);
    QWidget *whiteWidget = new QWidget(this);
    whiteWidget->lower();
    whiteWidget->setGeometry(20, 10, 430, 175);
    whiteWidget->setStyleSheet("background-color: white;");
}

TaskWindow::~TaskWindow()
{
    delete ui;
}
