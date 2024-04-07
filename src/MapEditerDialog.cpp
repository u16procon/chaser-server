#include "MapEditerDialog.h"
#include "ui_MapEditerDialog.h"
#include <QFileDialog>
#include <QDesktopServices>

MapEditerDialog::MapEditerDialog(GameSystem::Map map, QWidget *parent) :
    QDialog(parent),
    filepath(""),
    ui(new Ui::MapEditerDialog),
    clicking(false)
{
    ui->setupUi(this);

    //マウス追跡を有効化
    setMouseTracking(true);

    //ブロック、アイテムの数をカウントするリストを追加
    ui->ObjectCounter->addItem(new QListWidgetItem("×" + QString(QString::number(0))));
    ui->ObjectCounter->addItem(new QListWidgetItem("×" + QString(QString::number(0))));
    //カウント部分のアイコンを設定
    ui->ObjectCounter->setIconSize(QSize(ICON_SIZE*0.8, ICON_SIZE*0.8));
    ui->ObjectCounter->item(0)->setIcon(QIcon(map.texture_dir_path + "/Block.png"));
    ui->ObjectCounter->item(1)->setIcon(QIcon(map.texture_dir_path + "/Item.png"));

    ComboChanged("決戦(15x17)");
    ui->widget->setMap(map);
    ui->listWidget->addItem(new QListWidgetItem("Nothing"));
    ui->listWidget->addItem(new QListWidgetItem("Target"));
    ui->listWidget->addItem(new QListWidgetItem("Block"));
    ui->listWidget->addItem(new QListWidgetItem("Item"));
    ui->listWidget->setIconSize(QSize(ICON_SIZE*0.8, ICON_SIZE*0.8));
    ui->listWidget->item(0)->setIcon(QIcon(map.texture_dir_path + "/Floor.png"));
    ui->listWidget->item(1)->setIcon(QIcon(map.texture_dir_path + "/Cool.png"));
    ui->listWidget->item(2)->setIcon(QIcon(map.texture_dir_path + "/Block.png"));
    ui->listWidget->item(3)->setIcon(QIcon(map.texture_dir_path + "/Item.png"));

    //ブロック、アイテムの数をカウントして表示
    ReCount();

    connect(
        ui->listWidget,
        SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this,
        SLOT(SelectItem(QListWidgetItem*, QListWidgetItem*)));

    connect(ui->TurnSpin, SIGNAL(valueChanged(int)), this, SLOT(SpinChanged(int)));

    ui->listWidget->setCurrentRow(0);

    ui->TurnSpin->setValue(ui->widget->field.turn);

    //ランダム生成ボタン
    connect(ui->randomGenerateButton, SIGNAL(pressed()), this, SLOT(randomGenerateButtonPressed()));
}

MapEditerDialog::~MapEditerDialog()
{
    // QApplication::setOverrideCursor(Qt::ArrowCursor);
    delete ui;
}

GameSystem::Map MapEditerDialog::GetMap(){
    return ui->widget->field;
}

void MapEditerDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        this->clicking = true;
        FillItem(event->pos());
    }
}
void MapEditerDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)this->clicking = false;
}
void MapEditerDialog::mouseMoveEvent(QMouseEvent* event)
{
    if(clicking)
        FillItem(event->pos()); //設置

}

void MapEditerDialog::FillItem(const QPoint& pos)
{
    int left_m,top_m;
    this->layout()->getContentsMargins(&left_m, &top_m, nullptr, nullptr);
    QPoint fill_point((pos.x() - left_m)/ui->widget->image_part.width(), (pos.y() - top_m)/ui->widget->image_part.height());

    //有効範囲内でなければスキップ
    if(fill_point.x() < 0 || fill_point.x() >= ui->widget->field.size.x() ||
       fill_point.y() < 0 || fill_point.y() >= ui->widget->field.size.y())return;

    GameSystem::MAP_OBJECT obj = GameSystem::MAP_OBJECT::NOTHING;
    if     (ui->listWidget->selectedItems().first()->text() == "Nothing")obj = GameSystem::MAP_OBJECT::NOTHING;
    else if(ui->listWidget->selectedItems().first()->text() == "Block"  )obj = GameSystem::MAP_OBJECT::BLOCK;
    else if(ui->listWidget->selectedItems().first()->text() == "Item"   )obj = GameSystem::MAP_OBJECT::ITEM;
    if(ui->listWidget->selectedItems().first()->text() == "Target" ){
        //初期位置変更
        this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::COOL)] = fill_point;
        this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::HOT )] = ui->widget->field.MirrorPoint(fill_point);
        this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::COOL)] = fill_point;
        this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::HOT )] = ui->widget->field.MirrorPoint(fill_point);
    }else{
        this->ui->widget->field.field[fill_point.y()][fill_point.x()] = obj;
        //対称コピー
        if(ui->SymmetryCheck->isChecked()){
            QPoint r_fill_point(ui->widget->field.MirrorPoint(fill_point));
            this->ui->widget->field.field[r_fill_point.y()][r_fill_point.x()] = obj;
        }
    }
    //オブジェクト数の表示を変更するよ
    ReCount();

    update();
}

