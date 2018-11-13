#include <stdio.h>  
#include <windows.h>  
#include <time.h>  
#include <conio.h>     
#include <iostream.h>

//-------------------------------------------------------------------
void GotoXY(int x, int y)      //�趨���λ��
{  
	COORD c;  
	c.X = x-1;  
	c.Y = y-1;  
	SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), c);  
} 
//-----------------------------------------------------------------
void color(int a)              //��ɫ����
{
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),a);
}
struct HeroList
{
	char Name[10];
	int Highscore;
}Hero[10];
//----------------------------------------------------------
void Save()                    //�洢��߷ּ�����
{
	FILE *fp;
	if((fp=fopen("Hero_list","wb"))==NULL)
	{cout<<"can not open file!"<<endl;return;}
	if(fwrite(&Hero[0],sizeof(struct HeroList),1,fp)!=1)
		cout<<"file write error!"<<endl;
	fclose(fp);
}
//**********************************************************
class CSnake
{
     int image[20][20];        //��Ϸ��壬��С�߻��Χ
     int FoodX;                //������ֵ�X����
     int FoodY;                //������ֵ�Y����
     int snakeX;               //��¼С�ߵ�ͷ��X����
     int snakeY;               //��¼С�ߵ�ͷ��Y����
     int head;                 //С��ͷ���±�
     int tail;                 //С��β���±�
     int snake[2][2000];         //��¼С���������������
     int node;                 //С�ߵĽ���
	 int direction;            //С��ͷ���ķ���        
     int i,j;   	
	 int gamespeed;            //�ٶ�
	 int score;                //��¼����
	 char arrowKey;            //���ܰ����ı���
	 int GameOver;             //ʹ��Ϸ�����ı���
	 int level;                //���õȼ�
	 int length;               //Ϊ�����õȼ�����nodeһ����¼��ĳ���
public:
	 
