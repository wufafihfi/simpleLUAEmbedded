#ifndef LSWS
#define LSWS

#include <windows.h>
#include <string>

namespace LsWS {
	void setWindowTransparent(const std::string& winname, COLORREF colorKey, BYTE alpha = 255);
	void setWindowTransparent(HWND& hwnd, COLORREF colorKey, BYTE alpha = 255);

	void setWindowTransparent_Tool(const std::string& winname, COLORREF colorKey, BYTE alpha = 255);
	void setWindowTransparent_Tool(HWND& hwnd, COLORREF colorKey, BYTE alpha = 255);

	void setWindowTransparent_Tool_MouseT(const std::string& winname, COLORREF colorKey, BYTE alpha = 255);
	void setWindowTransparent_Tool_MouseT(HWND& hwnd, COLORREF colorKey, BYTE alpha = 255);

	void setWindowAlpha(const std::string& winname, COLORREF colorKey, BYTE Alpha);
	void setWindowAlpha(HWND& hwnd, COLORREF colorKey, BYTE Alpha);
}

#endif