#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket=new QTcpSocket(this);
    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1",5555);
}

void MainWindow::sockDisc()
{
    socket->deleteLater();
}
void MainWindow::sockReady()
{
    if (socket->waitForConnected(500))
    {
        socket->waitForReadyRead(500);
        data=socket->readAll();
        qDebug()<<data;
    }
}
