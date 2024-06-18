#include "taskwindow.h"
#include "ui_taskwindow.h"

extern const int thread_quantity;

TaskWindow::TaskWindow(const QUrl &u, const int i, QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::TaskWindow)
    ,location(i)
    ,url(u)
{
    ui->setupUi(this);
    downloadManager = new DownloadManager(url);
    connect(downloadManager, &DownloadManager::completes_signals, this, [this]{
        qDebug() << 111111;
        timer->stop();
        ui->progressBar->setValue(100);
        ui->label_6->setText("0.00 秒");
        emit state_signal(location);
    });
    connect(downloadManager, &DownloadManager::add_progress, this, &TaskWindow::add_progressBar);
    connect(downloadManager, &DownloadManager::refresh_signal, this, &TaskWindow::refresh_signal);

    ui->label->setText(url.toString());
    ui->pushButton_2->setEnabled(false);
    connect(downloadManager, &DownloadManager::header_OK, this, [this](const QString& s, const double& n){
        ui->pushButton_2->setEnabled(true);
        setWindowTitle(s);
        ui->label_8->setText(QString::number(n));
    });
    ui->progressBar->setStyleSheet(R"(QProgressBar{
                                    font:9pt;
                                    border-radius:5px;
                                    text-align:center;
                                    border:1px solid #E8EDF2;
                                    background-color: rgb(255, 255, 255);
                                    border-color: rgb(180, 180, 180);
                                }
                                QProgressBar:chunk{
                                    border-radius:5px;
                                    background-color:#1ABC9C;
                                }
                )");
    ui->progressBar->setRange(0, 0);

    //设置区域背景颜色为白色
    whiteWidget = new QWidget(this);
    whiteWidget->lower();
    whiteWidget->setGeometry(10, 10, 460, 190);
    whiteWidget->setStyleSheet("background-color: white;");
}

TaskWindow::~TaskWindow()
{
    qDebug() << "删除窗口" << location;
    downloadManager->deleteLater();
    whiteWidget->deleteLater();
    delete ui;
}

#include<QTime>

void TaskWindow::on_pushButton_2_clicked()
{
    downloadManager->startDownload();
    // 记录定时器开始的时间
    startTime= QTime::currentTime();

    emit start_signal(location, windowTitle(), ui->label_8->text().toDouble());
    ui->progressBar->setRange(0, 100);
    timer = new QTimer(this);
    timer->start(100);
    connect(timer, &QTimer::timeout, this, &TaskWindow::schedule_update);
}

void TaskWindow::schedule_update(){
    int n = 0;
    for (int i = 0; i < thread_quantity; i++){
        n += bars[i]->value();
    }
    ui->progressBar->setValue(n / 4);
    double elapsedSeconds = startTime.msecsTo(QTime::currentTime()) / 1000.0;
    double speed = (ui->label_8->text().toDouble() * ((n / 4) / 100.0)) / elapsedSeconds;
    double time_remaining = ui->label_8->text().toDouble() / speed - elapsedSeconds;
    ui->label_7->setText(QString::number(ui->label_8->text().toDouble() * ((n / 4) / 100.0)));
    ui->label_9->setText(QString::number(speed) + " MB/秒");
    ui->label_6->setText(QString::number(time_remaining) + "秒");

    emit schedule_updat_signal(location, speed, time_remaining);
}

void TaskWindow::add_progressBar(){
    qDebug() << "添加线程" << "进度条";
    QProgressBar * bar = new QProgressBar(this);
    bar->setStyleSheet("QProgressBar{text-align:center;background-color:#DDDDDD;border: 1px solid black;}"
                       "QProgressBar::chunk{background-color:#05B8CC;}");
    bar->setFixedHeight(10);
    bars.append(bar);
    ui->horizontalLayout_2->addWidget(bar);
}

void TaskWindow::refresh_signal(const int i, const qint64 bytesReceived, const qint64 bytesTotal){
    Existing_bytes_size += bytesReceived;
    int progress = static_cast<int>((bytesReceived * 100) / bytesTotal);
    bars[i]->setValue(progress);
}

void TaskWindow::on_pushButton_3_clicked()
{
    timer->stop();
    downloadManager->stop();
}


void TaskWindow::on_pushButton_4_clicked()
{
    downloadManager->go_on();
    timer->start(100);
}

