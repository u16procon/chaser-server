#include "StableLog.h"

void StableLog::Write(const QString &str) const
{
    //排他制御(スレッドセーフ)にするためにロックを取得
    QMutexLocker locker(m_mutex);

    //追加書き込みオープン
    QFile file(m_filename);
    file.open(QIODevice::Append | QIODevice::Unbuffered | QIODevice::Text);

    //書いて
    file.write(str.toUtf8());

    //閉じる
    file.close();
}

const StableLog &StableLog::operator<<(const QString &str) const
{
    Write(str);
    return (*this);
}

StableLog::StableLog()
    : m_filename(),
    m_mutex(new QMutex())
{

}
StableLog::StableLog(QString filename)
    : m_filename(filename),
    m_mutex(new QMutex())
{
    //排他制御(スレッドセーフ)にするためにロックを取得
    QMutexLocker locker(m_mutex);

    QFile file(m_filename);
    QTextStream log;

    //ログファイルオープン
    file.open(QIODevice::WriteOnly);

    log.setDevice(&file);
    log << "--Stable Log--\r\n";
    file.close();
}

// コピーはファイル名だけ複製し、mutex は新しく確保する
StableLog::StableLog(const StableLog& other)
    : m_filename(other.m_filename),
    m_mutex(new QMutex())
{
    
}

// 代入はファイル名だけ複製し、mutex は新しく確保しなおす
StableLog& StableLog::operator=(const StableLog& other)
{
    if (this == &other) return *this;

    // 新しい mutex を確保してから代入・破棄（例外安全を簡易に確保）
    QMutex* newMutex = new QMutex();

    // ファイル名をコピー
    m_filename = other.m_filename;

    // 既存の mutex を破棄して新しい mutex を設定
    delete m_mutex;
    m_mutex = newMutex;

    return *this;
}

StableLog::~StableLog()
{
    delete m_mutex;
}
