#include "taskwindow.h"
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(const QUrl &u, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskWindow)
    , url(u)
{
    ui->setupUi(this);
    qDebug() << url;

    dow = new DownloadManager(url);

    //设置区域背景颜色为白色
    whiteWidget = new QWidget(this);
    whiteWidget->lower();
    whiteWidget->setGeometry(10, 10, 410, 165);
    whiteWidget->setStyleSheet("background-color: white;");

    ui->label->setText(url.fileName());

    ui->pushButton_2->setEnabled(false);
    connect(dow, &DownloadManager::button_true, this, [this]{
        ui->pushButton_2->setEnabled(true);
    });
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
     // connect(thread, &QThread::started, dow, & DownloadManager::addDownload);
    dow->addDownload();

    // dow->start();
    b = true;
    emit maintain_signal();
}

