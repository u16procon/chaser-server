#include "GameSystem.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QRandomGenerator>

QString GameSystem::TEAM_PROPERTY::getTeamName(GameSystem::TEAM team){
    if(team == GameSystem::TEAM::COOL)return "COOL";
    if(team == GameSystem::TEAM::HOT) return "HOT";
    return "UNKNOWN";
}

QPoint GameSystem::Method::GetRoteVector(){
    //方向ベクトル生成
    switch(this->rote){
        case GameSystem::Method::ROTE::UP:    return QPoint( 0,-1);
        case GameSystem::Method::ROTE::RIGHT: return QPoint( 1, 0);
        case GameSystem::Method::ROTE::DOWN:  return QPoint( 0, 1);
        case GameSystem::Method::ROTE::LEFT:  return QPoint(-1, 0);
        default:                              return QPoint( 0, 0);
    }
}

GameSystem::Method GameSystem::Method::fromString(const QString& str){
    //文字列から抽出
    GameSystem::Method answer;
    if     (str[0] == 'w')answer.action = GameSystem::Method::ACTION::WALK;
    else if(str[0] == 'l')answer.action = GameSystem::Method::ACTION::LOOK;
    else if(str[0] == 's')answer.action = GameSystem::Method::ACTION::SEARCH;
    else if(str[0] == 'p')answer.action = GameSystem::Method::ACTION::PUT;
    else                  answer.action = GameSystem::Method::ACTION::UNKNOWN;
    if     (str[1] == 'u')answer.rote   = GameSystem::Method::ROTE::UP;
    else if(str[1] == 'd')answer.rote   = GameSystem::Method::ROTE::DOWN;
    else if(str[1] == 'r')answer.rote   = GameSystem::Method::ROTE::RIGHT;
    else if(str[1] == 'l')answer.rote   = GameSystem::Method::ROTE::LEFT;
    else                  answer.rote   = GameSystem::Method::ROTE::UNKNOWN;
    return answer;
}

GameSystem::Map::Map():
    //デフォルトの設定
    turn(100),
    name("[DEFAULT MAP]"),
    size(DEFAULT_MAP_WIDTH,DEFAULT_MAP_HEIGHT),
    texture_dir_path(":/Image/Jewel"){
}
void GameSystem::Map::SetSize(QPoint size, int block_num, int item_num, bool isOldMap){
    this->size = size;
    this->CreateRandomMap(block_num, item_num, isOldMap);
}

QPoint GameSystem::Map::MirrorPoint(const QPoint& pos){
    QPoint center(size.x() / 2.0f, size.y() / 2.0f);
    return center * 2 - pos;
}

bool GameSystem::Map::Import(QString Filename){
    //ファイルからマップを読み込む
    QFile file(Filename);
    if (file.open(QIODevice::ReadOnly)){
        char buf[1024]={};
        int calm=0;
        while(file.readLine(buf,1024) != -1){
            QString str = QString::fromLatin1(buf);
            str.replace("\r","");
            str.replace("\n","");

            //マップ名
            if(str[0]=='N')name = str.remove(0,2);
            //ターン数
            if(str[0]=='T')turn = str.remove(0,2).toInt();


            //マップ
            if(str[0]=='D'){
                QStringList list = str.remove(0,2).split(",");
                QVector<GameSystem::MAP_OBJECT> vec;
                foreach(QString s,list){
                    vec.push_back(static_cast<GameSystem::MAP_OBJECT>(s.toInt()));
                }
                size.setX(vec.size());
                field[calm] = vec;
                calm++;
            }

            //チーム初期位置
            for(int i=0;i<TEAM_COUNT;i++){
                if(str[0]==GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)).at(0)){
                    QStringList list = str.remove(0,2).split(",");
                    team_first_point[i] = QPoint(list[0].toInt(),list[1].toInt());
                }
            }
        }
        return true;
    }else{
        return false;
    }

}

bool GameSystem::Map::Export(QString Filename){
    QFile file(Filename);

	//拡張子取得
    QString fileExt = Filename.split("/").last().split(".").last();

    //Log出力時は追記モードでファイルを開く
    QIODevice::OpenMode openMode = (fileExt == "txt") ? 
        (QIODevice::WriteOnly | QIODevice::Append) : QIODevice::WriteOnly;

    if (!file.open(openMode)) {
        QMessageBox::information(nullptr, "ファイルを開けません", file.errorString());
        return false;
    }

    //Map出力
    QString outname(Filename.split("/").last().remove(".map"));
    QTextStream stream( &file );

    stream << "N:" + outname << "\n";
    stream << "T:" + QString::number(this->turn) + "\n";
    stream << "S:" + QString::number(size.x()) + "," + QString::number(size.y()) + "\n";
    for(auto v1 : std::as_const(field)){
        stream << "D:";
        for(auto it = v1.begin();it != v1.end();it++){
            stream << QString::number(static_cast<int>(*it));
            if(it != v1.end()-1)stream << ",";
        }
        stream << "\n";
    }
    for(int i=0;i<TEAM_COUNT;i++){
        stream << QString(GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)).at(0))
                + ":"
                + QString::number(team_first_point[i] .x())
                + ","
                + QString::number(team_first_point[i] .y());
        //if(i == TEAM_COUNT-1)
        stream << "\n";
    }

    file.close();
    return true;
}


