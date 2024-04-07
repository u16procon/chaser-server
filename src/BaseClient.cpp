#include "BaseClient.h"

void BaseClient::Startup()
{

}

BaseClient::BaseClient(QObject *parent) :
    QObject(parent),
    Name(""),
    is_disconnected(false)
{

}

BaseClient::~BaseClient()
{

}
