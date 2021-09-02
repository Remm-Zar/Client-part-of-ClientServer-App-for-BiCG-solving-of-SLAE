#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>
#include <QTextEdit>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTcpSocket *m_socket;
    QByteArray m_data;
    quint16 m_nextBlockSize;
    QTextEdit *m_txtInfo;
    QLineEdit *m_txtInput;

    MainWindow(const QString &host,int port,QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void sockReady();
    void sockDisc();
    void slotError(QAbstractSocket::SocketError err);
    void slotSendToServer();
    void slotConnected();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
