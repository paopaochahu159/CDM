#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QHeaderView>
#include<QStorageInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("多线程下载器");

    //设置工具栏 宽高
    ui->toolBar->setFixedSize(1800,70);
    //设置工具栏 使文字在图片下面
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //设置工具栏 不可移动
    ui->toolBar->setMovable(false);
    //设置列表列 自适应窗口大小
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qDebug() << QCoreApplication::applicationDirPath();

    QString folder_name("temporary files");    //要创建的文件夹名称
    QDir dir(QCoreApplication::applicationDirPath());    //初始化dir为exe文件所在的目录
    if(!dir.exists(folder_name))    //如果Images文件夹不存在
    {
        dir.mkdir(folder_name);    //创建文件夹（名为Images）
        qDebug()<<QString("文件夹%1创建成功！").arg(folder_name);
    }
}

void MainWindow::immediately_delete(){
    qDebug() << "immediately_delete";
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::get_url(){
    // 创建输入对话框
    QInputDialog *in = new QInputDialog();
    in->resize(450, in->width());//设置长度和宽度
    in->setWindowTitle("输入新任务的地址");
    in->setLabelText("地址：");

    // 获取文本输入
    if (in->exec() != QInputDialog::Accepted) {
        delete in;
        return "";
    }
    QString u = in->textValue();
    in->deleteLater();//工作完成 释放内存
    //正则表达式
    static QRegularExpression urlRegex("^https?://(?:www\\.)?\\S+\\.\\S+$");
    //使用正则表达式验证输入的url是否正确
    if (urlRegex.match(u).hasMatch() == false){
        QMessageBox::warning(this, "警告", "输入的地址错误");
        on_actionNew_triggered();
        return "";
    }
    return u;
}

//创建新任务
void MainWindow::on_actionNew_triggered()
{
    QString u = get_url();
    // QString u = "https://vscode.download.prss.microsoft.com/dbazure/download/stable/dc96b837cf6bb4af9cd736aa3af08cf8279f7685/VSCodeSetup-x64-1.89.1.exe";
    if (u.isEmpty())
        return;
    QUrl url(u);
    TaskWindow* task = new TaskWindow(url, location++);
    task->setWindowIcon(QIcon(":/img/img/xiazai.ico"));
    task->show();

    add_table();
    maintain_queue.append(task);

    //连接信号与曹 实现Download通知窗口进行相应操作
    connect(task, &TaskWindow::start_signal, this, &MainWindow::start_download);
}

void MainWindow::start_download(const int row,  const QString& fileName, const double& size){
    qDebug() << "start_download";
    ui->tableWidget->item(row, 0)->setText(fileName);
    ui->tableWidget->item(row, 1)->setText(QString::number(size));
    ui->tableWidget->item(row, 2)->setText("正在下载");
}

void MainWindow::add_table(){
    int RowConut = ui->tableWidget->rowCount();
    //添加一行内容
    ui->tableWidget->insertRow(RowConut);
    //必须先设置item,然后再获取,因为默认是空的

    for (int i = 0; i < ui->tableWidget->columnCount(); i++){
        ui->tableWidget->setItem(RowConut, i, new QTableWidgetItem("-"));
    }
    ui->tableWidget->setItem(RowConut, 2, new QTableWidgetItem("等待"));
}

//删除任务
void MainWindow::on_actionD_triggered()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
    if(items.empty()){
        QMessageBox::warning(this,"警告","请先选择一行再进行操作");
        return;
    }
    location--;
    int n = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(n);
    delete maintain_queue[n];
    maintain_queue.remove(n);

    for (int i = n; i < maintain_queue.size(); i++){
        maintain_queue[i]->refresh_location();
    }
}

void MainWindow::on_actionSet_triggered()
{
    SetupWindow * setup  = new SetupWindow();
    setup->setWindowModality(Qt::ApplicationModal);
    setup->setWindowIcon(QIcon(":/img/img/set.ico"));
    setup->show();
    connect(setup, &SetupWindow::kill, setup, [setup]{
        setup->deleteLater();
    });
}

