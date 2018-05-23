#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//※※※※※※※※※※※※※※※※※宿題※※※※※※※※※※※※※※※※※※※
//                      グローバル変数を無くす！！
//　　 なぜグローバル変数を無くすの？　=　グローバル変数はどこからでも変更可能なので、バグを起こしてしまったときにどこで修正していいかの目途が立ちにくい
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝マクロ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

#define WIDTH (19)                      //横幅.
#define HEIGHT (15)                     //縦幅.
#define NOWTIME (time(NULL))            //現在の経過時間（秒）.
#define TRUE (1)                        //真.
#define FALSE (0)                       //偽.
#define ROOTMAX (200)                   //保存する道順の最大量.
#define WALL (0)                        //壁の値.
#define ROOT (1)                        //通路の値.
#define DEBUG (1)                       //デバッグ用の定数（中身を1にすればデバッグモード、0だとデバッグモード解除）.

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝マクロここまで＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝


//＝＝＝＝＝＝＝＝＝＝＝＝＝＝グローバル変数＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝グローバル変数ここまで＝＝＝＝＝＝＝＝＝＝＝＝

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝プロトタイプ宣言＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
int DigMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y);
int CheckMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y);
void DrawMaze(int(*pMaze)[WIDTH], char * confirmStr, int *p_x, int *p_y);
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝プロトタイプ宣言ここまで＝＝＝＝＝＝＝＝＝＝＝＝

//main関数.
void main()
{
    int maze[HEIGHT][WIDTH];
    int result = FALSE;
    time_t updateTime = NOWTIME;
	int saveRoot[ROOTMAX], count = 0;
	int x = 1, y = 3;

    srand( time(0) );

    //外壁と基準の柱を作成.
    for( int heightIndex = 0 ; heightIndex < HEIGHT ; heightIndex++ )
    {
        for( int widthIndex = 0 ; widthIndex < WIDTH ; widthIndex++ )
        {
            maze[heightIndex][widthIndex] = WALL;           
        }
    }

	printf( "\n&saveRoot[ROOTMAX] = %d\n" , &saveRoot[ROOTMAX]);
	printf( "&maze               = %d\n", maze);
	printf( "↑2つのアドレスをよく見て！！！！！！！！！！！！！！\n" );
	system("pause");

    //描画.
	DrawMaze(maze, "初期状態確認", &x, &y);

    //無限ループ(この中で穴掘りをしていく).
    while( 1 )
    {
        if( updateTime != NOWTIME )
        {
            updateTime = NOWTIME;
			
			// before begin ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			// result = CheckMaze(maze, &saveRoot[ROOTMAX], &count, &x, &y);
			//DigMaze(maze, &saveRoot[ROOTMAX], &count, &x, &y);
            // before end\  ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			
			// after begin  ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			result = CheckMaze(maze, saveRoot, &count, &x, &y);
			DigMaze(maze, saveRoot, &count, &x, &y);
			// after end    ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			
			if( result == FALSE ){ break; };
            system("cls");
			DrawMaze(maze, "穴掘り中", &x, &y);
            //デバッグモード＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝.
            if( DEBUG )
            {
                printf( "x = %d y = %d\n\n" , x , y );
                printf( "count = %d\n\n" , count );
                printf( "updateTIme = %lld" , updateTime );
            }
            //デバッグモードここまで＝＝＝＝＝＝＝＝＝＝＝＝.
        }

    }

    system("cls");

    //描画.
	DrawMaze(maze, "穴掘り終了", &x, &y);
}

