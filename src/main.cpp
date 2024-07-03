#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // ダークテーマに変わらないように設定
    // (デフォルト:windows11 -> windowsvista)
    a.setStyle("windowsvista");
    MainWindow w;
    w.show();

    return a.exec();
}
