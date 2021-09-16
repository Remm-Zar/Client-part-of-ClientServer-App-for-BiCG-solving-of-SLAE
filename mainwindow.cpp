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

    connect(m_socket,SIGNAL(connected()),this,SLOT(slotConnected()));
    connect(m_socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    m_txtInfo=new QTextEdit;
    m_txtMat=new QLineEdit;
    m_txtVec=new QLineEdit;
    m_txtInfo->setReadOnly(true);
    button=new QPushButton("&Send");

    connect(button,SIGNAL(clicked()),this,SLOT(slotSendToServer()));
    connect(m_txtMat,SIGNAL(returnPressed()),this,SLOT(slotSendToServer()));
   // connect(m_txtVec,SIGNAL(returnPressed()),this,SLOT(slotSendToServer()));
    QVBoxLayout *vbox=new QVBoxLayout;
    vbox->addWidget(new QLabel("Client"));
    vbox->addWidget(m_txtInfo);
    vbox->addWidget(m_txtMat);
    vbox->addWidget(m_txtVec);
    vbox->addWidget(button);
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
void MainWindow::slotSendToServer()
{
    m_txtInfo->clear();
    QString fileNameMat=m_txtMat->text(),fileNameVec=m_txtVec->text();
    QFile fileMat("C:/Users/Hp/Desktop/QT projects/ClientApp/"+fileNameMat),fileVec("C:/Users/Hp/Desktop/QT projects/ClientApp/"+fileNameVec);
    if (fileMat.open(QIODevice::ReadOnly)&&fileVec.open(QIODevice::ReadOnly))
    {
        m_txtInfo->insertPlainText("Waiting...\n");
        qDebug()<<"Prepairing of data\n";
        QByteArray arr;
        QDataStream out(&arr,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_9);
        out<<quint64{0}<<quint64{0};
        QByteArray q=fileMat.readAll();
        qint64 size=quint64(q.size());
        arr.append(q);
        q.clear();
        q=fileVec.readAll();
        arr.append(q);
        out.device()->seek(0);
        out<<quint64(arr.size()-2*sizeof(quint64));
        out<<size;
        qint64 x=0;
        start=QTime::currentTime();
        qDebug()<<"Sending of data\n";
        while (x<arr.size())
        {
            qint64 y=m_socket->write(arr);
            x+=y;
        }
        qDebug()<<"Data has sent";
        m_txtMat->clear();
        m_txtVec->clear();
    }
    else
    {
        m_txtInfo->insertPlainText("Files don't exist");
    }
    fileMat.close();
    fileVec.close();

}
void MainWindow::slotConnected()
{
    m_txtInfo->append("Status:connected");
}
void MainWindow::sockDisc()
{
    m_socket->deleteLater();
}
void MainWindow::sockReady()
{
    m_txtInfo->clear();
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
    QTime diff;
    in>>diff;
    m_data=m_socket->readAll();
    end=QTime::currentTime();
    m_txtInfo->insertPlainText("Answer: \n"+QString(m_data.toStdString().c_str())+"\nSolving time: "+QString::number(diff.msec())+" msec\n"+"Total time: "+QString::number(end.addMSecs(start.msec()).msec())+" msec");

}
