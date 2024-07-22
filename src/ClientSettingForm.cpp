#include "ClientSettingForm.h"
#include "ui_ClientSettingForm.h"

ClientSettingForm::ClientSettingForm(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ClientSettingForm)
{
    ui->setupUi(this);
    this->client = new TCPClient();
    connect(this->client ,SIGNAL(Connected())   ,this,SLOT(Connected()));
    connect(this->client ,SIGNAL(Ready())       ,this,SLOT(SetStandby()));
    connect(this->client ,SIGNAL(Disconnected()),this,SLOT(DisConnected()));

    //Windowsでボットのexeファイルがあるときのみ、ボットプログラムを動かせるように設定
    #ifdef Q_OS_WINDOWS
        if(QFile::exists("./2019-U16asahikawaBot/u16asahikawaBot.exe"))
            ui->ComboBox->addItem("botV4");
    #endif
}

ClientSettingForm::~ClientSettingForm()
{
    delete ui;
    delete this->client;
    delete botProcess;
}

void ClientSettingForm::SetStandby()
{
    this->ui->NameLabel ->setText(this->client->Name == "" ? "Hot" : this->client->Name);
    this->ui->IPLabel   ->setText(this->client->IP);
    this->ui->StateLabel->setText("準備完了");
    //if(this->ui->ComboBox->currentText() != "TCPユーザー");
    this->ui->ConnectButton->setText("　切断　");

    emit Standby(this,true);
    //this->ui->ServerStartButton->setEnabled  (hot_standby && cool_standby && map_standby);
}

void ClientSettingForm::Connected()
{
    this->ui->IPLabel      ->setText(this->client->IP);
    this->ui->StateLabel   ->setText("接続中");
    this->ui->ConnectButton->setText("　切断　");
}

void ClientSettingForm::DisConnected() {

    disconnect(this->client, SIGNAL(Disconnected()), this, SLOT(DisConnected()));
    //TCP待機やめ
    if (dynamic_cast<TCPClient*>(this->client) != nullptr) {
        dynamic_cast<TCPClient*>(this->client)->CloseSocket();
        this->client = new TCPClient(this);
    }

    //再connectしクライアントの接続を待つ
    connect(this->client, SIGNAL(Connected()), this, SLOT(Connected()));
    connect(this->client, SIGNAL(Ready()), this, SLOT(SetStandby()));
    connect(this->client, SIGNAL(Disconnected()), this, SLOT(DisConnected()));
    this->client->Startup();

    this->ui->ConnectButton->setText("接続開始");
    this->ui->StateLabel->setText("非接続");
    this->ui->NameLabel->setText("不明");
    this->ui->IPLabel->setText("不明");
    this->ui->PortSpinBox->setEnabled(true);

    //状態解除
    if (this->ui->ConnectButton->isChecked())
        this->ui->ConnectButton->toggle();

    emit Standby(this, false);
}

void ClientSettingForm::ConnectionToggled(bool state)
{
    if(state){
        //TCP待機開始
        dynamic_cast<TCPClient*>(this->client)->OpenSocket(ui->PortSpinBox->value());
        this->ui->ConnectButton->setText("待機終了");
        this->ui->StateLabel->setText("TCP接続待ち状態");
        this->ui->PortSpinBox->setEnabled(false);
    }else{
        //TCP待機やめ
        dynamic_cast<TCPClient*>(this->client)->CloseSocket();
        //this->client->Startup();

        this->ui->ConnectButton->setText("接続開始");
        this->ui->StateLabel->setText("非接続");
        this->ui->NameLabel->setText("不明");
        this->ui->IPLabel->setText("不明");
        this->ui->PortSpinBox->setEnabled(true);
        emit Standby(this,false);
    }
}

void ClientSettingForm::ComboBoxChenged(QString text)
{
    //接続初期化
    delete client;
    if(text=="TCPユーザー"){
        this->client = new TCPClient(this);
        this->ui->StateLabel->setText("非接続");
        this->ui->NameLabel->setText("不明");
        this->ui->ConnectButton->setText("接続開始");
        this->ui->PortSpinBox->setEnabled(true);
        this->ui->ConnectButton->setEnabled(true);
    }else {
        this->ui->PortSpinBox->setEnabled(false);
        this->ui->ConnectButton->setEnabled(false);
        if(text=="自動くん")this->client = new ComClient(this);
        if(text=="ManualClient")this->client = new ManualClient(this);

        //ボットを接続
        if(text=="botV4"){
            this->client = new TCPClient(this);

            //待機開始
            ConnectionToggled(true);

            botProcess = new QProcess();

            // "u16asahikawaBot.exe a:127.0.0.1 p:2009 n:botV4"のような文字列を作る
            QString command = "./2019-U16asahikawaBot/u16asahikawaBot.exe";
            QStringList option;

            option << "a:127.0.0.1" << "p:" + QString::number(ui->PortSpinBox->value()) << "n:botV4";

            botProcess->start(command, option);
        }
    }

    emit Standby(this,false);
    //再connectしクライアントの接続を待つ
    connect(this->client,SIGNAL(Connected())   ,this,SLOT(Connected()));
    connect(this->client,SIGNAL(Ready())       ,this,SLOT(SetStandby()));
    connect(this->client,SIGNAL(Disconnected()),this,SLOT(DisConnected()));
    this->client->Startup();
}

void ClientSettingForm::SetPortSpin(int num)
{
    ui->PortSpinBox->setValue(num);
}
