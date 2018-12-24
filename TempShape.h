#pragma once

void Draw_Tmp_Switch(CClientDC* dc ,CPen* pen, CPoint Point ,int size_Of_Switch)
{
	//int x, y, x1, y1, x2, y2, x3, y3, x4, y4;
	int X, Y, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
	X = Point.x;
	Y = Point.y;
	X1 = X + size_Of_Switch;
	Y1 = Y;
	X2 = X + size_Of_Switch * 2;
	Y2 = Y - size_Of_Switch;//这里可能需要根据映射模式稍加修改  在六种映射模式中 判断使用的是哪一种模式 从而得出是 '+' 还是 '-' 不过一般的都是 '-'
	X3 = X2;
	Y3 = Y;
	X4 = X3 + size_Of_Switch;
	Y4 = Y;

	dc->MoveTo(X, Y);
	dc->LineTo(X1, Y1);
	dc->MoveTo(X1, Y1);
	dc->LineTo(X2, Y2);
	dc->MoveTo(X3, Y3);
	dc->LineTo(X4, Y4);
}