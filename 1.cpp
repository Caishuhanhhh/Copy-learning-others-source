//俄罗斯方块复原 参考代码 知乎 河海大学
#include <easyx.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <iostream.h>



#define WIDTH  10//宽度
#define HEIGHI 22//高度
#define UNIT   20//游戏区单位的实际像素

//方块绘制 枚举类型
enum CMD
{
   CMD_ROTATE,//方块旋转
   CMD_LEFT,CMD_RIGHI,CMD_DOWN,//方块的左、右、下移动（俄罗斯方块只有三个方向移动）
   CMD_SINK,//方块沉底
   CMD_QUIT//退出游戏
};

//
enum DRAW
{
    SHOW,  //显示方块
	CLEAR, //擦除方块
  	FIX    //固定方块
};

// 定义七种俄罗斯方块
struct BLOCK
{
    WORD dir[4];//方块的状态
	COLORREF color;//方块的颜色
}   g_Blocks[7]={	{0x0F00,0x4444,0x0F00,0x4444,RED},//用来实现方块的形态+颜色 每个形状的每个方向，用 4x4 的区域表示，然后共 16 个格子，可以转换为对应的二进制  
					{0x0660,0x0660,0x0660,0x0660,BLUE},//上行表示的是“-”的四种形态，本行是 “口”形。故所有位置旋转后不变 
					{0x4460,0x02E0,0x0622,0x0740,MAGENTA},
					{0x2260,0x0E20,0x0644,0x0470,YELLOW},
					{0x0C60,0x2640,0x0C60,0x2640,CYAN},
					{0x0360,0x4620,0x0360,0x4620,GREEN},
					{0x4E00,0x4C40,0x0E40,0x4640,BROWN}                  
};

struct BLOCKINFO
{
	byte id; //方块ID
	char x,y;//方块的游戏坐标
	byte dir:2;//方块的方向	
}	g_CurBlock,g_NextBlock;


BYTE g_World[WIDTH][HEIGHI]={0};//游戏区域  //未知用途
int grades;
TCHAR s[50];

void lnit();//进入游戏
void Quit();//退出游戏
void NewGame();//开始新游戏
void GameOver();//游戏失败
CMD GetCmd();//获取控制命令
void DispatchCmd(CMD _cmd);//分发控制命令
void NewBlock();//生成新的方块
bool CheckBlock(BLOCKINFO _block);//检测指定方块是否可以放下
void DrawUnit(int x,int y,COLORREF c,DRAW _draw);//画方块单元
void DrawBlock(BLOCKINFO _block,DRAW _draw=SHOW);//画方块
void OnRotate();//旋转方块
void OnLeft();
void OnRight();
void OnDown();
void OnSink();//沉底方块
//框架完成...  2017-11-04 22:54:02