	CSnake()        //���캯����ʼ�����ֱ���
	{
        head=3;                //��ʼ��ͷ���±�
        tail=0;                //��ʼ��β���±�
        node=4;                //С�߳�ʼ����Ϊ4��
    	gamespeed=300;         //��Ϸ�ٶȣ��ƶ��ļ��ʱ�䣩
    	score=0;               //��ʼ������Ϊ0
        direction=6;           //С�ߵĳ�ʼ��������
		GameOver=0;            //������Ϸ��ʼ��Ϊ0
		level=1;
		length=4;
	}
	~CSnake()     //��������
	{}	
	void Init();                //��ʼ����Ϸ��弰С�ߵ�����
	int  DrawSnake();           //��¼С��ÿ���ƶ���ͷ�����������һ����
	void  Automove();            //ʹС���Զ��ƶ�
    int  Gameover();            //�ж���Ϸ�Ƿ����
	int  Generatefood();        //����ʳ��
	void display();             //��ʾ��Ϸ���
	void shiftLeft();           //��������
	void shiftRight();          //��������
	void shiftDown();           //��������
	void shiftUp();	            //��������
	int  PlayGame();            //ִ����Ϸ�ĺ���
	void Setspeed();            //�����ٶ�
	int Pause();                //��Ϸ����ͣ
};
//-------------------------------------- -------------------------------
 void CSnake::Init()             //��ʼ����������
{
	 
 for(i=0;i<20;i++)                   //��ʼ����Ϸ���
   for(j=0;j<20;j++)    
	image[i][j]=0;
 for(i=0;i<4;i++)                    //��ʼ��С�߳�ʼλ��
    { snake[0][i]=1;
      snake[1][i]=i+1;
    }
 for(i=1;i<=3;i++)                   //��ʼ��С���������ʾֵ(����ʼ��С����ʾͼ�ζ�Ӧ��ֵ)
        image[1][i]=3; 
   image[1][4]=1;
}
//---------------------------------------------------------------
int CSnake::Gameover()                //�ж���Ϸ�Ƿ����
{
  if(snakeX<0 || snakeX>19 ||snakeY<0 || snakeY>19)                       // ����ǽ����Ϸ����
  {  
	  GotoXY(1,25);   
      cout << "Game over!" << endl;
      GameOver=1;
     return GameOver;
  }
  if((image[snakeX][snakeY]!=0)&&!(snakeX==FoodX&&snakeY==FoodY))         //��ͷ����������Ϸ����
   { 
	  GotoXY(1,25); 
	  cout << "Game over!" << endl;
	  GameOver=1;
	  return GameOver;
  }
  return 0;
}
//----------------------------------------------------------------
int CSnake::DrawSnake()
{   if(snakeX==FoodX && snakeY==FoodY)                                  //��С��ͷ��������ʳ�������ص���Ե�ʳ��
     {                                
      node++;                                                           // С�߽�����һ
      score+=10;                                                        //��������  
	  length++;
	  if(length>=8)                                                     //��С��������8��ʱ���ȼ���һ
	  {length-=8;
	  level++;}
      image[snakeX][snakeY]=1;                                           //С��ͷ���ƶ�����һλ��ͷ��ͼ���Ӧ��ֵ����ͼ��
      image[snake[0][head]][snake[1][head]] = 3;                         //С��ͷ��������������һ���ƶ�����һλ��ͷ��ͼ���Ӧ��ֵ����ͼ��
      head=(head+1)%10000;
      snake[0][head] = snakeX;                                           //��С��ͷ����ʱ������ֵ������¼ͷ�������snake[0][head]
      snake[1][head] = snakeY;
      Generatefood();                                                    //�Ե�ʳ����ٴ�����ֲ�ʳ��
	  display();                                                         //ˢ����ʾ
       
      }
     else
     {                                                                    //û�г�ʳ��
      image[snake[0][tail]][snake[1][tail]]=0;                            //��ȥС��β��
      tail=(tail+1)%10000;                                                
      image[snake[0][head]][snake[1][head]]=3;                            //С��ͷ��������������һ���ƶ�����һλ��ͷ��ͼ���Ӧ��ֵ����ͼ��
      head=(head+1)%10000;
      snake[0][head]=snakeX;                                              //��С��ͷ����ʱ������ֵ������¼ͷ�������snake[0][head]
      snake[1][head]=snakeY;
      image[snake[0][head]][snake[1][head]]=1;                            //С��ͷ���ƶ�����һλ��ͷ��ͼ���Ӧ��ֵ����ͼ��
      display(); 
      }
 
  return 0;
}
//-----------------------------------------------------------
void CSnake::shiftLeft()                                                 //������С����ͷ��������
{	
    direction=4;       
}
//++++++++++++++++++++++++++
void CSnake::shiftRight()                                                //������С����ͷ��������
{
    direction=6;       
}
//+++++++++++++++++++++++++++
void CSnake::shiftUp()                                                    //������С����ͷ��������
{
    direction=8;       
}	
//+++++++++++++++++++++++++++
void CSnake::shiftDown()                                                  //������С����ͷ��������
{     
    direction=2;       
}
//------------------------------------------------------
void CSnake::Automove()
{
	switch(direction)
	{                                                                 
	case 6:snakeX= snake[0][head]; snakeY= snake[1][head]+1;break;          //���յ����ҵ���ϢС��ͷ��Y�����һ��X���겻�䲢������¼ͷ������ı���
	case 4:snakeX= snake[0][head]; snakeY= snake[1][head]-1;break;         //���յ��������ϢС��ͷ��Y�����һ��X���겻�䲢������¼ͷ������ı���
	case 8:snakeX= snake[0][head]-1; snakeY= snake[1][head];break;          //���յ����ϵ���ϢС��ͷ��X�����һ��Y���겻�䲢������¼ͷ������ı���
	case 2:snakeX= snake[0][head]+1; snakeY= snake[1][head];break;          //���յ����µ���ϢС��ͷ��X�����һ��Y���겻�䲢������¼ͷ������ı���
    default:;
	}
}
//--------------------------------------------------------------------- 
int CSnake::Generatefood()                                        // ����ʳ��
{  
	srand(time(0));                                               //��ʱ��Ϊ���������������
	do{ 
     FoodX=rand()%20;                                             //ʳ�������X����
     FoodY=rand()%20;                                             //ʳ�������Y����
	}while(image[FoodX][FoodY]!=0);                               //������ʳ�������޶�����Ϸ����ڣ���ʳ�����겻��С�����������غ�
    image[FoodX][FoodY]=2;
 return image[FoodX][FoodY];
}
//----------------------------------------------------------------
void CSnake::display()                                            // ��ʾ��Ϸ��壨����Χǽ��ʳ�ＰС�ߣ�
{  
	color(10);
    GotoXY(1,1);
    for(i=0;i<22;i++)  
    printf("��");  
    GotoXY(1,2); 
    for(i=0;i<20;i++)  
	{	color(10);
	printf("��");  
	for(j=0;j<20;j++)  
	{
		switch(image[i][j])  
		{
		case 0:color(15);printf("  ");break;  
		case 1:color(11);printf("��");break;
		case 2:color(12);printf("��");break; 
		case 3:color(13);printf("��");break;
		}
	}  color(10);
	printf("��\n");
}  
	color(10);
for(i=0;i<22;i++)  
printf("��");  color(13);
GotoXY(46,9); printf("SCORE:%d",score);         //��¼����
GotoXY(46,10); printf("LEVEL=%d",level);         //��¼�ȼ�
color(12);
GotoXY(46,5); printf("�����������ǳ���д��\n");
GotoXY(46,6); printf("��Ȩ���У���Ȩ�ؾ���");
}
//----------------------------------------------------
void CSnake::Setspeed()                           //����С���ƶ����ٶ�
{  
	int Esc=0; 
	while(!Esc)
	{
		int speed;
				color(12);
				cout<<"1.Very Slow   2.Slow   3.Normal   4.Fast   5.Very Fast "<<endl;
				cout<<"����������ѡ��";
				cin>>speed;
				switch(speed)
				{
				case 0:
					Esc=1;
					gamespeed=300;
					break;
				case 1:
					gamespeed=300;
					Esc=1;
					break;
				case 2:
					gamespeed=200;
					Esc=1;
					break;
				case 3:
					gamespeed=80;
					Esc=1;
					break;
			   case 4:
					gamespeed=30;
					Esc=1;
					break;
			  case 5:
					gamespeed=12;
					Esc=1;
					break;
				}
	}
}
//-----------------------------------------------------------
int CSnake::Pause()                   //��ͣ����
{
    char c;  
   // GotoXY(1,25);printf("Pause! ");  
    do  
	{c=getch();}  
	while(c!='p');  
	arrowKey=0;
	return arrowKey;
} 
//--------------------------------------------   
//**************************************************************
int CSnake::PlayGame()                                        //ִ����Ϸ����
 {
   Init();                                                    //��ʼ������
   Generatefood();                                            //����ʳ��
   display();                                                 //��ʾ��Ϸ���
	while (!GameOver) 		
    {		
        if(kbhit())                                            //���ܰ���
		{
          arrowKey = getch();
        }	
			if(direction==2||direction==8)                     //��С�ߵ�ͷ���ƶ�����Ϊ����ʱ�����Ҽ���������
			{
              switch(arrowKey)
              {
			  case'6':shiftRight();break;
			  case'4':shiftLeft();break;				  
			  case'e':exit(1);break;
			  case'p':Pause();break;
			  default:break;
               }		
			}
			else if(direction==4||direction==6)                 //��С�ߵ�ͷ���ƶ�����Ϊ����ʱ�����¼���������
			{
		      switch(arrowKey)                        
			  {
			  case'8':shiftUp();break;
			  case'5':shiftDown();break;				 
			  case'e':exit(1);break;				  
			  case'p':Pause();break;
			  default:break;
			  }				
			}   
			Sleep(gamespeed);                                 //С���ƶ�һ����ٶ�
			Automove();			                              // У���Զ��ƶ�
			Gameover();                                       //�ж���Ϸ�Ƿ����
			if(GameOver==1)                                   //����Ϸ��������ֱ������ѭ��
				break;
			DrawSnake();                                      //�ж��Ƿ��ʳ��
			display();                                        //��ʾ��Ϸ���
	}
	Hero[0].Highscore=score;
	getch();
	return 0;
}
//**************************************************************************
void Welcome(void)                                           //��ӭ����
{
	printf("                                                                                ");color(13);
	printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ");
	printf("                                                                                ");
	printf("                                                                                ");
	printf("                                                                                ");color(14);
	printf("                                                                                ");
	printf("  ����������    ������        ����      ������      ����       ��  ������������ ");
	printf("��������������  ��������      ����    ����������    ����      ��   ������������ ");
	printf("������  ������  ���� ����     ����   ����    ����   ����    ��     ����         ");
	printf("������          ����  ����    ����  ����      ����  ����  ��       ����         ");
	printf("  ��������      ����   ����   ����  ����      ����  ���� ��        ������������ ");
	printf("   ����������   ����    ����  ����  ��������������  ������         ������������ ");
	printf("        ������  ����     ���� ����  ��������������  ���� ��        ����         ");
	printf("������  ������  ����      ��������  ����      ����  ����   ��      ����         ");
	printf("��������������  ����        ������  ����      ����  ����     ��    ������������ ");
	printf("  ����������    ����          ����  ����      ����  ����      ���� ������������ ");
	printf("                                                                                ");
	printf("                                                                                ");color(13);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ");color(10);
	printf("��C++����̰������Ϸ�����ǳ���д!!! ��Ȩ���У���Ȩ�ؾ�!\n");
	printf("[���ƣ�8 �����ƣ�5 �����ƣ�4 �����ƣ�6 �� ��ͣ��P�� �˳���E]\n");								
	system("pause");
	system("cls");
}


