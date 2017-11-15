//����˹���鸴ԭ �ο����� ֪�� �Ӻ���ѧ
#include <easyx.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <iostream.h>



#define WIDTH  10//���
#define HEIGHI 22//�߶�
#define UNIT   20//��Ϸ����λ��ʵ������

//������� ö������
enum CMD
{
   CMD_ROTATE,//������ת
   CMD_LEFT,CMD_RIGHI,CMD_DOWN,//��������ҡ����ƶ�������˹����ֻ�����������ƶ���
   CMD_SINK,//�������
   CMD_QUIT//�˳���Ϸ
};

//
enum DRAW
{
    SHOW,  //��ʾ����
	CLEAR, //��������
  	FIX    //�̶�����
};

// �������ֶ���˹����
struct BLOCK
{
    WORD dir[4];//�����״̬
	COLORREF color;//�������ɫ
}   g_Blocks[7]={	{0x0F00,0x4444,0x0F00,0x4444,RED},//����ʵ�ַ������̬+��ɫ ÿ����״��ÿ�������� 4x4 �������ʾ��Ȼ�� 16 �����ӣ�����ת��Ϊ��Ӧ�Ķ�����  
					{0x0660,0x0660,0x0660,0x0660,BLUE},//���б�ʾ���ǡ�-����������̬�������� ���ڡ��Ρ�������λ����ת�󲻱� 
					{0x4460,0x02E0,0x0622,0x0740,MAGENTA},
					{0x2260,0x0E20,0x0644,0x0470,YELLOW},
					{0x0C60,0x2640,0x0C60,0x2640,CYAN},
					{0x0360,0x4620,0x0360,0x4620,GREEN},
					{0x4E00,0x4C40,0x0E40,0x4640,BROWN}                  
};

struct BLOCKINFO
{
	byte id; //����ID
	char x,y;//�������Ϸ����
	byte dir:2;//����ķ���	
}	g_CurBlock,g_NextBlock;


BYTE g_World[WIDTH][HEIGHI]={0};//��Ϸ����  //δ֪��;
int grades;
TCHAR s[50];

void lnit();//������Ϸ
void Quit();//�˳���Ϸ
void NewGame();//��ʼ����Ϸ
void GameOver();//��Ϸʧ��
CMD GetCmd();//��ȡ��������
void DispatchCmd(CMD _cmd);//�ַ���������
void NewBlock();//�����µķ���
bool CheckBlock(BLOCKINFO _block);//���ָ�������Ƿ���Է���
void DrawUnit(int x,int y,COLORREF c,DRAW _draw);//�����鵥Ԫ
void DrawBlock(BLOCKINFO _block,DRAW _draw=SHOW);//������
void OnRotate();//��ת����
void OnLeft();
void OnRight();
void OnDown();
void OnSink();//���׷���
//������...  2017-11-04 22:54:02


