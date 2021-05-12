#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

//基礎ステータス値の設定

#define playerStartHP 1500
#define monster1startHP 300
#define monster2startHP 500
#define bossMonsterStertHP 1500

#define playerAttackDamage 200
#define monster1AttackDamage 200
#define monster2AttackDamage 150
#define bossMonsterAttackDamage 300

#define monster1SubjectDamage 50 
#define monster2SubjectDamage 30
#define bossMonsterSubjectDamage 100

#define potion 500

#define win 1
#define lose 2

int monster1HP = monster1startHP;
int monster2HP = monster2startHP;
int bossMonsterHP = bossMonsterStertHP;


//プレイヤーのステータス・状態を設定する構造体
typedef struct players{

    int life;       //HPの値
    int atk;        //攻撃力の値
    bool skill[6];  //後に明記するスキルらの所持を確認する
    int inRoom;     //どの部屋にいるか
    int hasItem;    //回復アイテムを持っている数
}player;

//部屋の情報を設定する構造体
typedef struct rooms{

    bool inMonster;      //モンスターがいるか
    bool inItem;         //アイテムが落ちているか
    bool inPlayer;       //プレイヤーがいるか
    bool isBossMonster;  //ボスモンスターがいるか
    struct rooms *up;    //現在の部屋から上方向に隣接する部屋のアドレス
    struct rooms *down;  //現在の部屋から下方向に隣接する部屋のアドレス
    struct rooms *right; //現在の部屋から右方向に隣接する部屋のアドレス
    struct rooms *left;  //現在の部屋から左方向に隣接する部屋のアドレス
    int num;             //部屋が個別で持つ値
    bool visited ;       //プレイヤーがすでに訪れているかを確認する

}room;

//モンスターのステータスを設定する構造体
typedef struct monsters{
    int life; //HPの値
    int atk;  //攻撃力の値
    int def;  //防御力の値
}monster;

room Dungeon[25];     //プレイヤーが移動するダンジョンの配列
room *stayRoom;       //プレイヤーが現在位置する部屋のアドレスを格納する
player users;         //プレイヤーのオブジェクト
monster monsterType1; 
monster monsterType2;
monster bossMonster;
bool winFalg;         //クリア条件を達成した際に真になるフラグ
bool loseFlag;        //ゲームオーバーになった際に真になるフラグ
int monsterKillCount; //モンスターを倒した数を数える変数

void shuffle(int array[],int size);//部屋割り用の関数
int randDamage(void);              //ダメージの乱数を決める関数
void startGame(void);              //ゲーム開始時の処理
void dungeonStartSet(void);        //ダンジョンの初期化
void playerStartSet(void);         //プレイヤー情報の初期化
void monsterStartSet(void);        //モンスター情報の初期化
void gameEndStartSet(void);        //終了条件の初期化
void printGameStart(void);         //スタート画面の表示
void winEnd(void);                 //勝利時の終了処理
void loseEnd(void);                //敗北時の終了処理
void openRule(void);               //ゲームルールの表示
void printMap(void);               //ダンジョン全体のマップ表示
void printRoom(void);              //現在位置する部屋の情報の表示
void moveRoom();                   //選択した部屋への移動

void dungeonlink();                //ダンジョンの構造が5×5になるようにアドレスを設定
void startRoomSet();               //スタート地点の部屋の情報の初期化
void monsterRoomSet(int num);      //モンスターがいる部屋の情報の初期化
void bossRoomSet(int getInt);      //ボスモンスターがいる部屋の情報の初期化
void itemRoomSet(int num);         //アイテムが落ちている部屋の情報の初期化


void checkRoomInfo(struct rooms *check); //部屋の情報を表示する
void fightMode(struct monsters *enemy);  //通常モンスターとの戦闘時に使用する関数
void bossFightMode();                    //ボスモンスターとの戦闘時に使用する関数
void fightersInfo();                     //プレイヤーとモンスターのステータスを表示する
int atackAction(int def);                //プレイヤーの攻撃アクション(モンスターに与えるダメージ値の決定)
void itemAction();                       //プレイヤーのアイテムアクション
int monsterAction(int atk);              //モンスターの攻撃アクション(プレイヤーに与えるダメージ値の決定)
void getItem();                          //アイテム入手を処理する関数
void getSkill();                         //スキル入手を処理する関数           
int roomNum(int x,int y);                //マップ表示時に使用する関数