//----------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
void main()                                        //������
{ 	
	CSnake SNAKE;
	char Name[10]=" ";
//	int Highscore=0;
	char ch;
	char Get=0;
//	int i;
	Welcome();
	while(1)
	{    
		color(14);                                                  // ������
		cout<<"     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^     "<<endl;
		cout<<"   -----* - * - * - * - * - * - *----   "<<endl;
		cout<<"   $   �x�x�x�x�x�x�x�x�x�x�x�x�x�x   $      "<<endl;
		cout<<"   $   ��                          �� $      "<<endl;
		cout<<"   $   ��       1.��ʼ��Ϸ         �� $      "<<endl;
		cout<<"   $   ��       2.�����ٶ�         �� $      "<<endl;
		cout<<"   $   ��       3.ʹ��˵��         �� $      "<<endl;
		cout<<"   $   ��       4.Ӣ�۰�           �� $      "<<endl;
		cout<<"   $   ��       5.�˳�             �� $      "<<endl;
		cout<<"   $   ��                          �� $      "<<endl;
		cout<<"   $   ����������������������������   $      "<<endl;
		cout<<"   -----* - * - * - * - * - * - *----   "<<endl;
		cout<<"     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^     "<<endl;
		cin>>ch;
		
		if(ch=='1')
		{	
			system("cls");
			SNAKE.PlayGame();
			system("cls");
			if(Hero[0].Highscore<100)
			{cout<<"������Ŭ��Ӵ!"<<endl;cout<<endl;}
			else if(400>Hero[0].Highscore&&Hero[0].Highscore>200)
			{cout<<"��һ���Ǹ���!"<<endl;cout<<endl;}
			else if(600>Hero[0].Highscore&&Hero[0].Highscore>400)
			{	cout<<"��ʵ��̫������!"<<endl;cout<<endl;}
			else
			{	cout<<"��һ���ǹǻҼ����!���!���!"<<endl;cout<<endl;}
			if(Hero[0].Highscore>200)
			{
			cout<<"���������Ĵ�����";
			cin>>Hero[0].Name;			
			Save();		
			}
			system("pause");
			system("cls");	
			break;
		}
		else if(ch=='2')
		{
			system("cls");
			SNAKE.Setspeed();
			system("pause");
			system("cls");
			continue;
		}
		else if(ch=='3')                                               //��ϸʹ��˵��
		{
			system("cls");
			cout<<"                                                    "<<endl;
			cout<<"                                                    "<<endl;color(14);
			cout<<"    *********************************************** "<<endl;color(12);
			cout<<"    *   ���ƣ�8 ;       ���ƣ�4 ;    ��ͣ��P      * "<<endl;
			cout<<"    *   ���ƣ�5 ;       ���ƣ�6 ;    �˳���E      * "<<endl;color(14); 
			cout<<"    *********************************************** "<<endl;color(12);
			cout<<"    $        ����Ӧ�ļ����Կ���С�ߵ��ƶ�!        $ "<<endl;
			cout<<"    $         ͨ������С�ߵ��ƶ�����ʳ�        $ "<<endl;
			cout<<"    $��С�ߵ�ͷ��ײ��ǽ�ڻ��Լ�������ʱ����Ϸ����!$ "<<endl;color(14);
			cout<<"    #-------------------------------------------- # "<<endl;
			cout<<"    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ "<<endl;
			cout<<"                                                    "<<endl;
			cout<<"                                                    "<<endl;
			system("pause");
			system("cls");
			continue;
		}
		else if(ch=='4')                                             // ��¼��߷֣���ʾӢ�۰�
		{
			system("cls");
			color(10);
			FILE *fp;
			fp=fopen("Hero_list","rb");
			fread(&Hero[0],sizeof(struct HeroList),1,fp);
			cout<<"$#######_---   Ӣ�۰�  ---_#######$"<<endl;
			cout<<"               ����:"<<Hero[0].Name<<endl;
			cout<<"               ��߷�:"<<Hero[0].Highscore<<endl;
			cout<<"--------------------------------------------"<<endl;
			fclose(fp);
			system("pause");
			system("cls");			
			continue;
		}
		else if(ch=='5')
		{
			system("cls");
			exit(0);
		}
		else
		{
			system("cls");
			cout<<"���������룡"<<endl;	
			continue;
		}
	}	
		
} 
	
	
