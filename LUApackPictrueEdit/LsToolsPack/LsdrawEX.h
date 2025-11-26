#ifndef LSDRAWEX
#define LSDRAWEX

#include <chrono>
#include <windows.h>

namespace LsDrawEX
{
	double& getRun_T();
	POINT& getMousePosition();

	class FPScontorl
	{
	public:
		int _FPS = 120;
		//帧限制
		clock_t FPS = 1000 / _FPS;// 1S/120FPS = 8.3(1帧所用时间）(1000ms/fps = freamTime)
		std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		std::chrono::steady_clock::time_point endTime = startTime;
		std::chrono::duration<double, std::milli> frameTime;
		//初始化
		void init_FPS(int setFPS);
		//帧处理函数
		double time_Sleep();
		void time_Begin();
	};

	//颜色数据
	struct LsRGB {
	public:
		double r = 0;
		double g = 0;
		double b = 0;

		COLORREF getRGB_EasyX();
	};

	//绘图窗口数据
	//绘图窗口数据
	class DrawData {
	public:
		int WindowW = 500, WindowH = 500;
		LsRGB bkColor = { 0,0,0 };
		bool DebugMode = false;

		DrawData(int SetWindowW, int SetWindowH, LsRGB SetbkColor, bool SetDebugMode);
	};

	HWND initDraw(DrawData initDrawData);

	BOOL OnForceShow_M(HWND hWnd, double cX, double cY);
	BOOL OnForceShow(HWND hwnd, double cX, double cY);
	BOOL WindowAlpha(HWND hwnd, int bkAlpha);

	bool isWindowActive(HWND hwnd);

	void drawFlush();

	void GetMouse(HWND& hwnd, POINT& mousePosition);

	void drawTransparentRect(int x, int y, int width, int height, COLORREF color, int alpha);
	void drawTransparentRectWithBorder(int x, int y, int width, int height, COLORREF fillColor, int alpha, COLORREF borderColor, int borderWidth);
}

#endif