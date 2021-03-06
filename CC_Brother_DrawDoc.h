
// CC_Brother_DrawDoc.h: CCCBrotherDrawDoc 类的接口
//


#pragma once

#include "CC_Brother_Line.h"
#include "CC_Brother_PLine.h"
#include "CC_Brother_GraphPara.h"
#include "CC_Brother_Text.h"
#include "CC_Brother_Switch.h"
#include "CC_Brother_Ground.h"

BOOL IsRectCross(float minx, float miny, float maxx, float maxy);

class CCCBrotherDrawDoc : public COleDocument
{
protected: // 仅从序列化创建
	CCCBrotherDrawDoc() noexcept;
	DECLARE_DYNCREATE(CCCBrotherDrawDoc)

//添加的成员函数
public:
	baseShape* GetGraph(short Lb,int Index);   //通过继承的多态性 通过抽象类获取实例

	CC_Brother_Line* AddLine(long ColorPen, long ColorBrush,float LineWide,short LineType,short Layer,int id_only, float X1,float Y1,float X2,float Y2);
	CC_Brother_Line * GetLine(int Index);
	CC_Brother_Text* AddText(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,
		float xStart, float yStart, float angle1, float angle2, float TextHeight, float TextWide, float cellWide, unsigned char TextFont, CString Text);
	CC_Brother_Text* AddText1(CC_Brother_Text* ent);
	CC_Brother_Text* GetText(int Index);
	CC_Brother_Switch* AddSwitch(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, float X, float Y);
	CC_Brother_Switch* GetSwitch(int Index);
	CC_Brother_PLine* AddPLine(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, int Number, PointStruct* List, bool fill);
	void AddPLine(CC_Brother_PLine* ent);
	CC_Brother_PLine* GetPLine(int Index);
	CC_Brother_Ground* AddGround(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, float X, float Y);
	CC_Brother_Ground* GetGround(int Index);
	//添加图形实例到存储他们的各自的容器中
	
	//ScreenStructForBoost* getScreen();   //获取每一屏的状态  以便还原
	int GetGraphNum(short Lb);           //通过标签参数获取这类图形的数目
	int GetGraphUpperBound(short Lb);    //通过标签参数获取这类图形的上界
	int GetGraphID(short Lb);            //通过标签参数获取这类图形的ID
	//提供选中图形的相关支持

	//本来应该设置成private或者protected  但是为了操作简单 设置为了public
	GraphSelectStruct* GraphSelect;        //选中图形元素的存储
	int n_GraphSelect;                     //选中图形数目
	bool b_IsOleSelect;			//是否选中了OLE对象

	void DeleteGraph(short Lb, int Index);    //通过标签参数和在容器中的位置index删除某一个图形

	void ReDraw(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor);        
	/*重画所有图形  包括被选择了的图形 只是跳过被标记了删除的图形
	如果有时间 这里可以做优化  将图形的状态不仅仅是两种状态（是否删除） 而是选中 没被选中 和删除  这样可以只需要画没被选中的图形 而选中的图形可以
	分开画，如果选中的图形很多的话 这样可以大大提高程序运行性能*/

	void DrawSelectedGraph(CDC* pDC, int Lb, int Index, int m_DrawMode, int m_DrawMode1, long BackColor);   //重画被选择了的图形 让其特殊显示
	void AddScreen(float StartX,float StartY,float blc);    //画图路径的记录
	bool GetRect(float * m_Xmin, float * m_Ymin, float * m_Xmax, float * m_Ymax);//获取一个矩形框 这个矩形框是能够包含所有图形的最小的一个矩形
	bool AddSelectList(int Lb, int Index, int Id_Only);           //将选中的图形 添加到GraphSelectList中
	bool PointSelect(float x, float y, float validDistance, float blc, int* Lb, int* Index, int* id_only);  
	//判断点是否在计算区域内  x,y 为点的坐标  validDistance：有效距离 blc：坐标变换的比例尺  Lb，Index，id_only三者共同确定一个图形以及其存储位置 
	bool AddUndo(int Lb, int Numble, GraphSelectStruct * GraphList);
	void ClearGraphSelect();   //清楚所有框选的图形
	void OnEditUndo();

//提供撤放大过程中销删除操作的支持
public:
	int m_MaxScreen;                  //能够记录的最大路径数
	int m_CurrentScreen;              //当前记录的路径位置
	ScreenStructForBoost* m_Screen;   //目前的功能是 记录放大过程的操作路径

// 重写
public:
	virtual BOOL OnNewDocument();       //新建文件
	virtual void Serialize(CArchive& ar);//序列化 实现能够打开 打印 和保存文件
	virtual void DeleteContents();			//删除文档时调用
	virtual void OnCloseDocument();			//关闭文档时调用
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);	//打开文档时调用
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);	//保存文档时调用

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CCCBrotherDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg void OnUpdateSelectClear(CCmdUI* pCmdUI);//是否能够执行"放弃选择“
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);	//是否能够执行"Undo"菜单
	//afx_msg void OnTest();
	DECLARE_MESSAGE_MAP()

//提供销删除操作的支持
protected:
	CString FilePath, FileName, DatabaseFileName;		//存储逆操作文档的路径和文件名
	CFile Fundo;					//用来存储逆操作信息的文件指针  指向逆向操作的文件对象
	CStdioFile DatabaseFile;             //存储数据库文档
	UndoStruct* m_UndoList;			//存储逆操作的索引信息
	int n_CurUnIndex, n_MaxUnIndex;	//当前处于操作的次数和总的操作次数
	//UINT DrawFormat;

//添加的自定义成员变量
private:
	CTypedPtrArray<CObArray, CC_Brother_Line*>m_LineArray;
	CTypedPtrArray<CObArray, CC_Brother_Text*>m_TextArray;
	CTypedPtrArray<CObArray, CC_Brother_Switch*>m_SwitchArray;
	CTypedPtrArray<CObArray, CC_Brother_PLine*>m_PLineArray;
	CTypedPtrArray<CObArray, CC_Brother_Ground*>m_GroundArray;
	int* m_OnlyIndex;             //每类图形分门别类的存储

	float m_xStart = 0, m_yStart = 0, blc = 0;  //原点坐标 以及这个坐标与实际坐标之间转换的比例尺
	long m_backgroundColor = RGB(255,255,255);/* = 4;*/ //可以添加模块获取其他的背景色 通过 getColor和setColor来获取或者设置其值
	short m_lineType = 0 , m_lineWide = 1.0;
	short m_currLayer;

public:
	CC_Brother_GraphPara m_GraphPara;


#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
