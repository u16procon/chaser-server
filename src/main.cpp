#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // ダークテーマに変わらないように設定
    // (デフォルト:windows11 -> windowsvista)
    a.setStyle("windowsvista");

    QTranslator qtTranslator;

    // 返り値を無視
    (void) qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
