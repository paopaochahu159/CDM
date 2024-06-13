#include "taskwindow.h"
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(const QUrl &url, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskWindow)
    ,url(url)
{
    ui->setupUi(this);

    dow = new DownloadManager(url);

    //设置区域背景颜色为白色
    whiteWidget = new QWidget(this);
    whiteWidget->lower();
    whiteWidget->setGeometry(10, 10, 410, 165);
    whiteWidget->setStyleSheet("background-color: white;");
}

TaskWindow::~TaskWindow()
{
    delete whiteWidget;
    delete ui;
}

void TaskWindow::refresh_the_page(){
    qDebug() << "refresh the page";
}

void TaskWindow::on_pushButton_2_clicked()
{
    dow->addDownload();
    // dow->start();
    b = true;
    emit maintain_signal();
}

