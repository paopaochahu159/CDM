#include "download.h"



extern const int thread_quantity;

Download::Download(const QUrl & u, const int i, const QVector<qint64>& aa, QObject *parent)
    : QObject{parent}
    ,fileOrder(i)
    ,a(aa)
    ,url(u)
{ }

Download::~Download(){
    if (manager != nullptr) manager->deleteLater();
    if (reply != nullptr) reply->deleteLater();
}

void Download::start(){
    manager = new QNetworkAccessManager();
    QNetworkRequest net(url);
    net.setRawHeader(QByteArray("Range"), QString("bytes=%1-%2")
                                              .arg(a[0])
                                              .arg(a[1])
                                              .toLocal8Bit()
                     );
    net.setRawHeader("User-Agent",
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36");

    m_fileName = QString("%1/temporary files/%2%3")
                     .arg(QCoreApplication::applicationDirPath())
                     .arg(fileOrder)
                     .arg(url.fileName());


    reply = manager->get(net);
    connect(reply, &QNetworkReply::finished, this, &Download::onFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &Download::onDownloadProgress);
}

void Download::onFinished(){
    QMutexLocker locker(&mutex);
    qDebug() << url.fileName() << "线程" + QString::number(fileOrder) + "下载完成";

    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.write(reply->readAll());
    }
    file.close();

    reply->deleteLater();
    manager->deleteLater();
    manager = nullptr;
    reply = nullptr;

    emit thread_finished_signal();
}

void Download::onDownloadProgress(const qint64 &bytesReceived, const qint64 &bytesTotal){
    QMutexLocker locker(&mutex);
    bytes = bytesReceived;
    // qDebug() << url.fileName() <<  "线程" << fileOrder << "更新进度" << bytes;

    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.write(reply->readAll());
    }
    file.close();

    emit refresh_signal(fileOrder, m_bytesReceived + bytes, m_bytesReceived + bytesTotal);
}

void Download::stop(){
    QMutexLocker locker(&mutex);

    disconnect(reply, &QNetworkReply::finished, this, &Download::onFinished);
    disconnect(reply, &QNetworkReply::downloadProgress, this, &Download::onDownloadProgress);
    a[0] += bytes + 1;
    m_bytesReceived = bytes;
    reply->abort();
    reply->deleteLater();
    manager->deleteLater();
    manager = nullptr;
    reply = nullptr;
}