void main()
{
     lnit();//进入游戏
	 CMD c;
		 while(1)
		 {
                c=GetCmd();
				DispatchCmd(c);

				if (c==CMD_QUIT)//点击退出时，弹出对话框咨询用户是否退出
				{
					HWND wnd=GetHWnd();
					if(MessageBox(wnd,_T("你要退出游戏吗？"),_T("提醒"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)//引用了Windows函数
					   Quit();
				}
		 }
}


void lnit()
{
    initgraph(640,480);//cmd窗口大小
	srand((unsigned)time(NULL));//产生随机数种子值
	setbkmode(TRANSPARENT);//设置图案填充的背景色为透明
    
    
	//显示操作说明
	settextstyle(14,0,_T("黑体"));//字体样式
    outtextxy(20,330,_T("操作说明"));//在指定位置输出字符串，指定位置有该函数第1，2参数确定。下同
	outtextxy(20,350,_T("上：旋转"));
	outtextxy(20,370,_T("左：左移"));
	outtextxy(20,390,_T("右：右移"));
	outtextxy(20,410,_T("下：下移"));
	outtextxy(20,430,_T("空格：沉底"));
	outtextxy(20,450,_T("ESC：退出"));
	outtextxy(20,300,_T("仿制程序，非本人所原创"));



	//设置坐标原点，方便编程
	setorigin(220,20);

	//游戏边界
	rectangle(-1,-1,WIDTH*UNIT,HEIGHI*UNIT);//小
	rectangle((WIDTH+1)*UNIT-1,-1,(WIDTH+5)*UNIT,4*UNIT);// 大
	//边界绘制原理：该函数为用来画矩形，通过所画的矩形的大小，来间接锁定游戏边界。

    
	NewGame();//开始新游戏
}




void Quit()//退出
{
     closegraph();//关闭图形环境
	 exit(0);//退出程序
}


void	NewGame()//开始新游戏
{
     //清空游戏区域
	 setfillcolor(BLACK);//填充背景色
	 solidrectangle(0,0,WIDTH*UNIT-1,HEIGHI*UNIT-1);//填充无边框矩形，覆盖游戏边界之内全部内容
	 ZeroMemory(g_World,WIDTH*HEIGHI);//用0填充内存区域，把之前所用内存段全部重置（归零）


	 //生成下一个方块
	 g_NextBlock.id=rand()%7;//生成的方块是0-6之间的任意一个
	 g_NextBlock.dir=rand()%4;//生成的方块形体是0-3的任意一种
     g_NextBlock.x=WIDTH+1;//新方块的横坐标等于长度+1
	 g_NextBlock.y=HEIGHI-1;//.......高度-1（才能往下落）
     
	 NewBlock();
}



void GameOver()//结束游戏
{
     HWND wnd=GetHWnd();
	 if(MessageBox(wnd,_T("游戏结束。\n 您想重来一局吗？"),_T("游戏结束"),MB_YESNO|MB_ICONQUESTION)==IDYES)
			NewGame();//Windows函数
		else
			Quit();
	 
}


//获取控制命令
DWORD m_oldtime;//DWORD 无符号短整型，4个字节
CMD GetCmd()
{
   //获取控制值
	while (true)
	{
		//如果超时，自动下落
		DWORD newtime=GetTickCount();//返回（retrieve）从操作系统启动所经过（elapsed）的毫秒数，它的返回值是DWORD
		if (newtime-m_oldtime>=500)//新时间减去旧时间
		{
			m_oldtime=newtime;
			return CMD_DOWN;//如果超时，下移（开始掉落方块）
		}
	    if (kbhit())
	    {
			switch(getch())
			{
			case 'w':
			case 'W':return CMD_ROTATE;//按W方块旋转
			case 'a':
            case 'A':return CMD_LEFT;//左移动
			case 'd':
			case 'D':return CMD_RIGHI;//右
			case 's':
			case 'S':return CMD_DOWN;//下
			case  27 :return CMD_QUIT;//退
			case ' ' :return CMD_SINK;//沉底
			case 0:
			case 0xE0://另一种控制方法  按键含义暂时未知
				 switch(getch())
				 {
				 case 72:return CMD_ROTATE;
				 case 75:return CMD_LEFT;
				 case 77:return CMD_RIGHI;
				 case 80:return CMD_DOWN;
				 }
			}
	    }
		Sleep(20);//延时
	}
}


//2017-11-05 14:43:39


//分发控制命令
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
void NewBlock()//生成新方块  curblock:指的是显示在屏幕中的方块   newblock：指的制造中的新方块  我们需要把制造的新方块运出去用curblock显示
{
	g_CurBlock.id=g_NextBlock.id,/*把生成好的方块给cur显示  id 方块标号*/g_NextBlock.id=rand()%7;//制造新的方块存放备用
	g_CurBlock.dir=g_NextBlock.dir,/*把生成好的方块的姿态给cur显示 dir 方块的姿态号*/g_NextBlock.dir=rand()%4;//制造新的方块的姿态
	g_CurBlock.x=(WIDTH-4)/2;//计算cur方块的坐标
	g_CurBlock.y=HEIGHI+2;
	
	
	//下移方块直到有局部显示
	WORD c=g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];//例如：g_Blocks[2].dir[1]  确定了一个方块的所有参数  创建word类型的变量c按字节存储该方块的参数
	while ((c&0x0F)==0)//&运算 将操作数对应位进行比对，二者都为1时返回1,否则返回0
	{
		g_CurBlock.y--;//通过与 0x0F 的二进制为0000 1111与运算 如果比较后为0，结束循环。表明已经到底（或已经与最底方块接触）
		c>>=4;//c右移4位
	}
	
	//绘制新方块
	DrawBlock(g_CurBlock);
	
	//绘制下一个方块
	setfillcolor(BLACK);//填充黑色
	solidrectangle((WIDTH+1)*UNIT,0,(WIDTH+5)*UNIT-1,4*UNIT-1);//绘制矩形大小
	DrawBlock(g_NextBlock);
	
	//设置计时器，判断自动下落
	m_oldtime=GetTickCount();//返回（retrieve）从操作系统启动所经过（elapsed）的毫秒数，它的返回值是DWORD
}



// 画单元方块
void DrawUnit(int x, int y, COLORREF c, DRAW _draw)
{
	// 计算单元方块对应的屏幕坐标
	int left = x * UNIT;
	int top = (HEIGHI - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHI - y) * UNIT - 1;
	
	// 画单元方块
	switch(_draw)
	{
	case SHOW:
		// 画普通方块
		setlinecolor(0x006060);
		roundrect(left + 1, top + 1, right - 1, bottom - 1, 5, 5);
		setlinecolor(0x003030);
		roundrect(left, top, right, bottom, 8, 8);
		setfillcolor(c);
		setlinecolor(LIGHTGRAY);
		fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
		break;
		
	case FIX:
		// 画固定的方块
		setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
		setlinecolor(DARKGRAY);
		fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
		break;
		
	case CLEAR:
		// 擦除方块
		setfillcolor(BLACK);
		solidrectangle(x * UNIT, (HEIGHI - y - 1) * UNIT, (x + 1) * UNIT - 1, (HEIGHI - y) * UNIT - 1);
		break;
	}
}

//画方块
void DrawBlock(BLOCKINFO _block,DRAW _draw)
{
	WORD b=g_Blocks[_block.id].dir[_block.dir];//例如：g_Blocks[2].dir[1]  确定了一个方块的所有参数  创建word类型的变量c按字节存储该方块的参数
	int x,y;
	
	
		  for (int i=0;i<16;i++,b<<=1)//画方块判断  0x8000 16位最大 
			  if (b&0x8000)
			  {
				  x=_block.x+i%4;
				  y=_block.y-i/4;
				  if(y<HEIGHI)
					  DrawUnit(x,y,g_Blocks[_block.id].color,_draw);//调用画单元方块函数
			  }
			  
}


//检测指定方块是否可以放下   2017-11-06 22:27:55 
bool CheckBlock(BLOCKINFO _block)//逻辑型 函数 for中都为假时，证明区域内方块都不存在，可以下落，所以返回值最后为真
{
	WORD b=g_Blocks[_block.id].dir[_block.dir];
	int x,y;
	for (int i=0;i<16;i++,b<<=1)
	     if(b&0x8000)
		 {
			 x=_block.x+i%4;
			 y=_block.y-i/4;
			 if ((x<0)||(x>=WIDTH)||(y<0))//如果横坐标X＜0或横坐标X>=实际方块的宽度或纵坐标Y<0(或：任意为真即为真，否则为假)越界就无法放下
			          return false;
			 if ((y<HEIGHI)&&(g_World[x][y]))//如果不越界并且下一个移动的位置有方块就不能放下
				      return false;
			
		 }
    return true;
}




//旋转方块
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



rotate://旋转处理过程  因为旋转的话，就是重新画图形，所以调用画方块的函数
	 DrawBlock(g_CurBlock,CLEAR);
	 g_CurBlock.dir++;
	 g_CurBlock.x+=dx;
	 DrawBlock(g_CurBlock);
} 





