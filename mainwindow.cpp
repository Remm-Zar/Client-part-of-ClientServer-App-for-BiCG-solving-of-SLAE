#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QDataStream>
#include <QFile>


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
    m_txtInput=new QLineEdit;
    m_txtInfo->setReadOnly(true);
    QPushButton *button=new QPushButton("&Send");

    connect(button,SIGNAL(clicked()),this,SLOT(slotSendToServer()));
    connect(m_txtInput,SIGNAL(returnPressed()),this,SLOT(slotSendToServer()));
    QVBoxLayout *vbox=new QVBoxLayout;
    vbox->addWidget(new QLabel("Client"));
    vbox->addWidget(m_txtInfo);
    vbox->addWidget(m_txtInput);
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
    QString fileName=m_txtInput->text();
    QFile file("C:/Users/Hp/Desktop/QT projects/"+fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray arr;
    QDataStream out(&arr,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out<<quint16{0};
    QByteArray q=file.readAll();
    arr.append(q);
    out.device()->seek(0);
    out<<quint16(arr.size()-sizeof(quint16));
    qint64 x=0;
    while (x<arr.size())
    {
        qint64 y=m_socket->write(arr);
        x+=y;
    }

    m_txtInput->clear();
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
    qDebug()<<"status: sockReady";
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
    m_data=m_socket->readAll();
    double *N;
    N=new double[m_data.size()/sizeof (double)];
    memcpy(N,m_data.data(),m_data.size());
    for (unsigned int i=0;i<m_data.size()/sizeof (double);++i)
    {
        qDebug()<<N[i]<<" ";
        m_txtInfo->insertPlainText(QString::number(N[i])+" ");
    }

    delete N;


}