int main(void){

    stayRoom = &Dungeon[0];
    char command,userSelect;
    int battleResult;
    int i=0;
    srand((unsigned)time(NULL));
    startGame();

    while (1)
    {
        printMap();//ダンジョンのマップを表示
        printRoom();//部屋内の情報を表示
        checkRoomInfo(stayRoom);//モンスターがいるかアイテムがあるかを判別する
                        //モンスターの場合バトルに突入
                        //ボスを倒すか戦闘で負けるかでフラグを建てる
        //ゲーム終了条件を満たしたらループを抜ける
        if(winFalg || loseFlag) break;
        moveRoom();//次の部屋を選択して移動
        stayRoom->visited =true;//一度通った部屋を表示するようにする

    }

    if(winFalg){
        printf("---------------------------------------\n");
        printf("             You Win!!!!!            \n");
        printf("          Congratulation!!!!\n");
        printf("---------------------------------------\n");
    }else if(loseFlag){
        printf("---------------------------------------\n");
        printf("             You Lose!!!!!            \n");
        printf("          Congratulation!!!!\n");
        printf("---------------------------------------\n");
    }else{
        printf("------------------------------------------------------------------\n");
        printf("             ちょっと待ってお前どうやって死んだん？？？？            \n");
        printf("------------------------------------------------------------------\n");
    }
    





    return 0;
}

int randDamage(){//プレイヤー及びモンスターの攻撃時、攻撃力に上乗せするダメージの設定

    int randInt;
    randInt = rand()%10;

    if(randInt<2){
        return 100;
    }else if(randInt > 9){
        return -30;
    }else
    {
        return 0;
    }
}


void fightMode(struct monsters *enemy){
    monster fightMon;
    int monsterHp = enemy->life;
    char command,userSelect;
    int atkAdd = 0;
    
    //モンスターを4体以上倒した際、次のバトルから通常モンスターを強化
    if(monsterKillCount > 3 && stayRoom->inMonster){
        monsterHp += 500; //HPを500アップ
        atkAdd = 40;      //攻撃力を40アップ
        
    }

    fightMon.atk = enemy->atk + atkAdd;
    fightMon.def = 2 * enemy->def; //防御力を二倍

    printf("戦闘開始！！\n\n");
    while(1){
        fightersInfo(monsterHp, fightMon);
        printf("あなたのターンです.\n");
        printf("行動を選択してください.\n");
        printf("1.攻撃\n");
        printf("2.アイテム\n");
        while(1)
        {
           command = getchar();
            scanf("%*c");
            if(command == '1'){
                monsterHp -= (atackAction(fightMon.def));
                break;
            }else if(command == '2'){
                itemAction();
                break;
            }else{
                printf("ちゃんと選択肢を選んでください\n");
            }
        }
        if(monsterHp <= 0){
            if(stayRoom->inMonster){
                printf("---------------------------------------\n");
                printf("     戦闘に勝利した！！\n");
                printf("---------------------------------------\n");
                getSkill();
                monsterKillCount++;
                printMap();
                stayRoom->inMonster = false;
                break;
            }else if(stayRoom->isBossMonster){
                printf("---------------------------------------\n");
                printf("     プレイヤーの追い打ち！！\n");
                printf("     プレイヤー「汚物は消毒だ！！！！！」\n");
                printf("     モンスターに5000兆ダメージ！！！！\n");
                printf("---------------------------------------\n\n");
                printf("---------------------------------------\n");
                printf("     ボスをやっつけた！！！！！\n");
                printf("---------------------------------------\n\n");
                printf("---------------------------------------\n");
                printf("     部屋の奥には宝の山が！！！！！\n");
                printf("---------------------------------------\n");
                winFalg = true;
                break;
            }
        }
        fightersInfo(monsterHp, fightMon);
        printf("モンスターのターン!\n");

        users.life -= monsterAction(fightMon.atk);

        if(users.skill[2] && playerStartHP == users.life){
            printf("---------------------------------------\n");
            printf("     回復の加護発動！！！！！\n");
            printf("     HPを200回復！！\n");
            printf("---------------------------------------\n");
            users.life += 200;
        }

        
        if(users.life <= 0){
            printf("---------------------------------------\n");
            printf("     戦闘に敗北した……\n");
            printf("---------------------------------------\n");
            loseFlag = true;
            break;
        }

    }

}

void fightersInfo(int monsterHp ,struct monsters enemy){
    printf("------------------------------------------\n");
    printf("     プレイヤーのHP:%d\n",users.life);
    printf("     プレイヤーの攻撃力:%d\n" , users.atk);
    printf("     モンスターのHP:%d\n",monsterHp);
    printf("     モンスターの攻撃力:%d\n",enemy.atk);
    printf("     モンスターの防御力:%d\n",enemy.def);
    printf("------------------------------------------\n\n");
}

