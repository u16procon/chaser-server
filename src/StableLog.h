#ifndef STABLELOG_H
#define STABLELOG_H
#include <QString>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QTextStream>
#include <QMutex>

class StableLog
{
public:
    QString filename() { return m_filename; }

    void Write(const QString& str)const;



    const StableLog& operator<<(const QString& str)const;

    StableLog();
    StableLog(QString filename);
    StableLog(const StableLog& other);
    StableLog& operator=(const StableLog& other);
    ~StableLog();

private:
	QString m_filename;

    //排他制御(スレッドセーフ)のための変数
	mutable QMutex* m_mutex;
};

#endif // STABLELOG_H
