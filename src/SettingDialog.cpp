#include "SettingDialog.h"
#include <QFileDialog>
#include <QSettings>
#include "ui_SettingDialog.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
{
    QSettings *mSettings;
    mSettings = new QSettings("setting.ini", QSettings::IniFormat); // iniファイルで設定を保存
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード
    ui->setupUi(this);

    QVariant v = mSettings->value("LogFilepath");

    // 非推奨 QVariant::Invalid → QMetaType::UnknownType
    if (v.typeId() != QMetaType::UnknownType)
        ui->Log->setText(v.toString());

    v = mSettings->value("Timeout");

    if (v.typeId() != QMetaType::UnknownType)
        ui->Timeout->setValue(v.toInt());

    v = mSettings->value("Gamespeed");

    if (v.typeId() != QMetaType::UnknownType)
        ui->Gamespeed->setValue(v.toInt());

    v = mSettings->value("Silent");
    if (v.typeId() != QMetaType::UnknownType)
        ui->SilentCheck->setChecked(v.toBool());

    v = mSettings->value("Maximum");

    if (v.typeId() != QMetaType::UnknownType)
        ui->MaximumCheck->setChecked(v.toBool());
}

void SettingDialog::Export()
{
    QSettings *mSettings;
    mSettings = new QSettings("setting.ini", QSettings::IniFormat); // iniファイルで設定を保存
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード

    mSettings->setValue("LogFilepath", ui->Log->text());
    mSettings->setValue("Timeout", ui->Timeout->value());
    mSettings->setValue("Gamespeed", ui->Gamespeed->value());
    mSettings->setValue("Silent", ui->SilentCheck->isChecked());
    mSettings->setValue("Maximum", ui->MaximumCheck->isChecked());
    //QMessageBox::information(this, tr("警告"), tr("設定は再起動後有効になります。"));
}

void SettingDialog::openDirectory()
{
    ui->Log->setText(QFileDialog::getExistingDirectory(this, tr("ログファイル保存先を選択")));
}

SettingDialog::~SettingDialog()
{
    delete ui;
}