int atackAction(int def){
    int res = (users.atk + randDamage() - def);
    if(users.skill[0]){
        printf("---------------------------------------\n");
        printf("     怪力の加護発動！！！！！\n");
        printf("     攻撃力を100上昇！！\n");
        printf("---------------------------------------\n");
        res += 100;
    }
    if(users.skill[3]){
        printf("---------------------------------------\n");
        printf("     粉砕の加護発動！！！！！\n");
        printf("     防御無視の攻撃！！\n");
        printf("---------------------------------------\n");
        res += def;
    }
    if(users.skill[4] && stayRoom->isBossMonster){
        printf("---------------------------------------\n");
        printf("     「蟷螂の斧」発動！！！！！\n");
        printf("     ボスへの攻撃力を上昇！！\n");
        printf("---------------------------------------\n");
        res *= 2;
    }
    if(users.skill[5] && stayRoom->inMonster){
        printf("---------------------------------------\n");
        printf("     雑魚ボコ殴り発動！！！！！\n");
        printf("     モンスターへの攻撃力を上昇！！\n");
        printf("---------------------------------------\n");
        res += 200;
    }
    printf("------------------------------------------\n");
    printf("     プレイヤーの攻撃！\n");
    printf("     相手に%dのダメージ！！！\n" , res);
    printf("------------------------------------------\n");
    return res;
}

void itemAction(){
    int randInt =rand()%20 + 10;
    if(users.hasItem >0){
        users.life += (potion + randInt);
        printf("------------------------------------------\n");
        printf("     ポーションを使って回復した！\n");
        printf("     プレイヤーは%d回復した\n",potion + randInt);
        printf("------------------------------------------\n");
    }else{
        printf("------------------------------------------\n");
        printf("     ポーションがもうない！\n");
        printf("------------------------------------------\n");
    }
}

int monsterAction(int atk){
    int res = atk + randDamage();

    printf("------------------------------------------\n");
    printf("     モンスターの攻撃！\n");
    if(users.skill[1]){
        printf("     耐久の加護発動！！！！！\n");
        printf("     防御力を100上昇！！\n");
        res -= 100;
    }
    printf("     相手に%dのダメージ！！！\n" , res);
    printf("------------------------------------------\n");
    return res; 
}

void getSkill(){//通常モンスター討伐時、確率でスキル(全6種)を獲得する
    int randInt = rand()%2;
    if(randInt == 0){
        randInt = rand()%6;
        if(!users.skill[randInt]){
            printf("---------------------------------------\n");
            printf("     スキルを入手した！！\n");
            printf("---------------------------------------\n");
            switch (randInt)
            {
            case 0:
                users.skill[0] = true;
                printf("---------------------------------------\n");
                printf("     怪力の加護を取得！！\n");
                printf("---------------------------------------\n");
                break;
            case 1:
                users.skill[1] = true;
                printf("---------------------------------------\n");
                printf("     耐久の加護を取得！！\n");
                printf("---------------------------------------\n");
                break;
            case 2:
                users.skill[2] = true;
                printf("---------------------------------------\n");
                printf("     回復の加護を取得！！\n");
                printf("---------------------------------------\n");
                break;
            case 3:
                users.skill[3] = true;
                printf("---------------------------------------\n");
                printf("     粉砕の加護を取得！！\n");
                printf("---------------------------------------\n");
                break;
            case 4:
                users.skill[4] = true;
                printf("---------------------------------------\n");
                printf("     「蟷螂の斧」を取得！！\n");
                printf("---------------------------------------\n");
                break;
            case 5:
                users.skill[5] = true;
                printf("---------------------------------------\n");
                printf("     雑魚ボコ殴りを取得！！\n");
                printf("---------------------------------------\n");
                break;
            default:
                break;
            }
        }
    }
}

void startGame(){
    dungeonStartSet();
    dungeonlink();
    playerStartSet();
    monsterStartSet();
    gameEndStartSet();
    printGameStart();
}