//大会ルール(https://www.procon-asahikawa.org/files/U16Procon-RuleBookV301.pdf)
//に則り、ブロックを置いてはいけない場所の場合はfalseを返す
bool GameSystem::Map::CheckBlockRole(QPoint pos) {
	//プレイヤーの初期位置にブロックがあってはいけない
	if (pos == team_first_point[0] || pos == team_first_point[1])
		return false;

    //X軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if (pos == team_first_point[0] + QPoint(9, 0) || pos == team_first_point[0] + QPoint(-9, 0))
        return false;
    else if (pos == team_first_point[1] + QPoint(9, 0) || pos == team_first_point[1] + QPoint(-9, 0))
        return false;
    
    //Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if (pos == team_first_point[0] + QPoint(0, 9) || pos == team_first_point[0] + QPoint(0, -9))
        return false;
    else if (pos == team_first_point[1] + QPoint(0, 9) || pos == team_first_point[1] + QPoint(0, -9))
        return false;


    //外周、かつ、プレイヤーのSearch範囲(9マス以内)にブロックがあってはいけない
	for (int i = 0; i < TEAM_COUNT; i++) {
		if (
            (pos.x() == 0 || pos.x() == size.x() - 1) && // 外周(X=0 or X=size.x()-1)、かつ、
			(pos.y() == team_first_point[i].y()) && // ブロックを置こうとしているY座標が、プレイヤーのY座標と同じ、かつ、
			(team_first_point[i].x() - 9 <= pos.x() && pos.x() <= team_first_point[i].x() + 9)) // プレイヤーのSearch範囲(X軸)なら、ブロックであってはいけない
			
            return false;

		if (
            (pos.y() == 0 || pos.y() == size.y() - 1) && // 外周(Y=0 or Y=size.y()-1)、かつ、
			(pos.x() == team_first_point[i].x()) && // ブロックを置こうとしているX座標が、プレイヤーのX座標と同じ、かつ、
			(team_first_point[i].y() - 9 <= pos.y() && pos.y() <= team_first_point[i].y() + 9)) // プレイヤーのSearch範囲(Y軸)なら、ブロックであってはいけない
			
            return false;
	}
    
    return true;
}


//大会ルール(https://www.procon-asahikawa.org/files/U16Procon-RuleBookV231.pdf)
//に則り、ブロックを置いてはいけない場所の場合はfalseを返す
bool GameSystem::Map::CheckBlockRoleOld(QPoint pos) {
    //プレイヤーの初期位置にブロックがあってはいけない
    if (pos == team_first_point[0] || pos == team_first_point[1])
        return false;

    //外周にブロックがあってはいけない
    else if (pos.x() == 0 || pos.x() == size.x() - 1 || pos.y() == 0 || pos.y() == size.y() - 1)
        return false;

    //X軸に対するSearchをしたとき、その終点にブロックがあるのはOK
    //Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if (pos == team_first_point[0] + QPoint(0, 9) || pos == team_first_point[0] + QPoint(0, -9))
        return false;
    else if (pos == team_first_point[1] + QPoint(0, 9) || pos == team_first_point[1] + QPoint(0, -9))
        return false;
    else
        return true;
}

