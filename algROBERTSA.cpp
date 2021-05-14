#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
using namespace std;
const double M_PI = 40;

class TFigure {}; //смещение для рисунка
HWND hwnd = GetConsoleWindow(); //Берём ориентир на консольное окно (В нём будем рисовать)
HDC dc = GetDC(hwnd); //Цепляемся к консольному окну
//HDC dc = CreateCompatibleDC(hg);
//HBITMAP hBM = CreateCompatibleBitmap(hg, 1000, 1000);
//SelectObject(dc, hBM);

#define clBk RGB(15,15,15); //Цвет по умолчанию


bool proverka(double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double wx, double wy, double wz) {
	double Vec1X = v1x - v2x;
	double Vec2X = v3x - v2x;
	double Vec1Y = v1y - v2y;
	double Vec2Y = v3y - v2y;
	double Vec1Z = v1z - v2z;
	double Vec2Z = v3z - v2z;

	double A = Vec1Y * Vec2Z - Vec2Y * Vec1Z;
	double B = Vec1Z * Vec2X - Vec2Z * Vec1X;
	double C = Vec1X * Vec2Y - Vec2X * Vec1Y;
	double D = -(A * v1x + B * v1y + C * v1z);

	if ((A * wx + B * wy + C * wz + D) < 0) {
		A = A * -1;
		B = B * -1;
		C = C * -1;
		D = D * -1;
	}
	if (A * 0 + B * 0 + C * LONG_MAX + D > 0) {
		return true;
	}
	else {
		return false;
	}
}

//C:\Qt\cube.obj



class TPoint3D :public TFigure {
	double x, y, z;
public:
	virtual void setx(const double value) { x = value; }
	virtual void sety(const double value) { y = value; }
	virtual void setz(const double value) { z = value; }
	virtual void set(const double value1, const double value2, const double value3) {
		x = value1, y = value2, z = value3;
	}

	virtual double getx() const { return x; }
	virtual double gety() const { return y; }
	virtual double getz() const { return z ; }

};

class TPoint : public TFigure {
	double x, y;
public:
	virtual void setx(const double value) { x = value; }
	virtual void sety(const double value) { y = value; }

	virtual double getx() const { return x; }
	virtual double gety() const { return y; }
};


class TCube3D {
	std::vector<TPoint3D> w;  //мировые координаты
	std::vector<TPoint>  v;  //видиовые координаты
	std::vector<int>  trian;
	std::vector<HPEN> penz;
	
	TPoint3D pointw;

public:
	std::vector<TPoint3D> WN;
	//std::vector<TPoint3D> WL;
	TCube3D(string file_name) {
		ifstream in;

		in.open(file_name.c_str());
		if (!in.is_open())
		{
			cout << "\nCan not open file for reading!\n";
			return;
		}
		string str;
		while (!in.eof())
		{
			getline(in, str);
			if (str[str.length()] == '\n') str[str.length()] = ' ';
			if (str[0] == 'v') {
				int x, y, z;
				string s[3];
				getline(in, s[0]);
				getline(in, s[1]);
				getline(in, s[2]);
				x = atoi(s[0].c_str());
				y = atoi(s[1].c_str());
				z = atoi(s[2].c_str());
				TPoint3D point;
				point.set(x, y, z);
				w.push_back(point);
				WN.push_back(point);
			//	WL.push_back(point);
			}
			if (str[0] == 'k') {
				int p1, p2, p3;
				string p[3];
				getline(in, p[0]);
				getline(in, p[1]);
				getline(in, p[2]);

				p1 = atoi(p[0].c_str());
				p2 = atoi(p[1].c_str());
				p3 = atoi(p[2].c_str());

				trian.push_back(p1);
				trian.push_back(p2);
				trian.push_back(p3);

			}
		}
		for (int i = 0; i < w.size(); i++) {
			TPoint op;

			op.setx(0);
			op.sety(0);
			v.push_back(op);
		}
		
	}

	/*ПЕРЕВОД МИРОВЫХ КООРИНАТ В ВИДОВЫЕ*/
	virtual void view_transformation(double LX, double LY, double LZ, double RRRX, double RRRY, double zoom) {
		//каждый кадр мы берём кубик который считали из файла и изменяем его по этим параметрам 

		for (int i = 0; i < w.size(); i++) {//берём по очереди все вершины кубика

			double KX1, KY1, KZ1;// умножаем на матрицу поворота X
			KX1 = w[i].getx();
			KY1 = w[i].gety() * cos(LX) + w[i].getz() * sin(LX);
			KZ1 = -w[i].gety() * sin(LX) + w[i].getz() * cos(LX);

			double KX2, KY2, KZ2;// умножаем на матрицу поворота Y
			KX2 = KX1 * cos(LY) - KZ1 * sin(LY);
			KY2 = KY1;
			KZ2 = KX1 * sin(LY) + KZ1 * cos(LY);

			double KX3, KY3, KZ3;// умножаем на матрицу поворота Z
			KX3 = KX2 * cos(LZ) + KY2 * sin(LZ);
			KY3 = -KX2 * sin(LZ) + KY2 * cos(LZ);
			KZ3 = KZ2;
			
			KX3 += RRRX;// перемещаем кубик в просранстве 
			KY3 += RRRY;

			KX3 *= zoom;// изменяем размер
			KY3 *= zoom;
			KZ3 *= zoom;

			TPoint3D N;
			N.setx(KX3);
			N.sety(KY3);
			N.setz(KZ3);
			WN[i] = N;
			v[i].setx(KX3 * (-sin(0)) + KY3 * (cos(0)) );
			v[i].sety(KX3 * (-cos(0) * cos(0)) - KY3 * (cos(0) * sin(0)) + KZ3 * (sin(0)));
		}
		double KX1, KY1, KZ1;
		KX1 = 10;
		KY1 = 10 * cos(LX) + 10 * sin(LX);
		KZ1 = -10 * sin(LX) + 10 * cos(LX);

		double KX2, KY2, KZ2;
		KX2 = KX1 * cos(LY) - KZ1 * sin(LY);
		KY2 = KY1;
		KZ2 = KX1 * sin(LY) + KZ1 * cos(LY);

		double KX3, KY3, KZ3;
		KX3 = KX2 * cos(LZ) + KY2 * sin(LZ);
		KY3 = -KX2 * sin(LZ) + KY2 * cos(LZ);
		KZ3 = KZ2;
		
		KX3 += RRRX;// перемещаем кубик в просранстве 
		KY3 += RRRY;

		KX3 *= zoom;// изменяем размер
		KY3 *= zoom;
		KZ3 *= zoom;

		TPoint3D N;
		N.setx(KX3);
		N.sety(KY3);
		N.setz(KZ3);
		pointw = N;

	}
	
