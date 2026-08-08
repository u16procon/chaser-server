#include "SettingDialog.h"
#include <QFileDialog>
#include <QSettings>
#include "ui_SettingDialog.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
{
    QSettings mSettings("setting.ini", QSettings::IniFormat); // iniファイルで設定を保存
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード
    ui->setupUi(this);

    QVariant v = mSettings.value("LogFilepath");
    if (v.typeId() != QMetaType::UnknownType)
        ui->Log->setText(v.toString());

    v = mSettings.value("Timeout");
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        ui->Timeout->setValue(v.toInt());

    v = mSettings.value("Gamespeed");
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        ui->Gamespeed->setValue(v.toInt());

    v = mSettings.value("Silent");
    if (v.typeId() != QMetaType::UnknownType)
        ui->SilentCheck->setChecked(v.toBool());

    v = mSettings.value("Maximum");
    if (v.typeId() != QMetaType::UnknownType)
        ui->MaximumCheck->setChecked(v.toBool());

    //AnimationTime設定
    QSettings aSettings("AnimationTime.ini", QSettings::IniFormat);
    v = aSettings.value( "Map" );
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        ui->AnimeMapTime->setValue(v.toInt());

    v = aSettings.value("Team");
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        ui->AnimeTeamTime->setValue(v.toInt());
}

void SettingDialog::Export()
{
    QSettings mSettings("setting.ini", QSettings::IniFormat); // iniファイルで設定を保存
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード

    mSettings.setValue("LogFilepath", ui->Log->text());
    mSettings.setValue("Timeout", ui->Timeout->value());
    mSettings.setValue("Gamespeed", ui->Gamespeed->value());
    mSettings.setValue("Silent", ui->SilentCheck->isChecked());
    mSettings.setValue("Maximum", ui->MaximumCheck->isChecked());

    QSettings aSettings("AnimationTime.ini", QSettings::IniFormat );
    aSettings.setValue("Map", ui->AnimeMapTime->value());
    aSettings.setValue("Team", ui->AnimeTeamTime->value());
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