void Download::go_on(){
    QMutexLocker locker(&mutex);

    qDebug() << fileOrder << "启动";
    manager = new QNetworkAccessManager();
    QNetworkRequest net(url);
    qDebug() << "线程：" <<fileOrder << "暂停下载位置:  " << a;
    net.setRawHeader(QByteArray("Range"), QString("bytes=%1-%2")
                                              .arg(a[0])
                                              .arg(a[1])
                                              .toLocal8Bit()
                     );
    net.setRawHeader("User-Agent",
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36");

    reply = manager->get(net);
    connect(reply, &QNetworkReply::finished, this, &Download::onFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &Download::onDownloadProgress);

}


/*
 * 断点续传存在问题：
 * 1.恢复下载立即奔溃或过一段时间奔溃
 * 2.出现某个线程停止下载
 * 3.全部下载完成后最终文件不可用
 * 4.某个线程下载完成时奔溃
 *
 * 开启vpn的时候下载会突然停止
*/



// 构造函数
DownloadManager::DownloadManager(const QUrl &u, QObject *parent)
    : QObject(parent)
    , url(u)
{
    //获得文件大小
    QNetworkAccessManager *m = new QNetworkAccessManager;
    QNetworkRequest request(url);
    m->head(request);
    connect(m, &QNetworkAccessManager::finished, this, &DownloadManager::get_SizeAndName);
}
//获得文件大小和名字
void DownloadManager:: get_SizeAndName(QNetworkReply* headReply){
    qint64 fileSize = 0;
    if (headReply->hasRawHeader(QString("Content-Length").toUtf8()))
    {
        fileSize = headReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        QVariant contentDisposition = headReply->header(QNetworkRequest::ContentDispositionHeader);
        if (contentDisposition.isValid()) {
            QString contentDispositionValue = contentDisposition.toString();
            static QRegularExpression regex("filename=\"?([^\";]*)\"?");
            QRegularExpressionMatch match = regex.match(contentDispositionValue);
            if (match.hasMatch()){
                fileName = match.captured(1);
                qDebug() << "File name:" << fileName;
            } else {
                fileName = url.fileName();
                qDebug() << "Filename not found in Content-Disposition";
            }
        } else {
            fileName = url.fileName();
            qDebug() << "Content-Disposition header not found";
        }
        qDebug() << fileSize / 1048576.0;
        divide_equally(fileSize);
    }
    emit header_OK(fileName, fileSize / 1048576.0);
    headReply->deleteLater();
    headReply->manager()->deleteLater();
}

//分配每个线程下载的区域
void DownloadManager::divide_equally(const qint64& fileSize){
    qDebug() << fileSize;
    qint64 part = fileSize / thread_quantity;
    QVector<qint64> size(2);
    for (int i = 0; i < thread_quantity; ++i){
        size[0] = i * part;
        size[1] = i * part + part - 1;
        vecSize.push_back(size);
    }
    //余数部分加入最后一个
    vecSize[thread_quantity - 1][1] = fileSize;
    qDebug() << vecSize;
}

// 析构函数，清理资源
DownloadManager::~DownloadManager()
{
    for (int i = 0; i < threads.size(); i++){
        if (threads[i] != nullptr) {
            qDebug() << "线程或者运行，已阻塞并释放";
            threads[i]->quit();
            threads[i]->wait();
            threads[i]->deleteLater();
            tasks[i]->deleteLater();
        } else {
            qDebug() << "线程已结束";
        }
    }
}

// 开始任务
void DownloadManager::startDownload()
{
    for (int i = 0; i < thread_quantity; i++){
        QThread *thread = new QThread;
        Download *task = new Download(url, i, vecSize[i]);
        // 将下载任务移动到新的线程中
        task->moveToThread(thread);

        // 连接信号和槽
        connect(task, &Download::refresh_signal, this, &DownloadManager::refresh_signal);
        connect(thread, &QThread::started, task, &Download::start);
        connect(task, &Download::thread_finished_signal, this, [this]{
            num++;
            if (num == thread_quantity){
                merging_data();
            }
        });
        connect(task, &Download::thread_finished_signal, thread, &QThread::quit);
        connect(task, &Download::thread_finished_signal, task, &Download::deleteLater);
        connect(thread, &QThread::finished, thread, [i, this]{
            threads[i]->deleteLater();
            threads[i] = nullptr;
        });

        // 添加到列表中
        threads.append(thread);
        tasks.append(task);
        emit add_progress();

        // 启动线程
        thread->start();
    }
}

void DownloadManager::merging_data(){
    QString path = QCoreApplication::applicationDirPath() + "/temporary files/%1" + url.fileName();
    qDebug() << '\n' << fileName << "全部下载完成！" << '\n';

    std::thread  ttt([path, this]{
        QFile final_file(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + fileName);
        if (!final_file.open(QIODevice::Append)){
            qDebug() << "写入打开失败：" << final_file.fileName();
            return;
        }

        for (int i = 0; i < thread_quantity; i++){
            QFile temporary(path.arg(i));
            if (!temporary.open(QIODevice::ReadOnly)){
                qDebug() << "读取打开失败：" << temporary.fileName();
                return;
            }
            QByteArray content = temporary.readAll();
            final_file.write(content);
            temporary.remove();
        }
        final_file.close();
    });
    ttt.detach();
    emit completes_signals();
}

void DownloadManager::stop(){
    for (Download *d : tasks){
        d->stop();
    }
}

void DownloadManager::go_on(){
    for (Download *d : tasks){
        d->go_on();
    }
}
