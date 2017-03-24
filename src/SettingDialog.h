#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT
public:

    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

    void Export();

public slots:
    void openDirectory();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