void dungeonStartSet(){
    int randInt;
    int list[23];       //部屋の種類をランダムに割り振るための配列
    monsterKillCount =0;//モンスターの討伐数の初期化

                              //ダンジョン内の到達ポイントの初期化
    Dungeon[0].visited = true;//スタート地点を除き未到達状態に更新
    for(int i=1;i<24;i++){
        Dungeon[i].visited = false;
    }
    
    for(int i=1;i<=23;i++){//1~23の整数を順番に格納
        list[i-1] = i;
    }

    randInt = 12 + rand()%12;//ボス部屋はスタート地点から離れた位置に置くように設定
    startRoomSet();//スタート地点の情報初期化
    bossRoomSet(randInt);//ボス部屋の情報初期化
    shuffle(list,23); //配列の各部屋の値をランダムに入れ替える
    for(int i=0;i<23;i++){  
        if(randInt != (i+1)){
            Dungeon[i+1].num = list[i];
        }
    }

    for(int i=1;i<=24;i++){
        if(randInt != i){
            if(Dungeon[i].num <13){
                monsterRoomSet(i);
            }else{
                itemRoomSet(i);
            }
        }
    }
}

void dungeonlink(){
    Dungeon[0].up = NULL;
    Dungeon[0].left = NULL;
    Dungeon[0].right = &Dungeon[1];
    Dungeon[0].down = &Dungeon[5];

    Dungeon[1].up = NULL;
    Dungeon[1].left = &Dungeon[0];
    Dungeon[1].right = &Dungeon[2];
    Dungeon[1].down = &Dungeon[6];
    
    Dungeon[2].up = NULL;
    Dungeon[2].left = &Dungeon[1];
    Dungeon[2].right = &Dungeon[3];
    Dungeon[2].down = &Dungeon[7];
    
    Dungeon[3].up = NULL;
    Dungeon[3].left = &Dungeon[2];
    Dungeon[3].right = &Dungeon[4];
    Dungeon[3].down = &Dungeon[8];
    
    Dungeon[4].up = NULL;
    Dungeon[4].left = &Dungeon[3];
    Dungeon[4].right = NULL;
    Dungeon[4].down = &Dungeon[9];
    
    Dungeon[5].up = &Dungeon[0];
    Dungeon[5].left = NULL;
    Dungeon[5].right = &Dungeon[6];
    Dungeon[5].down = &Dungeon[10];
    
    Dungeon[6].up = &Dungeon[1];
    Dungeon[6].left = &Dungeon[5];
    Dungeon[6].right = &Dungeon[7];
    Dungeon[6].down = &Dungeon[11];
    
    Dungeon[7].up = &Dungeon[2];
    Dungeon[7].left = &Dungeon[6];
    Dungeon[7].right = &Dungeon[8];
    Dungeon[7].down = &Dungeon[12];
    
    Dungeon[8].up = &Dungeon[3];
    Dungeon[8].left = &Dungeon[7];
    Dungeon[8].right = &Dungeon[9];
    Dungeon[8].down = &Dungeon[13];
    
    Dungeon[9].up = &Dungeon[4];
    Dungeon[9].left = &Dungeon[8];
    Dungeon[9].right = NULL;
    Dungeon[9].down = &Dungeon[14];

    Dungeon[10].up = &Dungeon[5];
    Dungeon[10].left = NULL;
    Dungeon[10].right = &Dungeon[11];
    Dungeon[10].down = &Dungeon[15];

    Dungeon[11].up = &Dungeon[6];
    Dungeon[11].left = &Dungeon[10];
    Dungeon[11].right = &Dungeon[12];
    Dungeon[11].down = &Dungeon[16];
    
    Dungeon[12].up = &Dungeon[7];
    Dungeon[12].left = &Dungeon[11];
    Dungeon[12].right = &Dungeon[13];
    Dungeon[12].down = &Dungeon[17];

    Dungeon[13].up = &Dungeon[8];
    Dungeon[13].left = &Dungeon[12];
    Dungeon[13].right = &Dungeon[14];
    Dungeon[13].down = &Dungeon[18];

    Dungeon[14].up = &Dungeon[9];
    Dungeon[14].left = &Dungeon[13];
    Dungeon[14].right = NULL;
    Dungeon[14].down = &Dungeon[19];

    Dungeon[15].up = &Dungeon[10];
    Dungeon[15].left = NULL;
    Dungeon[15].right = &Dungeon[16];
    Dungeon[15].down = &Dungeon[20];

    Dungeon[16].up = &Dungeon[11];
    Dungeon[16].left = &Dungeon[15];
    Dungeon[16].right = &Dungeon[17];
    Dungeon[16].down = &Dungeon[21];
    
    Dungeon[17].up = &Dungeon[12];
    Dungeon[17].left = &Dungeon[16];
    Dungeon[17].right = &Dungeon[18];
    Dungeon[17].down = &Dungeon[22];

    Dungeon[18].up = &Dungeon[13];
    Dungeon[18].left = &Dungeon[17];
    Dungeon[18].right = &Dungeon[19];
    Dungeon[18].down = &Dungeon[23];

    Dungeon[19].up = &Dungeon[14];
    Dungeon[19].left = &Dungeon[18];
    Dungeon[19].right = NULL;
    Dungeon[19].down = &Dungeon[24];

    Dungeon[20].up = &Dungeon[15];
    Dungeon[20].left = NULL;
    Dungeon[20].right = &Dungeon[21];
    Dungeon[20].down = NULL;

    Dungeon[21].up = &Dungeon[16];
    Dungeon[21].left = &Dungeon[20];
    Dungeon[21].right = &Dungeon[22];
    Dungeon[21].down = NULL;
    
    Dungeon[22].up = &Dungeon[17];
    Dungeon[22].left = &Dungeon[21];
    Dungeon[22].right = &Dungeon[23];
    Dungeon[22].down = NULL;

    Dungeon[23].up = &Dungeon[18];
    Dungeon[23].left = &Dungeon[22];
    Dungeon[23].right = &Dungeon[24];
    Dungeon[23].down = NULL;
    
    Dungeon[24].up = &Dungeon[19];
    Dungeon[24].left = &Dungeon[23];
    Dungeon[24].right = NULL;
    Dungeon[24].down = NULL;

}