void MapEditerDialog::Clear()
{
    for(auto& v1 : ui->widget->field.field){
        for(auto& v2 : v1){
            v2 = GameSystem::MAP_OBJECT::NOTHING;
        }
    }
    //オブジェクト数の表示を変更するよ
    ReCount();

    update();
}

void MapEditerDialog::SpinChanged(int value){

    ui->widget->field.turn = value;
}

void MapEditerDialog::Export()
{

    filepath = QFileDialog::getSaveFileName(
        this,
        tr("マップを保存"),
        QDir::currentPath()+"/Map/",
        tr("マップデータ (*.map)"));
    if(filepath != "")ui->widget->field.Export(filepath);
}

void MapEditerDialog::SelectItem(QListWidgetItem *next, [[maybe_unused]] QListWidgetItem *old)
{
    if(next){
        QIcon icon = next->icon();
        QCursor myCursor = QCursor(icon.pixmap(icon.actualSize(QSize(ICON_SIZE*0.8, ICON_SIZE*0.8))));
        // QApplication::setOverrideCursor(myCursor);
        // widget内(GameBoard内)でのみカーソルを変える
        ui->widget->setCursor(myCursor);
    }
}

void MapEditerDialog::ComboChanged([[maybe_unused]] QString value)
{
    randomGenerateButtonPressed();

    //ウインドウの縦横のマス目の比を取る
    double window_width_per_height = static_cast<double>(ui->widget->field.size.x())/ui->widget->field.size.y();
    //画面の最大高さをもとに、ウインドウの最大高さを決める
    int window_height = static_cast<int>(QGuiApplication::primaryScreen()->size().height()*0.8);

    resize(QSize(window_height*window_width_per_height+134, window_height+4));

    paintEvent(nullptr);
    update();
}

//アイテム数えなおし
void MapEditerDialog::ReCount()
{
    //ブロックの数をカウントして表示
    int counter = 0;
    counter = ui->widget->GetMapObjectCount(GameSystem::MAP_OBJECT::BLOCK);
    ui->ObjectCounter->item(0)->setText("×" + QString(QString::number(counter)));
    //アイテムの数をカウントして表示
    counter = ui->widget->GetMapObjectCount(GameSystem::MAP_OBJECT::ITEM);
    ui->ObjectCounter->item(1)->setText("×" + QString(QString::number(counter)));
}

//ランダムマップの生成しなおし
void MapEditerDialog::randomGenerateButtonPressed()
{
    auto fieldSizeText = ui->comboBox->currentText();

    if(fieldSizeText=="広域(21x17)"){
        this->ui->widget->field.SetSize(QPoint(21,17), ui->BlockSpin->value(), ui->ItemSpin->value());
    }
    else if(fieldSizeText=="決戦(15x17)"){
        this->ui->widget->field.SetSize(QPoint(15,17), ui->BlockSpin->value(), ui->ItemSpin->value());
    }

    this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::COOL)] = this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::COOL)];
    this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::HOT )] = this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::HOT )];

    ui->widget->setMap(ui->widget->field);
    paintEvent(nullptr);
    ReCount();
    update();

    if(this->ui->BlockSpin->value() % 2 != 0) // ブロック数が奇数なら警告
        QMessageBox::information(this, tr("警告"), tr("ブロックは必ず偶数個で生成されます"));

    if(this->ui->ItemSpin->value() % 2 == 0) // アイテム数が偶数なら警告
        QMessageBox::information(this, tr("警告"), tr("アイテムは必ず奇数個で生成されます"));

}