	virtual void draw() const {
		RECT rect;
		rect.left = 0;
		rect.top = 30;
		rect.right = 3000;
		rect.bottom = 2000;
		HBRUSH ki = CreateSolidBrush(RGB(15, 15, 15));
		FillRect(dc, &rect, ki);
			for (int i = 0; i < trian.size(); i = i + 3) {
				if (proverka(WN[trian[i]].getx(), WN[trian[i]].gety(), WN[trian[i]].getz(), WN[trian[i + 1]].getx(), WN[trian[i + 1]].gety(), WN[trian[i + 1]].getz(), WN[trian[i + 2]].getx(), WN[trian[i + 2]].gety(), WN[trian[i + 2]].getz(), pointw.getx(), pointw.gety(), pointw.getz())) {

					MoveToEx(dc, v[trian[i]].getx(), v[trian[i]].gety(), NULL);
					LineTo(dc, v[trian[i + 1]].getx(), v[trian[i + 1]].gety());
					LineTo(dc, v[trian[i + 2]].getx(), v[trian[i + 2]].gety());
				//	LineTo(dc, v[trian[i]].getx(), v[trian[i]].gety());
				}
			}
		Sleep(150);
	}
};


int main() {


	ifstream in;
	
	HPEN pen = CreatePen(PS_SOLID, 4, RGB(0, 255, 0));
	SelectObject(dc, pen);

	//каждый кадр мы будем закрашивать старый кубик и росовать новый со следующими параметрами которые можно будет менять нажатием на клавиши
	double teta = 1, phi = 1, zer = 1;   //углы для поворота
	double RRRX = -200, RRRY = 100;//перемещение кубика по экрану 
	double zoom = 1;//увеличение кубика


	cout << "use <- ^ v ->  N M  Z X  W A S D";


	TCube3D cub("C:\\Qt\\cube.obj");

	for (int i = 0; i < 10000; i++) {
		if (GetAsyncKeyState(38))//нажата стрелка влево 
		{
			phi += (M_PI / (360));//изменение коэфицента поворота по оси X
		}
		if (GetAsyncKeyState(40))//нажала стрелка вправо
		{
			phi -= (M_PI / (360));//изменение коэфицента поворота по оси X
		}
		if (GetAsyncKeyState(39)) {//нажата стрелка вверх
			teta += (M_PI / (360));//изменение коэфицента поворота по оси Y
		}
		if (GetAsyncKeyState(37)) {//нажата стрелка вниз
			teta -= (M_PI / (360));//изменение коэфицента поворота по оси Y
		}
		if (GetAsyncKeyState(87)) {//нажата клавиша A
			RRRY -= 10;			   //смещение кубика влево
		}
		if (GetAsyncKeyState(83)) {//нажата клавиша D
			RRRY += 10;			   //смещение кубика вправо
		}
		if (GetAsyncKeyState(65)) {//нажата клавиша S
			RRRX -= 10;			   //смещение кубика вниз
		}
		if (GetAsyncKeyState(68)) {//нажата клавиша W
			RRRX += 10;			   //смещение кубика вверх
		}
		if (GetAsyncKeyState(90)) {//нажата клавиша Z
			zoom += 0.1;		   //увелечение кубика
		}
		if (GetAsyncKeyState(88)) {//нажата клавиша X
			zoom -= 0.1;		   //уменьшение кубика
		}
		if (GetAsyncKeyState(77)) {//нажата клавиша M
			zer += (M_PI / (360)); //изменение коэфицента поворота по оси Z
		}
		if (GetAsyncKeyState(78)) {//нажата клавиша N
			zer -= (M_PI / (360)); //изменение коэфицента поворота по оси Z
		}
		//собрали инфу с клавиш, теперь рисуем по ним кубик
		cub.view_transformation(phi, teta, zer, RRRX, RRRY, zoom);
		cub.draw();
	}

	DeleteObject(pen);
	ReleaseDC(hwnd, dc);
}