//左移放块
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



//右移方块
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




//下移方块
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
		 OnSink();//不可下移时，直接沉底操作
}



//沉底
void OnSink()
{
     int i,x,y;
     
	 //执行连续下移操作
	 DrawBlock(g_CurBlock,CLEAR);
	 BLOCKINFO tmp=g_CurBlock;
	 tmp.y--;
     while(CheckBlock(tmp))
     {
	       g_CurBlock.y--;
		   tmp.y--;
	 }
	 DrawBlock(g_CurBlock,FIX);



	 WORD b=g_Blocks[g_CurBlock.id].dir[g_CurBlock.dir];//获取当前方块状态
	 for (i=0;i<16;i++,b<<=1)
	      if (b&0x8000)
	      {
			  if (g_CurBlock.y-i/4>=HEIGHI)//如果方块的固定高度高于制定高度，结束游戏
			  {   
				  GameOver();
				  return;

			  }
	          else
				  g_World[g_CurBlock.x+i%4][g_CurBlock.y-i/4]=1;//游戏区域更新为1 继续运行
		  }

	 //////////////////////////////////////////////////////////////////////////
		  /*消除函数目前不清楚运作机理，暂停编写 2017-11-07 16:21:21*/
		  /*经任忠浩同学点拨，指导，恢复作业     2017-11-09 16:20:45*/

     
		  	 //前半部分实现的是 消除操作，即把可以消除的方块替换为消除状态，后半部分为消除状态动画结束后，用来把上层的方块移动下来，继续游戏
	 BYTE remove=0;//低4位用来标记方块涉及的4行是否有消除行为
	 for (y=g_CurBlock.y;y>=max(g_CurBlock.y-3,0);y--)//消除动画设计
		 {
			  i=0;
		 for (x=0;x<WIDTH;x++)//遍历纵向，检测是否满格
			 
				  if (g_World[x][y]==1)//y固定x增加
				  //{
					  i++;
				  //}
		 if (i==WIDTH)//满格后执行if内操作(清除操作)
			  {
				  remove|=(1<<(g_CurBlock.y-y));//因为要消除  置1才消除 故通过该运算使其remove置1
				      /*例如：代表的含义为a=a|b;即把a和b做按位或(|)操作，结果赋值给a。
					  按位或的计算规则为：
					  1 逐位进行计算；
					  2 计算数的同位上值，如果均为0，则结果对应位上值为0；
                      3 除2中情况外，结果对应位上值为1。*/
				  setfillcolor(LIGHTGRAY);//清除操作的动画 消除动画的方块为浅蓝色
				  //设线为浅蓝色
				  setlinecolor(LIGHTGRAY);//清除动画方块里面的xxx为浅蓝色
				  setfillstyle(BS_HATCHED,HS_DIAGCROSS);//设置为图案填充，指定填充图案 为XX形
				  fillrectangle(0,(HEIGHI-y-1)*UNIT+UNIT/2-5,WIDTH*UNIT-1,(HEIGHI-y-1)*UNIT+UNIT/2+5);//设置消除动画的可填充矩形框
				  setfillstyle(BS_SOLID);//填充样式 固实填充
			  }
			  
		  }
        if (remove)//如果产生正行消除
        {
			Sleep(300);//延时X00毫秒  
			//擦掉刚才标记的行
			
			
			IMAGE img;//图像对象   
			for (i=0;i<4;i++,remove>>=1)//消除实现  
			{
				if (remove&1)//作为位运算符:操作数1&操作数2,将操作数对应位进行比对，二者都为1时返回1,否则返回0.  如果为1执行消除操作
				{
					for (y=g_CurBlock.y-i+1;y<HEIGHI;y++)//横向遍历
					    for(x=0;x<WIDTH;x++)
						{
						g_World[x][y-1]=g_World[x][y];//把已经删除的替换为上层未被删除的
						g_World[x][y]=0;//故上层重新置为0
						}
				
	               
				   
						//位置下移后，将图片拷贝至0,unit位置(作为图片刷新显示)			
					getimage(&img,0,0,WIDTH*UNIT,(HEIGHI-(g_CurBlock.y-i+1))*UNIT);
					putimage(0,UNIT,&img);

				}
			}

					setfillcolor(getbkcolor());
					outtextxy(20,-40,s);
					_stprintf(s,_T("你的得分是：  %d"),grades);
					settextcolor(WHITE);
					outtextxy(20,-40,s);

        }

          NewBlock();

}




