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
