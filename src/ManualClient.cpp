#include "ManualClient.h"
#include <QEventLoop>
#include <QSharedPointer>

void ManualClient::closeEvent(){
    emit Disconnected();
    disconnected_flag=true;
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
    QDialog::connect(diag.data(), SIGNAL(ReadyAction()), &eventLoop, SLOT(quit()));
    QDialog::connect(this, SIGNAL(Disconnected()), &eventLoop, SLOT(quit()));
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
    diag = QSharedPointer<ManualClientDialog>::create();
    connect(diag.data(), SIGNAL(CloseWindow()), this, SLOT(closeEvent()));
}

ManualClient::~ManualClient()
{
    diag->hide();
}

