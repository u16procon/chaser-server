#include "ManualClient.h"
#include <QEventLoop>

void ManualClient::closeEvent(){
    emit Disconnected();
    is_disconnected=true;
}

bool ManualClient::WaitGetReady(){
    //自動GetReady
    return true;
}

GameSystem::Method ManualClient::WaitReturnMethod(GameSystem::AroundData data){
    //自動返答
    diag->AppendLog(QString("[GetReady  Response] ") + data.toString());

    //GUIレスポンス待ち
    QEventLoop eventLoop;
    QDialog::connect(diag, &ManualClientDialog::ReadyAction, &eventLoop, &QEventLoop::quit);
    QDialog::connect(this, &ManualClient::Disconnected, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();//GUIからのレスポンスがあるまで待機
    return diag->next_method;
}

bool ManualClient::WaitEndSharp(GameSystem::AroundData data){
    //自動返答
    diag->AppendLog(QString("[Method Response] ") + data.toString());
    return true;
}

void ManualClient::Startup(){
    emit Connected();
    emit WriteTeamName();
    emit Ready();
    diag->setWindowFlags(Qt::WindowStaysOnTopHint);

    //画面のサイズをもとにダイアログの位置を決める
    auto displaySize = QGuiApplication::primaryScreen()->size();
    auto diagSize = diag->size();

    //画面のサイズに合わせて場所移動(邪魔にならない場所)
    diag->move((displaySize.width()-diagSize.width())*0.5, displaySize.height()*0.65);

    diag->show();
}

ManualClient::ManualClient(QWidget* parent):
    BaseClient(parent)
{
    Name = "ManualClient";
    IP   = "ローカル";
    diag = new ManualClientDialog();
    connect(diag, &ManualClientDialog::CloseWindow, this, &ManualClient::closeEvent);
}

ManualClient::~ManualClient()
{
    diag->hide();
}