void main()
{
     lnit();//������Ϸ
	 CMD c;
		 while(1)
		 {
                c=GetCmd();
				DispatchCmd(c);

				if (c==CMD_QUIT)//����˳�ʱ�������Ի�����ѯ�û��Ƿ��˳�
				{
					HWND wnd=GetHWnd();
					if(MessageBox(wnd,_T("��Ҫ�˳���Ϸ��"),_T("����"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)//������Windows����
					   Quit();
				}
		 }
}


void lnit()
{
    initgraph(640,480);//cmd���ڴ�С
	srand((unsigned)time(NULL));//�������������ֵ
	setbkmode(TRANSPARENT);//����ͼ�����ı���ɫΪ͸��
    
    
	//��ʾ����˵��
	settextstyle(14,0,_T("����"));//������ʽ
    outtextxy(20,330,_T("����˵��"));//��ָ��λ������ַ�����ָ��λ���иú�����1��2����ȷ������ͬ
	outtextxy(20,350,_T("�ϣ���ת"));
	outtextxy(20,370,_T("������"));
	outtextxy(20,390,_T("�ң�����"));
	outtextxy(20,410,_T("�£�����"));
	outtextxy(20,430,_T("�ո񣺳���"));
	outtextxy(20,450,_T("ESC���˳�"));
	outtextxy(20,300,_T("���Ƴ��򣬷Ǳ�����ԭ��"));



	//��������ԭ�㣬������
	setorigin(220,20);

	//��Ϸ�߽�
	rectangle(-1,-1,WIDTH*UNIT,HEIGHI*UNIT);//С
	rectangle((WIDTH+1)*UNIT-1,-1,(WIDTH+5)*UNIT,4*UNIT);// ��
	//�߽����ԭ���ú���Ϊ���������Σ�ͨ�������ľ��εĴ�С�������������Ϸ�߽硣

    
	NewGame();//��ʼ����Ϸ
}




void Quit()//�˳�
{
     closegraph();//�ر�ͼ�λ���
	 exit(0);//�˳�����
}


void	NewGame()//��ʼ����Ϸ
{
     //�����Ϸ����
	 setfillcolor(BLACK);//��䱳��ɫ
	 solidrectangle(0,0,WIDTH*UNIT-1,HEIGHI*UNIT-1);//����ޱ߿���Σ�������Ϸ�߽�֮��ȫ������
	 ZeroMemory(g_World,WIDTH*HEIGHI);//��0����ڴ����򣬰�֮ǰ�����ڴ��ȫ�����ã����㣩


	 //������һ������
	 g_NextBlock.id=rand()%7;//���ɵķ�����0-6֮�������һ��
	 g_NextBlock.dir=rand()%4;//���ɵķ���������0-3������һ��
     g_NextBlock.x=WIDTH+1;//�·���ĺ�������ڳ���+1
	 g_NextBlock.y=HEIGHI-1;//.......�߶�-1�����������䣩
     
	 NewBlock();
}



void GameOver()//������Ϸ
{
     HWND wnd=GetHWnd();
	 if(MessageBox(wnd,_T("��Ϸ������\n ��������һ����"),_T("��Ϸ����"),MB_YESNO|MB_ICONQUESTION)==IDYES)
			NewGame();//Windows����
		else
			Quit();
	 
}


//��ȡ��������
DWORD m_oldtime;//DWORD �޷��Ŷ����ͣ�4���ֽ�
CMD GetCmd()
{
   //��ȡ����ֵ
	while (true)
	{
		//�����ʱ���Զ�����
		DWORD newtime=GetTickCount();//���أ�retrieve���Ӳ���ϵͳ������������elapsed���ĺ����������ķ���ֵ��DWORD
		if (newtime-m_oldtime>=500)//��ʱ���ȥ��ʱ��
		{
			m_oldtime=newtime;
			return CMD_DOWN;//�����ʱ�����ƣ���ʼ���䷽�飩
		}
	    if (kbhit())
	    {
			switch(getch())
			{
			case 'w':
			case 'W':return CMD_ROTATE;//��W������ת
			case 'a':
            case 'A':return CMD_LEFT;//���ƶ�
			case 'd':
			case 'D':return CMD_RIGHI;//��
			case 's':
			case 'S':return CMD_DOWN;//��
			case  27 :return CMD_QUIT;//��
			case ' ' :return CMD_SINK;//����
			case 0:
			case 0xE0://��һ�ֿ��Ʒ���  ����������ʱδ֪
				 switch(getch())
				 {
				 case 72:return CMD_ROTATE;
				 case 75:return CMD_LEFT;
				 case 77:return CMD_RIGHI;
				 case 80:return CMD_DOWN;
				 }
			}
	    }
		Sleep(20);//��ʱ
	}
}


//2017-11-05 14:43:39


//�ַ���������
void DispatchCmd(CMD _cmd)
{
	switch(_cmd)
	{
	case CMD_ROTATE: OnRotate(); break;
	case CMD_LEFT:   OnLeft();   break;
	case CMD_RIGHI:  OnRight();  break;
	case CMD_DOWN:   OnDown();   break;
	case CMD_SINK:   OnSink();   break;
	case CMD_QUIT:   break;
	}
}
void NewBlock()//�����·���  curblock:ָ������ʾ����Ļ�еķ���   newblock��ָ�������е��·���  ������Ҫ��������·����˳�ȥ��curblock��ʾ
{
	g_CurBlock.id=g_NextBlock.id,/*�����ɺõķ����cur��ʾ  id ������*/g_NextBlock.id=rand()%7;//�����µķ����ű���
	g_CurBlock.dir=g_NextBlock.dir,/*�����ɺõķ������̬��cur��ʾ dir �������̬��*/g_NextBlock.dir=rand()%4;//�����µķ������̬
	g_CurBlock.x=(WIDTH-4)/2;//����cur���������
	g_CurBlock.y=HEIGHI+2;
	
	
	//���Ʒ���ֱ���оֲ���ʾ
	WORD c=g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];//���磺g_Blocks[2].dir[1]  ȷ����һ����������в���  ����word���͵ı���c���ֽڴ洢�÷���Ĳ���
	while ((c&0x0F)==0)//&���� ����������Ӧλ���бȶԣ����߶�Ϊ1ʱ����1,���򷵻�0
	{
		g_CurBlock.y--;//ͨ���� 0x0F �Ķ�����Ϊ0000 1111������ ����ȽϺ�Ϊ0������ѭ���������Ѿ����ף����Ѿ�����׷���Ӵ���
		c>>=4;//c����4λ
	}
	
	//�����·���
	DrawBlock(g_CurBlock);
	
	//������һ������
	setfillcolor(BLACK);//����ɫ
	solidrectangle((WIDTH+1)*UNIT,0,(WIDTH+5)*UNIT-1,4*UNIT-1);//���ƾ��δ�С
	DrawBlock(g_NextBlock);
	
	//���ü�ʱ�����ж��Զ�����
	m_oldtime=GetTickCount();//���أ�retrieve���Ӳ���ϵͳ������������elapsed���ĺ����������ķ���ֵ��DWORD
}



// ����Ԫ����
void DrawUnit(int x, int y, COLORREF c, DRAW _draw)
{
	// ���㵥Ԫ�����Ӧ����Ļ����
	int left = x * UNIT;
	int top = (HEIGHI - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHI - y) * UNIT - 1;
	
	// ����Ԫ����
	switch(_draw)
	{
	case SHOW:
		// ����ͨ����
		setlinecolor(0x006060);
		roundrect(left + 1, top + 1, right - 1, bottom - 1, 5, 5);
		setlinecolor(0x003030);
		roundrect(left, top, right, bottom, 8, 8);
		setfillcolor(c);
		setlinecolor(LIGHTGRAY);
		fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
		break;
		
	case FIX:
		// ���̶��ķ���
		setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
		setlinecolor(DARKGRAY);
		fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
		break;
		
	case CLEAR:
		// ��������
		setfillcolor(BLACK);
		solidrectangle(x * UNIT, (HEIGHI - y - 1) * UNIT, (x + 1) * UNIT - 1, (HEIGHI - y) * UNIT - 1);
		break;
	}
}

//������
void DrawBlock(BLOCKINFO _block,DRAW _draw)
{
	WORD b=g_Blocks[_block.id].dir[_block.dir];//���磺g_Blocks[2].dir[1]  ȷ����һ����������в���  ����word���͵ı���c���ֽڴ洢�÷���Ĳ���
	int x,y;
	
	
		  for (int i=0;i<16;i++,b<<=1)//�������ж�  0x8000 16λ��� 
			  if (b&0x8000)
			  {
				  x=_block.x+i%4;
				  y=_block.y-i/4;
				  if(y<HEIGHI)
					  DrawUnit(x,y,g_Blocks[_block.id].color,_draw);//���û���Ԫ���麯��
			  }
			  
}


//���ָ�������Ƿ���Է���   2017-11-06 22:27:55 
bool CheckBlock(BLOCKINFO _block)//�߼��� ���� for�ж�Ϊ��ʱ��֤�������ڷ��鶼�����ڣ��������䣬���Է���ֵ���Ϊ��
{
	WORD b=g_Blocks[_block.id].dir[_block.dir];
	int x,y;
	for (int i=0;i<16;i++,b<<=1)
	     if(b&0x8000)
		 {
			 x=_block.x+i%4;
			 y=_block.y-i/4;
			 if ((x<0)||(x>=WIDTH)||(y<0))//���������X��0�������X>=ʵ�ʷ���Ŀ�Ȼ�������Y<0(������Ϊ�漴Ϊ�棬����Ϊ��)Խ����޷�����
			          return false;
			 if ((y<HEIGHI)&&(g_World[x][y]))//�����Խ�粢����һ���ƶ���λ���з���Ͳ��ܷ���
				      return false;
			
		 }
    return true;
}




//��ת����
void OnRotate()
{
     int dx;
     BLOCKINFO  tmp=g_CurBlock;
	 tmp.dir++;
	 if (CheckBlock(tmp))
	 {
		 dx=0;
		 goto rotate;
	 }
	 tmp.x=g_CurBlock.x-1;
	 if (CheckBlock(tmp))
	 {
		 dx=-1;
		 goto rotate;
	 }
	 tmp.x=g_CurBlock.x+1;
	 if (CheckBlock(tmp))
	 {
		 dx=1;
		 goto rotate;
	 }
	 tmp.x=g_CurBlock.x-2;
	 if (CheckBlock(tmp))
	 {
		 dx=-2;
		 goto rotate;
	 }
	 tmp.x=g_CurBlock.x+2;
	 if (CheckBlock(tmp))
	 {
		 dx=2;
		 goto rotate;
	 }
	 return;



rotate://��ת�������  ��Ϊ��ת�Ļ����������»�ͼ�Σ����Ե��û�����ĺ���
	 DrawBlock(g_CurBlock,CLEAR);
	 g_CurBlock.dir++;
	 g_CurBlock.x+=dx;
	 DrawBlock(g_CurBlock);
} 





//���Ʒſ�
void OnLeft()
{
     BLOCKINFO tmp=g_CurBlock;
	 tmp.x--;
	 if (CheckBlock(tmp))
	 {
		 DrawBlock(g_CurBlock,CLEAR);
		 g_CurBlock.x--;
		 DrawBlock(g_CurBlock);
	 }
}



//���Ʒ���
void OnRight()
{
     BLOCKINFO tmp=g_CurBlock;
	 tmp.x++;
	 if (CheckBlock(tmp))
	 {
		 DrawBlock(g_CurBlock,CLEAR);
		 g_CurBlock.x++;
		 DrawBlock(g_CurBlock);
	 }
}




//���Ʒ���
void OnDown()
{
     
	 BLOCKINFO tmp=g_CurBlock;
	 tmp.y--;
	 if (CheckBlock(tmp))
	 {
		 DrawBlock(g_CurBlock,CLEAR);
		 g_CurBlock.y--;
		 DrawBlock(g_CurBlock);
	 }
     else
		 OnSink();//��������ʱ��ֱ�ӳ��ײ���
}



//����
void OnSink()
{
     int i,x,y;
     
	 //ִ���������Ʋ���
	 DrawBlock(g_CurBlock,CLEAR);
	 BLOCKINFO tmp=g_CurBlock;
	 tmp.y--;
     while(CheckBlock(tmp))
     {
	       g_CurBlock.y--;
		   tmp.y--;
	 }
	 DrawBlock(g_CurBlock,FIX);



	 WORD b=g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];//��ȡ��ǰ����״̬
	 for (i=0;i<16;i++,b<<=1)
	      if (b&0x8000)
	      {
			  if (g_CurBlock.y-i/4>=HEIGHI)//�������Ĺ̶��߶ȸ����ƶ��߶ȣ�������Ϸ
			  {   
				  GameOver();
				  return;

			  }
	          else
				  g_World[g_CurBlock.x+i%4][g_CurBlock.y-i/4]=1;//��Ϸ�������Ϊ1 ��������
		  }

	 //////////////////////////////////////////////////////////////////////////
		  /*��������Ŀǰ���������������ͣ��д 2017-11-07 16:21:21*/
		  /*�����Һ�ͬѧ�㲦��ָ�����ָ���ҵ     2017-11-09 16:20:45*/

     
		  	 //ǰ�벿��ʵ�ֵ��� �������������ѿ��������ķ����滻Ϊ����״̬����벿��Ϊ����״̬�����������������ϲ�ķ����ƶ�������������Ϸ
	 BYTE remove=0;//��4λ������Ƿ����漰��4���Ƿ���������Ϊ
	 for (y=g_CurBlock.y;y>=max(g_CurBlock.y-3,0);y--)//�����������
		 {
			  i=0;
		 for (x=0;x<WIDTH;x++)//�������򣬼���Ƿ�����
			 
				  if (g_World[x][y]==1)//y�̶�x����
				  //{
					  i++;
				  //}
		 if (i==WIDTH)//�����ִ��if�ڲ���(�������)
			  {
				  remove|=(1<<(g_CurBlock.y-y));//��ΪҪ����  ��1������ ��ͨ��������ʹ��remove��1
				      /*���磺����ĺ���Ϊa=a|b;����a��b����λ��(|)�����������ֵ��a��
					  ��λ��ļ������Ϊ��
					  1 ��λ���м��㣻
					  2 ��������ͬλ��ֵ�������Ϊ0��������Ӧλ��ֵΪ0��
                      3 ��2������⣬�����Ӧλ��ֵΪ1��*/
				  setfillcolor(LIGHTGRAY);//��������Ķ��� ���������ķ���Ϊǳ��ɫ
				  //����Ϊǳ��ɫ
				  setlinecolor(LIGHTGRAY);//����������������xxxΪǳ��ɫ
				  setfillstyle(BS_HATCHED,HS_DIAGCROSS);//����Ϊͼ����䣬ָ�����ͼ�� ΪXX��
				  fillrectangle(0,(HEIGHI-y-1)*UNIT+UNIT/2-5,WIDTH*UNIT-1,(HEIGHI-y-1)*UNIT+UNIT/2+5);//�������������Ŀ������ο�
				  setfillstyle(BS_SOLID);//�����ʽ ��ʵ���
			  }
			  
		  }
        if (remove)//���������������
        {
			Sleep(300);//��ʱX00����  
			//�����ղű�ǵ���
			
			
			IMAGE img;//ͼ�����   
			for (i=0;i<4;i++,remove>>=1)//����ʵ��  
			{
				if (remove&1)//��Ϊλ�����:������1&������2,����������Ӧλ���бȶԣ����߶�Ϊ1ʱ����1,���򷵻�0.  ���Ϊ1ִ����������
				{
					for (y=g_CurBlock.y-i+1;y<HEIGHI;y++)//�������
					    for(x=0;x<WIDTH;x++)
						{
						g_World[x][y-1]=g_World[x][y];//���Ѿ�ɾ�����滻Ϊ�ϲ�δ��ɾ����
						g_World[x][y]=0;//���ϲ�������Ϊ0
						}
				
	               
				   
						//λ�����ƺ󣬽�ͼƬ������0,unitλ��(��ΪͼƬˢ����ʾ)			
					getimage(&img,0,0,WIDTH*UNIT,(HEIGHI-(g_CurBlock.y-i+1))*UNIT);
					putimage(0,UNIT,&img);

				}
			}

					setfillcolor(getbkcolor());
					outtextxy(20,-40,s);
					_stprintf(s,_T("��ĵ÷��ǣ�  %d"),grades);
					settextcolor(WHITE);
					outtextxy(20,-40,s);

        }

          NewBlock();

}