//穴掘り関数、この関数で掘り進めていく.
int DigMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y)
{
    int direction = (rand() % 4) + 1;               //方向を決める.
    int digResult = FALSE;                          //結果初期値（最初は偽）.
    switch (direction)
    {
    default:
        break;
    case 1:
        if ((*p_y) - 2 >= 1 && pMaze[(*p_y) - 2][*p_x] == WALL)
        {
            pMaze[*p_y - 2][*p_x] = ROOT;
            pMaze[*p_y - 1][*p_x] = ROOT;
            (*p_y) -= 2;
            digResult = TRUE;
        }
        break;
    case 2:
        if ((*p_y) + 2 < HEIGHT - 1 && pMaze[*p_y + 2][*p_x] == WALL)
        {
            pMaze[*p_y + 2][*p_x] = ROOT;
            pMaze[*p_y + 1][*p_x] = ROOT;
            (*p_y) += 2;
            digResult = TRUE;
        }
        break;
    case 3:
        if ((*p_x) - 2 >= 1 && pMaze[*p_y][(*p_x) - 2] == WALL)
        {
            pMaze[*p_y][(*p_x) - 2] = ROOT;
            pMaze[*p_y][(*p_x) - 1] = ROOT;
            (*p_x) -= 2;
            digResult = TRUE;
        }
        break;
    case 4:
        if (*p_x + 2 < WIDTH - 1 && pMaze[*p_y][*p_x + 2] == WALL)
        {
            pMaze[*p_y][(*p_x) + 2] = ROOT;
            pMaze[*p_y][(*p_x) + 1] = ROOT;
            (*p_x) += 2;
            digResult = TRUE;
        }
        break;
    }
    //穴を掘れたという判定があった時に進んだ方向を記憶させる(後々掘れなくなった時に前の場所に帰るため).
    if( digResult )
    {
        *(p_saveRoot + *p_count) = direction;
        (*p_count)++;
    }
    return digResult;
}

//穴を掘り進めれるかチェックする関数（行き止まりになったら掘れるところまでもどる、戻る場所がなくなれば終了判定を返す）.
int CheckMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y)
{
    int checkFlg = FALSE;
    
    if (*p_y - 2 >= 1 && pMaze[*p_y - 2][*p_x] == WALL){ checkFlg = TRUE; }
    if (*p_y + 2 < HEIGHT - 1 && pMaze[*p_y + 2][*p_x] == WALL){ checkFlg = TRUE; }
    if (*p_x - 2 >= 1 && pMaze[*p_y][*p_x - 2] == WALL){ checkFlg = TRUE; }
    if (*p_x + 2 < WIDTH - 1 && pMaze[*p_y][*p_x + 2] == WALL){ checkFlg = TRUE; }

    if( checkFlg == FALSE )
    {
        (*p_count)--;
        if( *p_count < 0 ){ return FALSE; }
        switch( *(p_saveRoot + *p_count) )
        {
        case 1:
                *p_y += 2;
            break;
        case 2:
                *p_y -= 2;
            break;
        case 3:
                *p_x += 2;
            break;
        case 4:
                *p_x -= 2;
            break;
        }
        *(p_saveRoot + *p_count) = 0;
    }

    return TRUE;
}

//現在地、通路、壁を表示するプログラム.
void DrawMaze(int(*pMaze)[WIDTH], char * confirmStr, int *p_x, int *p_y)
{
    printf( "\n\n＝＝＝＝＝＝＝＝＝%s＝＝＝＝＝＝＝＝＝\n\n" , confirmStr );
    for( int heightIndex = 0 ; heightIndex < HEIGHT ; heightIndex++ , pMaze++ )
    {
        for( int widthIndex = 0 ; widthIndex < WIDTH ; widthIndex++ )
        {
            //現在地は☆で表示する、そうでなければ壁か通路を表示してやる.
            if( widthIndex == *p_x && heightIndex == *p_y )
            {
                printf("☆");
            }
            else if( (*pMaze)[widthIndex] == 0 )
            {
                printf( "■" );
            }
            else
            {
                printf( "□" );                
            }
        }
        printf( "\n" );
    }
    printf( "\n\n＝＝＝＝＝＝＝＝＝%s＝＝＝＝＝＝＝＝＝\n\n" , confirmStr );
}
