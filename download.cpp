#include "download.h"

Download::Download(const QUrl & u, const int i, QObject *parent)
    : QObject{parent}
    ,fileOrder(i)
    , url(u)
{ }

void Download::start(){
    manager = new QNetworkAccessManager();
    QNetworkRequest net(url);
    int a1 = a[0];
    int a2 = a[1];
    net.setRawHeader(QByteArray("Range"), QString("bytes=%1-%2")
                                              .arg(a1)
                                              .arg(a2)
                                              .toLocal8Bit());
    net.setRawHeader("User-Agent",
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36");
    reply = manager->get(net);

    file = new QFile(QString("%1/temporary files/%2%3")
                     .arg(QCoreApplication::applicationDirPath())
                     .arg(fileOrder)
                     .arg(url.fileName())
                     );
    if (!file->open(QIODevice::WriteOnly)){
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &Download::onFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &Download::onDownloadProgress);

    emit AddTable_signal();
}

void Download::onFinished(){
    qDebug() << url.fileName() << "线程" + QString::number(fileOrder) + "下载完成";
    if (file) {
        file->write(reply->readAll());
        file->close();
    }
    reply->deleteLater();
    delete file;
    delete manager;
    emit thread_finished_signal(fileOrder);
}

void Download::onDownloadProgress(){
    qDebug() << url.fileName() <<  "线程" + QString::number(fileOrder) + "更新进度";
    if (file) {
        file->write(reply->readAll());
    }
    emit refresh_TaskWindow_signal();
}



// 构造函数
DownloadManager::DownloadManager(const QUrl &u, QObject *parent)
    : QObject(parent)
    , url(u)
{
    //获得文件大小
    QNetworkAccessManager *m = new QNetworkAccessManager;
    QNetworkRequest request(url);
    QNetworkReply* headReply = m->head(request);
    connect(headReply, &QNetworkReply::finished, this, [headReply, this]{
        if (headReply->hasRawHeader(QString("Content-Length").toUtf8()))
        {
             qint64 fileSize = headReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
             qDebug() << fileSize / 1048576.0;
             divide_equally(fileSize);
        }
        headReply->deleteLater();
    });
}

void DownloadManager::divide_equally(const qint64& fileSize){
    int nnn = 4;
    qDebug() << fileSize;
    qint64 part = fileSize / nnn;
    for (int i = 0; i < nnn; ++i) {
        QVector<qint64> size(2);
        size[0] = i * part;
        size[1] = i * part + part - 1;
        vecSize.push_back(size);
    }
    //余数部分加入最后一个
    vecSize[nnn - 1][1] = fileSize;
    qDebug() << vecSize;
    emit button_true();
}

// 析构函数，清理资源
DownloadManager::~DownloadManager()
{
    for (QThread *thread : threads) {
        thread->quit();
        thread->wait();
        delete thread;
    }
    qDeleteAll(tasks);
}

// 添加一个下载任务
void DownloadManager::addDownload()
{
    for (int i = 0; i < 4; i++){
        QThread *thread = new QThread;
        Download *task = new Download(url, i);
        task->get_vecSize(vecSize[i]);

        // 将下载任务移动到新的线程中
        task->moveToThread(thread);

        // 连接信号和槽
        connect(thread, &QThread::started, task, &Download::start);
        connect(task, &Download::thread_finished_signal, this, &DownloadManager::preserve);
        connect(task, &Download::thread_finished_signal, thread, &QThread::quit);
        connect(task, &Download::thread_finished_signal, task, &Download::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        // 添加到列表中
        threads.append(thread);
        tasks.append(task);

        // 启动线程
        thread->start();
    }
}

void DownloadManager::preserve(const int i){
    temporaryFiles.append(QString::number(i) + url.fileName());
    if (temporaryFiles.size() == 4){
        temporaryFiles.sort();
        merging_data();
    }
}

void DownloadManager::merging_data(){
    QString path = QCoreApplication::applicationDirPath() + "/temporary files/";
    QString filename = url.fileName();
    qDebug() << '\n' << filename << "全部下载完成！" << '\n';

    qDebug() << temporaryFiles;

    QFile final_file(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + filename);
    if (!final_file.open(QIODevice::Append)){
        qDebug() << "写入打开失败：" << final_file.fileName();
        return;
    }

    for (int i = 0; i < 4; i++){
        QFile temporary(path + temporaryFiles[i]);
        if (!temporary.open(QIODevice::ReadOnly)){
            qDebug() << "读取打开失败：" << temporary.fileName();
            return;
        }
        QByteArray content = temporary.readAll();
        final_file.write(content);
        temporary.remove();
    }
    final_file.close();
}
