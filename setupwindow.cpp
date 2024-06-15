#include "setupwindow.h"
#include "ui_setupwindow.h"

int thread_quantity = 4;

SetupWindow::SetupWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SetupWindow)
{
    ui->setupUi(this);
}

SetupWindow::~SetupWindow()
{
    delete ui;
}

void SetupWindow::on_comboBox_activated(int index)
{
    thread_quantity = ui->comboBox->itemText(index).toInt();
}
