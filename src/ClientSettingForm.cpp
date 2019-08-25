#include "ClientSettingForm.h"
#include "ui_ClientSettingForm.h"
#include <QSharedPointer>

ClientSettingForm::ClientSettingForm(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ClientSettingForm)
{
    ui->setupUi(this);
    this->client = QSharedPointer<TCPClient>::create();
    connect(this->client.data() ,SIGNAL(Connected())   , this, SLOT(Connected()));
    connect(this->client.data() ,SIGNAL(Ready())       , this, SLOT(SetStandby()));
    connect(this->client.data() ,SIGNAL(Disconnected()), this, SLOT(DisConnected()));

}

ClientSettingForm::~ClientSettingForm()
{
    delete ui;
}


void ClientSettingForm::SetStandby (){
    this->ui->NameLabel ->setText(this->client->Name == "" ? "Hot" : this->client->Name);
    this->ui->IPLabel   ->setText(this->client->IP);
    this->ui->StateLabel->setText("準備完了");
    //if(this->ui->ComboBox->currentText() != "TCPユーザー");
    this->ui->ConnectButton->setText("　切断　");

    emit Standby(this,true);
    //this->ui->ServerStartButton->setEnabled  (hot_standby && cool_standby && map_standby);
}

void ClientSettingForm::Connected  (){
    this->ui->IPLabel      ->setText(this->client->IP);
    this->ui->StateLabel   ->setText("接続中");
    this->ui->ConnectButton->setText("　切断　");
}
void ClientSettingForm::DisConnected(){
    /*
    this->client = new TCPClient(this);
    this->ui->NameLabel ->setText("不明");
    this->ui->IPLabel   ->setText("不明");
    this->ui->StateLabel->setText("未接続");
    this->ui->ConnectButton->setText("接続開始");

    */
    //disconnect(this->client.data(), SIGNAL( Disconnected()), this, SLOT(DisConnected()));
    //TCP待機やめ
    //if(this->client.dynamicCast<TCPClient>()->isConnecting()){
        //this->client.dynamicCast<TCPClient>()->CloseSocket();
        //this->client = QSharedPointer<TCPClient>::create(this);
    //}

    //再connectしクライアントの接続を待つ
    //connect(this->client.data(), SIGNAL(Connected())   , this, SLOT(Connected()));
    //connect(this->client.data(), SIGNAL(Ready())       , this, SLOT(SetStandby()));
    //connect(this->client.data(), SIGNAL(Disconnected()), this, SLOT(DisConnected()));
    this->client->Startup();

    this->ui->ConnectButton->setText("接続開始");
    this->ui->StateLabel->setText("非接続");
    this->ui->NameLabel->setText("不明");
    this->ui->IPLabel->setText("不明");
    this->ui->PortSpinBox->setEnabled(true);

    //状態解除
    if(this->ui->ConnectButton->isChecked()){
        this->ui->ConnectButton->toggle();
    }

    emit Standby(this,false);
}

void ClientSettingForm::ConnectionToggled(bool state){
    if(state){
        //TCP待機開始
        this->client.dynamicCast<TCPClient>()->OpenSocket(ui->PortSpinBox->value());
        this->ui->ConnectButton->setText("待機終了");
        this->ui->StateLabel->setText("TCP接続待ち状態");
        this->ui->PortSpinBox->setEnabled(false);
    }else{
        //TCP待機やめ
        this->client.dynamicCast<TCPClient>()->CloseSocket();
        //this->client = QSharedPointer<TCPClient>::create(this);

        //再connectしクライアントの接続を待つ
        //connect(this->client.data(), SIGNAL(Connected())   , this, SLOT(Connected()));
        //connect(this->client.data(), SIGNAL(Ready())       , this, SLOT(SetStandby()));
        //connect(this->client.data(), SIGNAL(Disconnected()), this, SLOT(DisConnected()));
        this->client->Startup();

        this->ui->ConnectButton->setText("接続開始");
        this->ui->StateLabel->setText("非接続");
        this->ui->NameLabel->setText("不明");
        this->ui->IPLabel->setText("不明");
        this->ui->PortSpinBox->setEnabled(true);
        emit Standby(this,false);
    }
}

void ClientSettingForm::ComboBoxChenged(QString text){
    //接続初期化
    if(text=="TCPユーザー"){
        this->client = QSharedPointer<TCPClient>::create(this);
        this->ui->StateLabel->setText("非接続");
        this->ui->NameLabel->setText("不明");
        this->ui->ConnectButton->setText("接続開始");
        this->ui->PortSpinBox->setEnabled(true);
        this->ui->ConnectButton->setEnabled(true);
    }else {
        this->ui->PortSpinBox->setEnabled(false);
        this->ui->ConnectButton->setEnabled(false);
        if(text=="自動くん")this->client = QSharedPointer<ComClient>::create(this);
        if(text=="ManualClient")this->client = QSharedPointer<ManualClient>::create(this);
    }

    emit Standby(this,false);
    //再connectしクライアントの接続を待つ
    connect(this->client.data(), SIGNAL(Connected())   , this, SLOT(Connected()));
    connect(this->client.data(), SIGNAL(Ready())       , this, SLOT(SetStandby()));
    connect(this->client.data(), SIGNAL(Disconnected()), this, SLOT(DisConnected()));
    this->client->Startup();
}
void ClientSettingForm::SetPortSpin(int num){
    ui->PortSpinBox->setValue(num);
}

