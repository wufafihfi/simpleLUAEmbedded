#include <thread>
#include <windows.h>
#include <graphics.h>
#include <iostream>
#include <limits>
#include "LsdrawEX.h"

namespace LsDrawEX
{
	double Run_T = 0;
	double& getRun_T() {
		return Run_T;
	}
	POINT mousePosition = { 0,0 };
	POINT& getMousePosition() {
		return mousePosition;
	}

	double FPScontorl::time_Sleep() {
		frameTime = startTime - endTime;
		endTime = startTime;
		if (frameTime.count() < FPS) {
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(FPS - frameTime.count())));
		}
		return frameTime.count();
	}
	void FPScontorl::time_Begin() {
		startTime = std::chrono::high_resolution_clock::now();
	}
	void FPScontorl::init_FPS(int setFPS) {
		_FPS = setFPS;
		FPS = 1000 / _FPS;// 1S/120FPS = 8.3(1帧所用时间）(1000ms/fps = freamTime)
	}

	COLORREF LsRGB::getRGB_EasyX() {
		return RGB(r, g, b);
	}

	DrawData::DrawData(int SetWindowW, int SetWindowH, LsRGB SetbkColor, bool SetDebugMode) {
		WindowW = SetWindowW;
		WindowH = SetWindowH;
		bkColor = SetbkColor;
		DebugMode = SetDebugMode;
	}

	// 窗口全屏（终极）
	BOOL OnForceShow_M(HWND hWnd, double cX, double cY)
	{
		HWND hForeWnd = NULL;
		DWORD dwForeID = 0;
		DWORD dwCurID = 0;

		hForeWnd = ::GetForegroundWindow();
		dwCurID = ::GetCurrentThreadId();
		dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
		::AttachThreadInput(dwCurID, dwForeID, TRUE);
		::ShowWindow(hWnd, SW_SHOWNORMAL);
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetForegroundWindow(hWnd);
		// 将前台窗口线程贴附到当前线程（也就是程序A中的调用线程）
		::AttachThreadInput(dwCurID, dwForeID, FALSE);

		//SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_EX_LAYERED); // 去掉标题栏
		::SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));

		::SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, cX, cY, SWP_FRAMECHANGED); // 设置位置和大小

		return TRUE;
	}
	// 窗口全屏
	BOOL OnForceShow(HWND hwnd, double cX, double cY)
	{
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_EX_LAYERED); // 去掉标题栏
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));

		::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED);
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, cX, cY, SWP_FRAMECHANGED); // 设置位置和大小

		return TRUE;
	}
	BOOL WindowAlpha(HWND hwnd, int bkAlpha)
	{
		// 透明
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));

		//::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		SetLayeredWindowAttributes(hwnd, 0x000000, bkAlpha, LWA_ALPHA | LWA_COLORKEY);

		return TRUE;
	}

	//绘图初始化
	HWND initDraw(DrawData initDrawData) {
		// 窗口创建
		initgraph(initDrawData.WindowW, initDrawData.WindowH, initDrawData.DebugMode);
		HWND hwnd = GetHWnd();

		//颜色
		setbkcolor(initDrawData.bkColor.getRGB_EasyX());

		//双缓冲
		BeginBatchDraw();

		return hwnd;
	}

	bool isWindowActive(HWND hwnd) {
		// 获取当前活动窗口的句柄
		HWND activeWindow = GetForegroundWindow();
		// 比较传入的窗口句柄与当前活动窗口的句柄是否相同
		return activeWindow == hwnd;
	}

	//刷新
	void drawFlush() {
		Run_T += 0.001;
		if (Run_T >= (DBL_MAX - 1)) {
			Run_T = 0;
		}
		// 刷新
		FlushBatchDraw();
		// 清空
		cleardevice();
	}

	void GetMouse(HWND& hwnd, POINT& mousePosition)
	{
		GetCursorPos(&mousePosition);// 获取标坐标 
		ScreenToClient(hwnd, &mousePosition);
	}

	// 绘制透明矩形的函数
	void drawTransparentRect(int x, int y, int width, int height, COLORREF color, int alpha)
	{
		// 获取主绘图设备上下文
		DWORD* dst = GetImageBuffer();
		int graphWidth = getwidth();
		int graphHeight = getheight();

		// 计算矩形边界
		int xEnd = x + width;
		int yEnd = y + height;

		// 确保在有效范围内绘制
		int startX = max(0, x);
		int startY = max(0, y);
		int endX = min(graphWidth, xEnd);
		int endY = min(graphHeight, yEnd);

		// 提取颜色分量
		int sr = GetRValue(color);
		int sg = GetGValue(color);
		int sb = GetBValue(color);

		// Alpha混合计算
		for (int iy = startY; iy < endY; iy++)
		{
			for (int ix = startX; ix < endX; ix++)
			{
				int dstX = ix + iy * graphWidth;

				// 获取目标位置的颜色
				int dr = (dst[dstX] & 0xff0000) >> 16;
				int dg = (dst[dstX] & 0xff00) >> 8;
				int db = dst[dstX] & 0xff;

				// Alpha混合公式: result = (source * alpha) + (destination * (1 - alpha))
				int r = (sr * alpha + dr * (255 - alpha)) / 255;
				int g = (sg * alpha + dg * (255 - alpha)) / 255;
				int b = (sb * alpha + db * (255 - alpha)) / 255;

				// 写回显存
				dst[dstX] = RGB(r, g, b);
			}
		}
	}

	// 绘制带边框的透明矩形
	void drawTransparentRectWithBorder(int x, int y, int width, int height, COLORREF fillColor, int alpha, COLORREF borderColor, int borderWidth)
	{
		// 绘制填充矩形
		drawTransparentRect(x + borderWidth, y + borderWidth,
			width - 2 * borderWidth, height - 2 * borderWidth,
			fillColor, alpha);

		// 绘制边框
		setlinecolor(borderColor);
		setlinestyle(PS_SOLID, borderWidth);
		rectangle(x, y, x + width, y + height);
	}
}