void shuffle(int array[],int size){
    for(int i=0;i<size;i++){
        int j= rand()%size;
        int t = array[i];
        array[i] = array[j];
        array[j] = t;
    }
}

void startRoomSet(){
    Dungeon[0].inItem = false;
    Dungeon[0].inMonster = false;
    Dungeon[0].inPlayer = true;
    Dungeon[0].isBossMonster = false;
    Dungeon[0].num = 0;   
}

void bossRoomSet(int getInt){
    Dungeon[getInt].inItem = false;
    Dungeon[getInt].inMonster = false;
    Dungeon[getInt].isBossMonster = true;
    Dungeon[getInt].inPlayer = false;
    Dungeon[getInt].num = 24;
}

void monsterRoomSet(int num){
    Dungeon[num].inMonster = true;
    Dungeon[num].inItem = false;
    Dungeon[num].inPlayer = false;
    Dungeon[num].isBossMonster = false;
}

void itemRoomSet(int num){
    Dungeon[num].inMonster = false;
    Dungeon[num].inItem = true;
    Dungeon[num].isBossMonster = false;
    Dungeon[num].inPlayer = false;
}

void playerStartSet(){
    
    users.life = playerStartHP;
    users.atk = playerAttackDamage;
    users.inRoom = 0;
    users.hasItem = 3;
    for(int i=0;i<6;i++){
        users.skill[i] = false;
    }
}
 
void monsterStartSet(){
    monsterType1.life = monster1startHP;
    monsterType1.atk = monster1AttackDamage;
    monsterType1.def = monster1SubjectDamage;
    
    monsterType2.life = monster2startHP;
    monsterType2.atk = monster2AttackDamage;
    monsterType2.def = monster2SubjectDamage;

    bossMonster.life = bossMonsterStertHP;
    bossMonster.atk = bossMonsterAttackDamage;
    bossMonster.def = bossMonsterSubjectDamage;

}

void printGameStart(){
       char c , usersAction;

    printf("****************************************\n");
	printf("*                                      *\n");
	printf("*            モンスターラッシュ         *\n");
	printf("*                                      *\n");
	printf("*       テキスト形式型ダンジョンRPG     *\n");
	printf("*                                      *\n");
	printf("*              製作：大村雄飛           *\n");
	printf("*                                      *\n");
	printf("****************************************\n");

    printf("ルール・操作方法を見る：1\n");
    printf("ゲームスタート：2\n");
    printf("どちらかを選択してEnterキーを押してください\n");

    c=getchar();
    usersAction = c;
    while((c = getchar()) != '\n');
        if(usersAction == '1'){
            printf("     ルール説明\n");
            printf("=====================\n");
            printf("このゲームはテキストベース型のRPGゲームです");
            printf("テキストの表示に従ってください\n");
            printf("テキストが指示していないことをしないでください\n");

            printf("\nいずれかのキーを入力してね\n");
            c = getchar();
        }
    
    printf("ゲームスタート！\n");
}

