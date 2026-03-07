#include "TcpClient.h"
#include <QSettings>
#include <QRegularExpression>
#include <QFont>
#include <QLabel>

QString TCPClient::VisibilityString(QString str)
{
    QString answer;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\n')
            answer.push_back("\\n");
        else if (str[i] == '\r')
            answer.push_back("\\r");
        else
            answer.push_back(str[i]);
    }
    return answer;
}

QString TCPClient::WaitResponce()
{
    int ignore = 0;
    while (ignore != this->IGNORE_INVALD) {
        //レスポンス待ち
        qDebug() << "WaitStart:";
        if (this->client->waitForReadyRead(this->TIMEOUT)) {
            //レスポンスあり

            QString response = "";

            //自動結合
            //bool f = false;
            do {
                response += client->readLine();
            } while (*(response.end() - 1) != '\n' && this->client->waitForReadyRead(this->TIMEOUT));

            qDebug() << "" + VisibilityString(response);

            //不正文字列：空
            if (response == "" || response == "\n" || response == "\r" || response == "\r\n") {
                ignore++;

                continue;
            }
            //不正文字列：改行なし
            if (response.size() > 0 && *(response.end() - 1) != '\n') {
                is_disconnected = true;
                qDebug() << QString("[Port") + QString::number(this->client->localPort())
                                + "]:Noting \\n";

                return QString();
            }

            qDebug() << "test:" + response;
            return response;
        } else {
            //レスポンスなし
            is_disconnected = true;
            //qDebug() << QString("[Port") + QString::number(this->client->localPort()) +"]:Noting responce";
            return QString();
        }
    }
    is_disconnected = true;
    //qDebug() << QString("[Port") + QString::number(this->client->localPort()) +"]:Too many invald responce";
    return QString();
}

bool TCPClient::WaitGetReady()
{
    if (is_disconnected)
        return false;
    //ターン開始文字列
    if (client == nullptr)
        return false;
    client->write(QString("@\r\n").toUtf8());

    //レスポンス待ち
    QString response = WaitResponce();
    qDebug() << "res:" + response;
    qDebug() << (response == "gr\r\n");
    return (response == "gr\r\n");
}

GameSystem::Method TCPClient::WaitReturnMethod(GameSystem::AroundData data)
{
    if (is_disconnected)
        return GameSystem::Method();
    //周辺情報文字列
    if (client == nullptr)
        return GameSystem::Method{GameSystem::TEAM::UNKNOWN,
                                  GameSystem::Method::ACTION::UNKNOWN,
                                  GameSystem::Method::ROTE::UNKNOWN};
    client->write(QString(data.toString() + "\r\n").toUtf8());
    qDebug() << "WRM";
    qDebug() << data.toString();
    //レスポンス待ち
    QString response = WaitResponce();
    if (response != QString())
        return GameSystem::Method::fromString(response);
    else
        return GameSystem::Method{GameSystem::TEAM::UNKNOWN,
                                  GameSystem::Method::ACTION::UNKNOWN,
                                  GameSystem::Method::ROTE::UNKNOWN};
}

bool TCPClient::WaitEndSharp(GameSystem::AroundData data)
{
    if (is_disconnected)
        return false;

    if (client == nullptr)
        return false;
    //周辺情報文字列
    client->write(QString(data.toString() + "\r\n").toUtf8());
    qDebug() << "WES";
    qDebug() << data.toString();
    //レスポンス待ち
    return (WaitResponce() == "#\r\n");
}

bool TCPClient::OpenSocket(int Port)
{
    this->server->listen(QHostAddress::Any, Port);
    return true;
}

bool TCPClient::CloseSocket()
{
    if (this->client != nullptr && this->client->isOpen()) {
        this->client->close();
    }
    if (this->server->isListening()) {
        this->server->close();
    }
    return true;
}

bool TCPClient::isConnecting()
{
    return this->server->isListening();
}

void TCPClient::NewConnection()
{
    delete this->client;
    this->client = this->server->nextPendingConnection();
    this->IP = this->client->peerAddress().toString();
    connect(this->client, &QTcpSocket::readyRead,    this, &TCPClient::GetTeamName);
	connect(this->client, &QTcpSocket::disconnected, this, &TCPClient::DisConnected);
    is_disconnected = false;

    //クライアントが1度接続された後は、接続を受け付けない
    this->server->close();
    emit Connected();
}

