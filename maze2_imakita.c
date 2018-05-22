#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//�����������������������������������h�聦������������������������������������
//                      �O���[�o���ϐ��𖳂����I�I
//�@�@ �Ȃ��O���[�o���ϐ��𖳂����́H�@=�@�O���[�o���ϐ��͂ǂ�����ł��ύX�\�Ȃ̂ŁA�o�O���N�����Ă��܂����Ƃ��ɂǂ��ŏC�����Ă������̖ړr�������ɂ���
//����������������������������������������������������������������������������

//�����������������������������}�N����������������������������������

#define WIDTH (19)                      //����.
#define HEIGHT (15)                     //�c��.
#define NOWTIME (time(NULL))            //���݂̌o�ߎ��ԁi�b�j.
#define TRUE (1)                        //�^.
#define FALSE (0)                       //�U.
#define ROOTMAX (200)                   //�ۑ����铹���̍ő��.
#define WALL (0)                        //�ǂ̒l.
#define ROOT (1)                        //�ʘH�̒l.
#define DEBUG (1)                       //�f�o�b�O�p�̒萔�i���g��1�ɂ���΃f�o�b�O���[�h�A0���ƃf�o�b�O���[�h�����j.

//�����������������������������}�N�������܂Ł�������������������������������


//�����������������������������O���[�o���ϐ���������������������������������

//�����������������������������O���[�o���ϐ������܂Ł�����������������������

//�����������������������������v���g�^�C�v�錾��������������������������������
int DigMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y);
int CheckMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y);
void DrawMaze(int(*pMaze)[WIDTH], char * confirmStr, int *p_x, int *p_y);
//�����������������������������v���g�^�C�v�錾�����܂Ł�����������������������

//main�֐�.
void main()
{
    int maze[HEIGHT][WIDTH];
    int result = FALSE;
    time_t updateTime = NOWTIME;
	int saveRoot[ROOTMAX], count = 0;
	int x = 1, y = 3;

    srand( time(0) );

    //�O�ǂƊ�̒����쐬.
    for( int heightIndex = 0 ; heightIndex < HEIGHT ; heightIndex++ )
    {
        for( int widthIndex = 0 ; widthIndex < WIDTH ; widthIndex++ )
        {
            maze[heightIndex][widthIndex] = WALL;           
        }
    }

	printf( "\n&saveRoot[ROOTMAX] = %d\n" , &saveRoot[ROOTMAX]);
	printf( "&maze               = %d\n", maze);
	printf( "��2�̃A�h���X���悭���āI�I�I�I�I�I�I�I�I�I�I�I�I�I\n" );
	system("pause");

    //�`��.
	DrawMaze(maze, "������Ԋm�F", &x, &y);

    //�������[�v(���̒��Ō��@������Ă���).
    while( 1 )
    {
        if( updateTime != NOWTIME )
        {
            updateTime = NOWTIME;
			
			// before begin ������������������������������������������������������������������������
			// result = CheckMaze(maze, &saveRoot[ROOTMAX], &count, &x, &y);
			//DigMaze(maze, &saveRoot[ROOTMAX], &count, &x, &y);
            // before end\  ������������������������������������������������������������������������
			
			// after begin  ������������������������������������������������������������������������
			result = CheckMaze(maze, saveRoot, &count, &x, &y);
			DigMaze(maze, saveRoot, &count, &x, &y);
			// after end    ������������������������������������������������������������������������
			
			if( result == FALSE ){ break; };
            system("cls");
			DrawMaze(maze, "���@�蒆", &x, &y);
            //�f�o�b�O���[�h��������������������������������.
            if( DEBUG )
            {
                printf( "x = %d y = %d\n\n" , x , y );
                printf( "count = %d\n\n" , count );
                printf( "updateTIme = %lld" , updateTime );
            }
            //�f�o�b�O���[�h�����܂Ł�����������������������.
        }

    }

    system("cls");

    //�`��.
	DrawMaze(maze, "���@��I��", &x, &y);
}

//���@��֐��A���̊֐��Ō@��i�߂Ă���.
int DigMaze(int(*pMaze)[WIDTH], int *p_saveRoot, int *p_count, int *p_x, int *p_y)
{
    int direction = (rand() % 4) + 1;               //���������߂�.
    int digResult = FALSE;                          //���ʏ����l�i�ŏ��͋U�j.
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
    //�����@�ꂽ�Ƃ������肪���������ɐi�񂾕������L��������(��X�@��Ȃ��Ȃ������ɑO�̏ꏊ�ɋA�邽��).
    if( digResult )
    {
        *(p_saveRoot + *p_count) = direction;
        (*p_count)++;
    }
    return digResult;
}

//�����@��i�߂�邩�`�F�b�N����֐��i�s���~�܂�ɂȂ�����@���Ƃ���܂ł��ǂ�A�߂�ꏊ���Ȃ��Ȃ�ΏI�������Ԃ��j.
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

//���ݒn�A�ʘH�A�ǂ�\������v���O����.
void DrawMaze(int(*pMaze)[WIDTH], char * confirmStr, int *p_x, int *p_y)
{
    printf( "\n\n������������������%s������������������\n\n" , confirmStr );
    for( int heightIndex = 0 ; heightIndex < HEIGHT ; heightIndex++ , pMaze++ )
    {
        for( int widthIndex = 0 ; widthIndex < WIDTH ; widthIndex++ )
        {
            //���ݒn�́��ŕ\������A�����łȂ���Εǂ��ʘH��\�����Ă��.
            if( widthIndex == *p_x && heightIndex == *p_y )
            {
                printf("��");
            }
            else if( (*pMaze)[widthIndex] == 0 )
            {
                printf( "��" );
            }
            else
            {
                printf( "��" );                
            }
        }
        printf( "\n" );
    }
    printf( "\n\n������������������%s������������������\n\n" , confirmStr );
}
