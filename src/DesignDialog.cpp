#include "DesignDialog.h"
#include "ui_DesignDialog.h"
#include "qsettings.h"
#include "qmessagebox.h"
#include <QSharedPointer>

DesignDialog::DesignDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DesignDialog)
{
    ui->setupUi(this);

    QSharedPointer<QSettings> mSettings;
    mSettings = QSharedPointer<QSettings>::create( "design.ini", QSettings::IniFormat ); // iniファイルで設定を保存
    mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード

    QVariant v = mSettings->value( "Dark" );
    if (v.type() != QVariant::Invalid)
    {
        ui->DarkBox->setChecked(v.toBool());
    }
    v = mSettings->value( "Bot" );
    if(v.type() != QVariant::Invalid){
        ui->BotBox->setChecked(v.toBool());
    }
}

DesignDialog::~DesignDialog()
{
    delete ui;
}

bool DesignDialog::GetCheckDark() const {
    return ui->DarkBox->isChecked();
}

bool DesignDialog::GetCheckBot() const {
    return ui->BotBox->isChecked();
}

void DesignDialog::Export(){

    QSharedPointer<QSettings> mSettings;
    mSettings = QSharedPointer<QSettings>::create( "design.ini", QSettings::IniFormat ); // iniファイルで設定を保存
    mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード

    mSettings->setValue( "Dark"    , GetCheckDark());
    mSettings->setValue( "Bot"     , GetCheckBot());
    QMessageBox::information(this, tr("警告"), tr("設定は再起動後有効になります。"));
}
