#ifndef MANUALCLIENT_H
#define MANUALCLIENT_H

#include "BaseClient.h"
#include "ManualClientDialog.h"
#include <QSharedPointer>

class ManualClient : public BaseClient
{
    Q_OBJECT
public:
    QSharedPointer<ManualClientDialog> diag;

public:
    bool WaitGetReady()override;
    GameSystem::Method WaitReturnMethod(GameSystem::AroundData data)override;
    bool WaitEndSharp(GameSystem::AroundData data)override;

    void Startup() override;

    ManualClient(QWidget *parent=0);
    ~ManualClient() override;
public slots:
    void closeEvent();
};

#endif // MANUALCLIENT_H
