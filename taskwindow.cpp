#include "taskwindow.h"
#include "ui_taskwindow.h"

TaskWindow::TaskWindow(const QUrl &u, const int i, QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::TaskWindow)
    ,location(i)
    ,url(u)
{
    ui->setupUi(this);
    downloadManager = new DownloadManager(url);

    ui->label->setText(url.toString());
    ui->pushButton_2->setEnabled(false);
    connect(downloadManager, &DownloadManager::header_OK, this, [this](const QString& s, const double& n){
        ui->pushButton_2->setEnabled(true);
        setWindowTitle(s);
        ui->label_8->setText(QString::number(n));
    });

    //设置区域背景颜色为白色
    whiteWidget = new QWidget(this);
    whiteWidget->lower();
    whiteWidget->setGeometry(10, 10, 410, 165);
    whiteWidget->setStyleSheet("background-color: white;");
}

TaskWindow::~TaskWindow()
{
    qDebug() << "删除窗口" << location;
    // downloadManager->deleteLater();
    whiteWidget->deleteLater();
    delete ui;
}

void TaskWindow::refresh_the_page(){
    qDebug() << "refresh the page";
}

void TaskWindow::on_pushButton_2_clicked()
{
    downloadManager->startDownload();
    emit start_signal(location, windowTitle(), ui->label_8->text().toDouble());
}

