#include "StableLog.h"
#include <QSharedPointer>



bool StableLog::Write(const QString& str) const{
    //追加書き込みオープン
    QSharedPointer<QFile> file = QSharedPointer<QFile>::create(filename);
    file->open(QIODevice::Append);
    QTextStream log;
    log.setDevice(file.data());

    //書いて
    log << str;
}

const StableLog& StableLog::operator<<(const QString& str)const{
    Write(str);
    return (*this);
}

StableLog::StableLog()
{

}

StableLog::StableLog(QString filename) : filename(filename){

    QSharedPointer<QFile> file = QSharedPointer<QFile>::create(filename);

    QTextStream log;
    file->open(QIODevice::WriteOnly);
    log.setDevice(file.data());
    //ログファイルオープン
    log.setDevice(file.data());
    log << "--Stable Log--\r\n";
}

StableLog::~StableLog()
{

}