void GameSystem::Map::CreateRandomMap(int block_num, int item_num, bool isOldMap){
    turn = 100;

    //大会ルール(v3.0.1)に準拠してブロック、アイテム、COOLとHOTをランダムに配置
    //デフォルトではブロック20個、アイテム50個を配置
    if (!isOldMap) 
    {
        name = "[RANDOM MAP]";

        //大会ルールに準拠した位置にCOOLとHOTが配置されるようにする
        do {
            auto pos = QPoint(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());
            
            //テスト用座標
            //pos = QPoint(3, 10);

            if (pos.x() < size.x() / 2 || //盤面の左側にCOOLが配置されるように
                (pos.x() == size.x() / 2 && pos.y() < size.y() / 2)) { //盤面の真ん中の縦列に生成される場合は、そのy座標が0～size.y()/2までに生成されるように
                team_first_point[0] = pos;
                break;
            }

        } while (true);

        //点対称に配置
        team_first_point[1] = MirrorPoint(team_first_point[0]);

        field.clear();
        for (int i = 0; i < size.y(); i++) {
            field.push_back(QVector<GameSystem::MAP_OBJECT>(size.x()));
        }

        //ブロック配置
        for (int i = 0; i < block_num / 2; i++) {
            QPoint pos(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());

            if (CheckBlockRole(pos) &&
                field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK ) { 

                field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::BLOCK;

                //点対称に配置
                auto mirrorPos = MirrorPoint(pos);
                field[mirrorPos.y()][mirrorPos.x()] = GameSystem::MAP_OBJECT::BLOCK;
            }
            else {
                //ブロックが大会ルールに準拠してない場合は、もう一度乱数を回す
                i--;
                continue;
            }
        }

        //アイテム配置
        for (int i = 0; i < item_num / 2; i++) {
            QPoint pos(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());

            if (pos != team_first_point[0] && pos != team_first_point[1] &&
                field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::ITEM &&
                field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK ) { 

                field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::ITEM;

                //点対称に配置
                auto mirrorPos = MirrorPoint(pos);
                field[mirrorPos.y()][mirrorPos.x()] = GameSystem::MAP_OBJECT::ITEM;
            }
            else {
                i--;
                continue;
            }
        }

    }

    //大会ルール(v2.3.1)の「A 基本タイプ」に準拠してブロック、アイテム、COOLとHOTをランダムに配置
    //*A 基本タイプ: COOLとHOTの周囲8マスにアイテムがない
    //デフォルトではブロック20個、アイテム50個を配置
    else
    {
        name = "[OLD RANDOM MAP]";

        //一様ノルム(L∞ノルム)
        //詳細はWikipediaを参照
        auto UniformNorm = [](QPoint pos) {
            return std::abs(pos.x()) > std::abs(pos.y()) ? std::abs(pos.x()) : std::abs(pos.y());
            };

        //大会ルールに準拠した位置にCOOLとHOTが配置されるようにする
        do {
            auto pos = QPoint(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());
            //盤面の真ん中にあるアイテムの周りには配置されないように
            if (UniformNorm(pos - QPoint(size.x() / 2, size.y() / 2)) <= 1)
                continue;
            if (pos.x() < size.x() / 2 || //盤面の左側にCOOLが配置されるように
                (pos.x() == size.x() / 2 && pos.y() < size.y() / 2)) { //盤面の真ん中の縦列に生成される場合は、そのy座標が0～size.y()/2までに生成されるように
                team_first_point[0] = pos;
                break;
            }

        } while (true);

        //点対称に配置
        team_first_point[1] = MirrorPoint(team_first_point[0]);

        field.clear();
        for (int i = 0; i < size.y(); i++) {
            field.push_back(QVector<GameSystem::MAP_OBJECT>(size.x()));
        }

        //ブロック配置
        for (int i = 0; i < block_num / 2; i++) {
            QPoint pos(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());

            auto mirrorPos = MirrorPoint(pos);

            if (CheckBlockRoleOld(pos) &&
                field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK &&
                pos != QPoint(size.x() / 2, size.y() / 2)) { //真ん中は必ずアイテム(ブロックは置けない)

                field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::BLOCK;

                //点対称に配置
                field[mirrorPos.y()][mirrorPos.x()] = GameSystem::MAP_OBJECT::BLOCK;
            }
            else {
                //ブロックが大会ルールに準拠してない場合は、もう一度乱数を回す
                i--;
                continue;
            }
        }

        //アイテム配置
        for (int i = 0; i < item_num / 2; i++) {
            QPoint pos(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());
            auto mirrorPos = MirrorPoint(pos);

            bool around_item_flag = true;

            //プレイヤーとアイテムを置こうとしている位置の一様ノルムが1以下なら、アイテムを置かない
            //(プレイヤーが初期位置でGetReadyしたときに、アイテムがない状態にする)
            //これは「A 基本タイプ」の要件
            if (UniformNorm(team_first_point[0] - pos) <= 1 || UniformNorm(team_first_point[1] - pos) <= 1)
                around_item_flag = false;

            if (around_item_flag &&
                pos != team_first_point[0] && pos != team_first_point[1] &&
                field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::ITEM &&
                field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK &&
                pos != QPoint(size.x() / 2, size.y() / 2)) { //真ん中は後の処理で必ずアイテムにするので、ここでは置かない

                field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::ITEM;
                //点対称に配置
                field[mirrorPos.y()][mirrorPos.x()] = GameSystem::MAP_OBJECT::ITEM;
            }
            else {
                i--;
                continue;
            }
        }
        // 真ん中は必ずアイテムにする
        field[size.y() / 2][size.x() / 2] = GameSystem::MAP_OBJECT::ITEM;
    }
    
}

void GameSystem::AroundData::finish(){
    this->data[0] = static_cast<GameSystem::MAP_OBJECT>(GameSystem::CONNECTING_STATUS::FINISHED);
    this->connect = GameSystem::CONNECTING_STATUS::FINISHED;
}

QString GameSystem::AroundData::toString(){
    //文字列へ変換
    QString str;
    str.append(static_cast<QChar>('0' + static_cast<int>(this->connect)));
    for(int i=0;i<9;i++){
        if     (this->data[i] == GameSystem::MAP_OBJECT::NOTHING)str.append('0');
        else if(this->data[i] == GameSystem::MAP_OBJECT::TARGET) str.append('1');
        else if(this->data[i] == GameSystem::MAP_OBJECT::BLOCK)  str.append('2');
        else if(this->data[i] == GameSystem::MAP_OBJECT::ITEM)   str.append('3');
    }
    return str;
}