void TCPClient::DisConnected()
{
    if (this->client->isOpen()) {
        this->client->close();
    }
    if (this->server->isListening()) {
        this->server->close();
    }
    this->IP = "";
    this->Name = "";
    is_disconnected = true;
    connect(this->client, &QTcpSocket::readyRead,    this, &TCPClient::GetTeamName);
    connect(this->client, &QTcpSocket::disconnected, this, &TCPClient::DisConnected);
    emit Disconnected();
}

QString TCPClient::GetTeamName()
{
    if (this->Name == "") {
        QByteArray bytebuf = client->readAll();

        //不明な文字が発見された場合、文字コードをUTFに変換 (0xFFFD)
        QString namebuf = QString::fromUtf8(bytebuf);

        //名前のエスケープ処理(改行やタブなどの文字の効果を削除)
        static auto REX = QRegularExpression(
            "["
             "\r\n\u2028\u2029\u202A\u202B\u202C\u202D\u202E\u202F" //改行
             "\u200B" //ゼロ幅スペース
             "\a\b\f\t\v\uFFFD" //その他
             "]"
            );
        namebuf = namebuf.replace(REX, "");

        QLabel nameLabel;
        //HTMLタグは解釈しない
        nameLabel.setTextFormat(Qt::PlainText);
        nameLabel.setFont(QFont("Yu Gothic UI", 20));

        QString clientName = "", lineText = "";
        bool insertNewLineFlag = false;

        for(int i = 0; i < namebuf.length(); i++){

            QString nextChar = namebuf[i];
            nameLabel.setText(lineText + nextChar);

            //サロゲート文字の場合は、次のQCharも連結する
            if(namebuf[i].isHighSurrogate()){
                i++;
                nextChar += namebuf[i];
                nameLabel.setText(lineText + nextChar);
            }
            //次の文字が字形選択子で絵文字用の異体字セレクタの場合は、次のQCharも連結する
            else if(i + 1 < namebuf.length() && (QString(namebuf[i + 1]) == "\uFE0E" || QString(namebuf[i + 1]) == "\uFE0F")){
                i++;
                nextChar += namebuf[i];
                nameLabel.setText(lineText + nextChar);
            }

            nameLabel.adjustSize();

            //文字幅を計算して、ある程度以上の幅の文字列の表示をしない/改行して分割する
            //(LIMIT_NAME_PIXEL_SIZE以上なら2行に分割)
            if(nameLabel.size().width() < LIMIT_NAME_PIXEL_SIZE){
                clientName += nextChar;
                lineText += nextChar;
            } else if (!insertNewLineFlag){
                insertNewLineFlag = true;
                clientName += "\n";
                clientName += nextChar;
                lineText = nextChar;
            } else { //1回改行が入ったあとは、2行目まで表示する(3行目以降は無視)
                break;
            }

        }

        this->Name = clientName;

        disconnect(this->client, &QTcpSocket::readyRead, this, &TCPClient::GetTeamName);

        emit WriteTeamName();
        emit Ready();
        return this->Name;
    }
    return this->Name;
}

TCPClient::TCPClient(QObject *parent)
    : BaseClient(parent)
{
    QSettings *mSettings;
    mSettings = new QSettings("setting.ini", QSettings::IniFormat); // iniファイルで設定を保存
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード (QSettingsはデフォルトでUTF-8で、変更不可っぽい)
    QVariant v = mSettings->value("Timeout");
    // 非推奨 QVariant::Invalid → QMetaType::UnknownType
    if (v.typeId() != QMetaType::UnknownType) {
        TIMEOUT = v.toInt();
    }
    delete mSettings;

    this->server = new QTcpServer(this);
    this->client = nullptr;
    //接続最大数を1に固定
    this->server->setMaxPendingConnections(1);
    //シグナルとスロットを接続
    connect(this->server, &QTcpServer::newConnection, this, &TCPClient::NewConnection);
}

TCPClient::~TCPClient()
{
    if (isConnecting()) {
        CloseSocket();
    }

    delete this->client;
    delete this->server;
}
