#pragma once

struct LayerStruct
{
	short m_id_only;
	CString m_Name;  //ͼ�������
	BOOL b_Display;   //�Ƿ���ʾ
	int reserved;     //���� ������û���õ� �������Խ�����չ ���籣��һ����ɫ ��ʾ���ͼ�������ͼ�ζ��������ɫ��ʾ
};

struct ScreenStructForBoost
{
	float blc;     //������
	float startX;  
	float startY;  //ԭ������λ��
};

struct GraphSelectStruct
{
	short int Lb;    //ͼ�α�ǩ
	short int Index;  //ÿ��ͼ�ε�ĳ��Ԫ�ص������±�
	short int id_only; //Ψһʶ���
	GraphSelectStruct(short int lb = 0, short int index = 0, short int Id_only = 0) :Lb(lb), Index(index), id_only(Id_only){}
};

struct UndoStruct	//�����洢�������Ϣ�Ľṹ
{
	unsigned char Lb;	//���������       2--�����ĵ� 3--ɾ���ĵ�   ���������Ĳ���(����Ŵ��ƶ�ͼ�ε�) Ҳ�����Խ���һ��ʶ��Ž��м�¼
	long l_Start;		//���ⲿ��ʱ�ļ��в��������ļ�¼λ��
	int	Index;			//��ɾ���������Ӳ�������ʱ��ɾ��ͼ��Ԫ�ص�����
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
	int m_AllColorNum;      //������ɫ��
	int m_AllLayerNum;      //��ͼ����
	int m_CurrentColorNum;  //��ǰ��ɫ��
	int m_CurrentLayerNum;  //��ǰͼ����
	long* m_ColorList;      //��ɫ�б�  �洢ÿһ����ɫ
public:
	LayerStruct* m_LayerList;//�洢ÿһ��ͼ��

public:
	COLORREF GetColor(int n);  //���ݲ���n��ȡ��ͬ����ɫ
	BOOL GetDisplayState(int n);//��ȡ��ǰ��ʾ״̬
};

