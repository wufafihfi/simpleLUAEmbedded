#include "LsWinodwStyle.h"

namespace LsWS {
    // 设置窗口透明和无边框
    void setWindowTransparent(const std::string& winname, COLORREF colorKey, BYTE alpha) {
        HWND hwnd = FindWindowA(NULL, winname.c_str());
        if (hwnd) {
            // 无边框
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // 最大化
            ShowWindow(hwnd, SW_MAXIMIZE);

            // 透明
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // 设置位置和大小
            SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY);
        }
    }
    void setWindowTransparent(HWND& hwnd, COLORREF colorKey, BYTE alpha) {
        if (hwnd) {
            // 无边框
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // 最大化
            ShowWindow(hwnd, SW_MAXIMIZE);

            // 透明
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // 设置位置和大小
            SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY);
        }
    }
    // 设置窗口透明和无边框 工具
    void setWindowTransparent_Tool(const std::string& winname, COLORREF colorKey, BYTE alpha) {
        HWND hwnd = FindWindowA(NULL, winname.c_str());
        if (hwnd) {
            // 无边框
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // 最大化
            ShowWindow(hwnd, SW_MAXIMIZE);

            // 透明和工具窗口样式
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // 设置位置和大小
            SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY);
        }
    }
    void setWindowTransparent_Tool(HWND& hwnd, COLORREF colorKey, BYTE alpha) {
        if (hwnd) {
            // 无边框
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // 最大化
            ShowWindow(hwnd, SW_MAXIMIZE);

            // 透明和工具窗口样式
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // 设置位置和大小
            SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY);
        }
    }
    // 设置窗口透明和无边框 工具 鼠标穿透
    void setWindowTransparent_Tool_MouseT(const std::string& winname, COLORREF colorKey, BYTE alpha) {
        HWND hwnd = FindWindowA(NULL, winname.c_str());
        if (hwnd) {
            // 无边框
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // 最大化
            ShowWindow(hwnd, SW_MAXIMIZE);

            // 透明和工具窗口样式
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // 设置位置和大小
            SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY);
        }
    }
    void setWindowTransparent_Tool_MouseT(HWND& hwnd, COLORREF colorKey, BYTE alpha) {
        if (hwnd) {
            // 无边框
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // 最大化
            ShowWindow(hwnd, SW_MAXIMIZE);

            // 透明和工具窗口样式
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // 设置位置和大小
            SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY);
        }
    }
    // 设置窗口透明化/半透
    void setWindowAlpha(const std::string& winname, COLORREF colorKey, BYTE Alpha) {
        HWND hwnd = FindWindowA(NULL, winname.c_str());
        if (hwnd) {
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            exStyle = exStyle | WS_EX_LAYERED;
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
            SetLayeredWindowAttributes(hwnd, colorKey, Alpha, LWA_ALPHA);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        }
    }
    void setWindowAlpha(HWND& hwnd, COLORREF colorKey, BYTE Alpha) {
        if (hwnd) {
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            exStyle = exStyle | WS_EX_LAYERED;
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
            SetLayeredWindowAttributes(hwnd, colorKey, Alpha, LWA_ALPHA);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        }
    }
}