#pragma once

struct LayerStruct
{
	short m_id_only;
	CString m_Name;  //图层的名称
	BOOL b_Display;   //是否显示
	int reserved;     //备用 在这里没有用到 不过可以将其扩展 比如保存一个颜色 表示这个图层的所有图形都用这个颜色显示
};

struct ScreenStructForBoost
{
	float blc;     //比例尺
	float startX;  
	float startY;  //原点坐标位置
};

struct GraphSelectStruct
{
	short int Lb;    //图形标签
	short int Index;  //每类图形的某个元素的数组下标
	short int id_only; //唯一识别号
	GraphSelectStruct(short int lb = 0, short int index = 0, short int Id_only = 0) :Lb(lb), Index(index), id_only(Id_only){}
};

struct UndoStruct	//用来存储逆操作信息的结构
{
	unsigned char Lb;	//操作的类别       2--增加文档 3--删除文档   对于其他的操作(比如放大、移动图形等) 也都可以建立一个识别号进行记录
	long l_Start;		//在外部零时文件中操作参数的记录位置
	int	Index;			//在删除或者增加操作操作时是删除图形元素的数量
};

struct PointStruct
{
	float x;
	float y;
	PointStruct(float x = 0.0, float y = 0.0) :x(x), y(y) {}
};

class CC_Brother_GraphPara
{
public:
	CC_Brother_GraphPara();
	~CC_Brother_GraphPara();

protected:
	int m_AllColorNum;      //所有颜色书
	int m_AllLayerNum;      //总图层数
	int m_CurrentColorNum;  //当前颜色数
	int m_CurrentLayerNum;  //当前图层数
	long* m_ColorList;      //颜色列表  存储每一个颜色
public:
	LayerStruct* m_LayerList;//存储每一个图层

public:
	COLORREF GetColor(int n);  //根据参数n获取不同的颜色
	BOOL GetDisplayState(int n);//获取当前显示状态
};