void printMap(){
    int x = 0,y = 0;

    for(y = 0;y<5;y++){
        //1列目
        for(x=0;x<5;x++){
            if(Dungeon[roomNum(x,y)].visited){
                if(Dungeon[roomNum(x,y)].up == NULL){
                    printf("*******");
                }else{
                    printf("***D***");
                }
            }else{
                printf("       ");
            }
        }
        printf("\n");
        //2列目
        for(x=0;x<5;x++){
            if(Dungeon[roomNum(x,y)].visited){
                printf("*     *");
            }else{
                printf("       ");
            }
        }
        printf("\n");

        //4列目
        for(x=0;x<5;x++){
            if(Dungeon[roomNum(x,y)].visited){
                if(Dungeon[roomNum(x,y)].left == NULL && Dungeon[roomNum(x,y)].right == NULL){
                    if(Dungeon[roomNum(x,y)].inPlayer){
                        printf("*  P  *");
                    }else{
                        printf("*     *");
                    }
                }else if(Dungeon[roomNum(x,y)].left ==NULL){
                    if(Dungeon[roomNum(x,y)].inPlayer){
                        printf("*  P  D");
                    }else{
                        printf("*     D");
                    }
                }else if(Dungeon[roomNum(x,y)].right == NULL){
                    if(Dungeon[roomNum(x,y)].inPlayer){
                        printf("D  P  *");
                    }else{
                        printf("D     *");
                    }
                }else{
                    if(Dungeon[roomNum(x,y)].inPlayer){
                        printf("D  P  D");
                    }else{
                        printf("D     D");
                    }
                }
            }else{
                printf("       ");
            }
        }
        printf("\n");

        //5列目
         for(x=0;x<5;x++){
            if(Dungeon[roomNum(x,y)].visited){
                printf("*     *");
            }else{
                printf("       ");
            }
        }
        printf("\n");


        //6列目
        for(x=0;x<5;x++){
            if(Dungeon[roomNum(x,y)].visited){
                if(Dungeon[roomNum(x,y)].down == NULL){
                    printf("*******");
                }else{
                    printf("***D***");
                }
            }else{
                printf("       ");
            }
        }
        printf("\n");
    }
    
}

void printRoom(){
    printf(" \n");
}


void moveRoom(){
    printf("\n");
    char c;
    bool moveClear = false;
    
    stayRoom->inPlayer =false;
    printf("Move Action\n\n");

    do
    {
        printf("行きたい方向を入力しよう, 左:a,上:w,右:d,下:s \n");
        
        printf("行ける方向 ");
        if(stayRoom->left != NULL)printf("左 ");
        if(stayRoom->down != NULL)printf("下 ");
        if(stayRoom->up != NULL)printf("上 ");
        if(stayRoom->right != NULL)printf("右 ");
        printf("\n");
        c = getchar();
        scanf("%*c");
        switch (c)
        {
        case 'a':
            if(stayRoom->left != NULL){
                stayRoom = stayRoom->left;
                stayRoom->inPlayer = true;
                moveClear = true;
                break;
            }
            
        case 'w':
            if(stayRoom->up != NULL){
                stayRoom = stayRoom->up;
                stayRoom->inPlayer = true;
                moveClear = true;
                break;
            }
        case 'd':
            if(stayRoom->right != NULL){
                stayRoom = stayRoom->right;
                stayRoom->inPlayer = true;
                moveClear = true;
                break;
            }
        case 's':
            if(stayRoom->down != NULL){
                stayRoom = stayRoom->down;
                stayRoom->inPlayer = true;
                moveClear = true;
                break;
            }
        default:
            
            printf("入力ミスです %c\n", c);
            
        }
    }while (moveClear == false);
    
}



void gameEndStartSet(){
    winFalg =false;
    loseFlag = false;
}

void checkRoomInfo(struct rooms *check){
    int randInt;
    if(check->inMonster){
        printf("モンスターが現れた！\n");
        randInt = rand()%2;
        if(randInt ==1){
            fightMode(&monsterType1);
        }else{
            fightMode(&monsterType2);
        }
    }else if(check->inItem)
    {
        printf("アイテムが落ちている…\n");
        printf("ポーションを手に入れた！！");
        getItem();
    }else if(check->isBossMonster)
    {
        printf("凶悪なモンスターだ！！！！\n");
        printf("全てをかけろ！！！\n");
        fightMode(&bossMonster);
    }else{
        printf("なにもないな\n");
    }
    
}

void getItem(){
    users.hasItem++;
    stayRoom->inItem = false;
}

int roomNum(int x,int y){
    int res;

    res = 5*y + x ;

    return res;
}