#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QSharedPointer>
#include "BaseClient.h"

class TCPClient : public BaseClient
{
    Q_OBJECT
    //TCPユーザー用の接続サーバー
private:
    int TIMEOUT = 5000;   //タイムアウト
    const static int IGNORE_INVALD = 10;   //無効ライン無視回数

    const QString NEWLINE_CODE = "";

    QSharedPointer<QTcpServer>  server;        //TCPサーバー
    QSharedPointer<QTcpSocket>  client;        //TCPクライアント
public:
    QString VisibilityString(QString str);//可視化
    QString WaitResponce();//通信待機


    virtual bool WaitGetReady();
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data);
    virtual bool WaitEndSharp(GameSystem::AroundData data);

    //ポート開く
    bool OpenSocket(int Port);
    //ポート閉じる
    bool CloseSocket();
    //接続確認
    bool isConnecting();

    explicit TCPClient(QObject *parent = 0);
    virtual ~TCPClient();

signals:

private slots:
    void NewConnection (); //新規接続
    void DisConnected (); //切断

public slots:
    QString GetTeamName();//チーム名取得
};

#endif // TCPCLIENT_H
