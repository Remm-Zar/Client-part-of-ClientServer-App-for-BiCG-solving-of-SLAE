#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QDataStream>
#include <QFile>
#include <QTime>


MainWindow::MainWindow(const QString &host,int port,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_nextBlockSize=0;
    m_socket=new QTcpSocket(this);
    m_socket->connectToHost(host,port);
    QPushButton *but=new QPushButton("Close conection");

    connect(m_socket,SIGNAL(connected()),this,SLOT(slotConnected()));
    connect(m_socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    connect(but,SIGNAL(clicked()),this,SLOT(sockDisc()));
    m_txtInfo=new QTextEdit;
    m_txtInfo->setReadOnly(true);


    QVBoxLayout *vbox=new QVBoxLayout;
    vbox->addWidget(new QLabel("Client"));
    vbox->addWidget(m_txtInfo);
    vbox->addWidget(but);
    ui->centralwidget->setLayout(vbox);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotError(QAbstractSocket::SocketError err)
{
    QString strErr="Error: "+(err==QAbstractSocket::HostNotFoundError?"The host wasn't found":err==QAbstractSocket::RemoteHostClosedError?"The remote host is closed":QString(m_socket->errorString()));
    m_txtInfo->append(strErr);
}
void MainWindow::slotConnected()
{
    m_txtInfo->append("Status:connected\n");
}
void MainWindow::sockDisc()
{
    m_txtInfo->insertPlainText("\nSocket closed. Goodbye");
    m_socket->close();
    m_socket->deleteLater();
}
void MainWindow::sockReady()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_9);

    if (!m_nextBlockSize)
    {
        if (m_socket->bytesAvailable()<sizeof(quint16))
        {
            qDebug()<<"1:"<<m_socket->bytesAvailable();
            return;
        }
        in>>m_nextBlockSize;
    }
    if (m_socket->bytesAvailable()<m_nextBlockSize)
    {
        qDebug()<<"2";
        return;
    }
    QString str;
    in>>str;
    m_data=m_socket->readAll();
   // end=QTime::currentTime();
    m_txtInfo->insertPlainText(str+"\n");
    m_nextBlockSize=0;
}
