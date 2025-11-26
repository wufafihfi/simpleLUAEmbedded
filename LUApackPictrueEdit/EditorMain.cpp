#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <graphics.h>
#include <shellscalingapi.h> 
#include <curl/curl.h>
#include <string>
#include <map>
#include <lua.hpp>
#define ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_FORCE_USE_STD_ERROR
#include "easylogging++.h"

#include "LsToolsPack/LsToolsAll.hpp"
#include <codecvt>

#pragma comment(lib, "shcore.lib")


POINT ScreenWH = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

LsDrawEX::DrawData MainWindow(
    (double)(ScreenWH.x* (double)1000 / 2560), (double)(ScreenWH.y* (double)800 / 1440),
    { 0,0,0 },
    false
);
POINT mouseV2 = { 0,0 };
bool isWindowActive = false;

std::string GetExePath()
{
    char szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    std::string path = szFilePath;
    return path;
}
std::string BasePath = GetExePath();

INITIALIZE_EASYLOGGINGPP
class SimpleLogger {
public:
    static void initialize() {
        el::Configurations conf;
        conf.setToDefault();
        std::string run_logPath = GetExePath() + "\\logs\\run.log";

        std::filesystem::create_directories(std::filesystem::path(run_logPath).parent_path());

        conf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
        conf.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
        conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
        conf.set(el::Level::Global, el::ConfigurationType::Filename, run_logPath);
        // 时间 + 级别 + 消息
        conf.set(el::Level::Global, el::ConfigurationType::Format,
            "%datetime [%level] %msg");
        conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize,
            "2097152"); // 2MB

        //el::Loggers::reconfigureAllLoggers(conf);
        el::Loggers::reconfigureLogger("default", conf);
        el::Loggers::setLoggingLevel(el::Level::Info);

        LOG(WARNING) << "===LOG START===";
        LOG(INFO) << "MaxLogFileSize:2097152 Bytes = 2 MB";
    }
};

bool g_bWindowClosed = false;
// 窗口过程函数（需要自己处理消息）
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CLOSE:
        std::cout << "WM_CLOSE message received - window is closing" << std::endl;
        g_bWindowClosed = true;
        return 0; // 不传递消息，自己处理

    case WM_DESTROY:
        std::cout << "WM_DESTROY message received" << std::endl;
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

// 响应结构体 - 添加构造函数和拷贝构造函数
struct HttpResponse {
    std::string text;
    long status_code;
    bool ok;
    std::string error_message;

    // 默认构造函数
    HttpResponse() : status_code(0), ok(false) {}

    // 拷贝构造函数
    HttpResponse(const HttpResponse& other)
        : text(other.text),
        status_code(other.status_code),
        ok(other.ok),
        error_message(other.error_message) {
    }

    // 赋值运算符
    HttpResponse& operator=(const HttpResponse& other) {
        if (this != &other) {
            text = other.text;
            status_code = other.status_code;
            ok = other.ok;
            error_message = other.error_message;
        }
        return *this;
    }
};

// 回调函数，用于接收响应数据
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

// 主要的 HTTP 请求函数 - 返回指针避免拷贝问题
HttpResponse* MakeHttpRequest(const std::string& url,
    const std::string& method = "GET",
    const std::string& data = "",
    const std::map<std::string, std::string>& headers = {}) {

    CURL* curl;
    CURLcode res;
    HttpResponse* response = new HttpResponse();

    curl = curl_easy_init();
    if (!curl) {
        response->error_message = "Failed to initialize cURL";
        return response;
    }

    std::string response_text;

    // 设置 URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // 设置 HTTP 方法
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (!data.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
        }
    }
    else if (method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        if (!data.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }
    }
    else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    else if (method == "HEAD") {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    }

    // 设置响应回调
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_text);

    // 设置超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // 跟随重定向
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);

    // 设置自定义头部
    struct curl_slist* header_list = nullptr;
    for (const auto& header : headers) {
        std::string header_str = header.first + ": " + header.second;
        header_list = curl_slist_append(header_list, header_str.c_str());
    }
    if (header_list) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    // 执行请求
    res = curl_easy_perform(curl);

    // 获取状态码
    long http_code = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        response->ok = (http_code >= 200 && http_code < 300);
        response->status_code = http_code;
        response->text = response_text;
    }
    else {
        response->status_code = 0;
        response->error_message = "cURL error: " + std::string(curl_easy_strerror(res));
    }

    // 清理
    if (header_list) {
        curl_slist_free_all(header_list);
    }
    curl_easy_cleanup(curl);

    return response;
}

// LUA
static int cpp_print(lua_State* L) {
    const char* input_str = luaL_checkstring(L, 1);
    std::string str = std::string(input_str);
    std::cout << "[LUA] " << str << std::endl;
    return 0;
}
static int custom_print(lua_State* L) {
    int n = lua_gettop(L);
    std::stringstream ss;

    for (int i = 1; i <= n; i++) {
        if (i > 1) ss << "\t";

        const char* str = luaL_tolstring(L, i, NULL);
        if (str) {
            ss << str;
        }
        lua_pop(L, 1);
    }

    std::cout << "[LUA] " << ss.str() << std::endl;
    return 0;
}
static int cpp_GetWindowWH(lua_State* L) {
    lua_createtable(L, 0, 2);

    lua_pushinteger(L, MainWindow.WindowW);
    lua_setfield(L, -2, "weight");

    lua_pushinteger(L, MainWindow.WindowH);
    lua_setfield(L, -2, "high");
    return 1;
}
static int cpp_GetMouse(lua_State* L) {
    lua_createtable(L, 0, 2);

    lua_pushinteger(L, mouseV2.x);
    lua_setfield(L, -2, "x");

    lua_pushinteger(L, mouseV2.y);
    lua_setfield(L, -2, "y");
    return 1;
}
static int cpp_DrawLine(lua_State* L) {
    int argc = lua_gettop(L);
    if (argc != 3) {
        lua_pushstring(L, "need 3 parameters");
        lua_error(L);
        return 0;
    }

    if (!lua_istable(L, 1) || !lua_istable(L, 2) || !lua_istable(L, 3)) {
        lua_pushstring(L, "parameters erro,parameters:(x1,y1)[table],(x2,y2)[table],(RGB)[table]");
        lua_error(L);
        return 0;
    }

    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    double r = 255, g = 255, b = 255;
    
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    if (lua_isnumber(L, -2)) x1 = lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) y1 = lua_tonumber(L, -1);
    lua_pop(L, 2);

    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    if (lua_isnumber(L, -2)) x2 = lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) y2 = lua_tonumber(L, -1);
    lua_pop(L, 2);

    lua_getfield(L, 3, "r");
    lua_getfield(L, 3, "g");
    lua_getfield(L, 3, "b");
    if (lua_isnumber(L, -3)) r = lua_tonumber(L, -3);
    if (lua_isnumber(L, -2)) g = lua_tonumber(L, -2);
    if (lua_isnumber(L, -1)) b = lua_tonumber(L, -1);
    lua_pop(L, 3);

    setlinecolor(RGB(r, g, b));
    line(x1, y1, x2, y2);

    return 0;
}
static int cpp_DrawCircle(lua_State* L) {
    // 参数数量
    if (lua_gettop(L) != 3) {
        return luaL_error(L, "need 3 parameters");
    }

    // 类型
    luaL_checktype(L, 1, LUA_TTABLE);  // 坐标table
    luaL_checktype(L, 2, LUA_TNUMBER); // 半径
    luaL_checktype(L, 3, LUA_TTABLE);  // 颜色table

    // 获取
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    double x = luaL_optnumber(L, -2, 0);
    double y = luaL_optnumber(L, -1, 0);
    lua_pop(L, 2);

    double radius = lua_tonumber(L, 2);

    // 颜色
    lua_getfield(L, 3, "r");
    lua_getfield(L, 3, "g");
    lua_getfield(L, 3, "b");
    double r = luaL_optnumber(L, -3, 255);
    double g = luaL_optnumber(L, -2, 255);
    double b = luaL_optnumber(L, -1, 255);
    lua_pop(L, 3);

    setlinecolor(RGB(r, g, b));
    circle(x, y, radius);

    return 0;
}
static int cpp_DrawFillCircle(lua_State* L) {
    // 检查参数数量
    if (lua_gettop(L) != 4) {
        return luaL_error(L, "need 3 parameters");
    }

    // 检查参数类型
    luaL_checktype(L, 1, LUA_TTABLE);  // 坐标table
    luaL_checktype(L, 2, LUA_TNUMBER); // 半径
    luaL_checktype(L, 3, LUA_TTABLE);  // 颜色table
    luaL_checktype(L, 4, LUA_TTABLE);  // 颜色table

    // 获取坐标
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    double x = luaL_optnumber(L, -2, 0);
    double y = luaL_optnumber(L, -1, 0);
    lua_pop(L, 2);

    // 获取半径
    double radius = lua_tonumber(L, 2);

    // 获取颜色
    lua_getfield(L, 3, "r");
    lua_getfield(L, 3, "g");
    lua_getfield(L, 3, "b");
    double r = luaL_optnumber(L, -3, 255);
    double g = luaL_optnumber(L, -2, 255);
    double b = luaL_optnumber(L, -1, 255);
    lua_pop(L, 3);

    lua_getfield(L, 3, "r");
    lua_getfield(L, 3, "g");
    lua_getfield(L, 3, "b");
    double Fr = luaL_optnumber(L, -3, 255);
    double Fg = luaL_optnumber(L, -2, 255);
    double Fb = luaL_optnumber(L, -1, 255);
    lua_pop(L, 3);

    setlinecolor(RGB(r, g, b));
    setfillcolor(RGB(Fr, Fg, Fb));
    fillcircle(x, y, radius);

    return 0;
}
static int cpp_DrawFillRectangle(lua_State* L) {
    // 检查参数数量
    if (lua_gettop(L) != 4) {
        return luaL_error(L, "need 4 parameters");
    }

    // 检查参数类型
    luaL_checktype(L, 1, LUA_TTABLE);  // 坐标table
    luaL_checktype(L, 2, LUA_TTABLE); // 坐标2table
    luaL_checktype(L, 3, LUA_TTABLE);  // 颜色table
    luaL_checktype(L, 4, LUA_TTABLE);  // 颜色table

    // 获取坐标
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    double x = luaL_optnumber(L, -2, 0);
    double y = luaL_optnumber(L, -1, 0);
    lua_pop(L, 2);

    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    double x1 = luaL_optnumber(L, -2, 0);
    double y1 = luaL_optnumber(L, -1, 0);
    lua_pop(L, 2);

    // 获取颜色
    lua_getfield(L, 3, "r");
    lua_getfield(L, 3, "g");
    lua_getfield(L, 3, "b");
    double r = luaL_optnumber(L, -3, 255);
    double g = luaL_optnumber(L, -2, 255);
    double b = luaL_optnumber(L, -1, 255);
    lua_pop(L, 3);

    lua_getfield(L, 3, "r");
    lua_getfield(L, 3, "g");
    lua_getfield(L, 3, "b");
    double Fr = luaL_optnumber(L, -3, 255);
    double Fg = luaL_optnumber(L, -2, 255);
    double Fb = luaL_optnumber(L, -1, 255);
    lua_pop(L, 3);

    setlinecolor(RGB(r, g, b));
    setfillcolor(RGB(Fr, Fg, Fb));
    fillrectangle(x, y, x1, y1);

    return 0;
}
// 按键检测 按键是否按下（按下则true）
static int cpp_GetKeyPress(lua_State* L) {
    // 参数数量
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "need 2 parameters");
    }

    // 检查参数类型
    luaL_checktype(L, 1, LUA_TNUMBER);  // 键值
    luaL_checktype(L, 2, LUA_TNUMBER);  // 模式 0:窗口活动时才返回true 1:无其他条件，对应键值按下返回true

    int Key_ = lua_tonumber(L, 1);
    int Mode_ = lua_tonumber(L, 2);

    bool isKeyState = false;
    //https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
    if (GetAsyncKeyState(Key_)){
        if (Mode_ == 0) {
            isKeyState = true;
        }
        if (Mode_ == 1) {
            if(isWindowActive)
            {
                isKeyState = true;
            }
        }
    }

    // 返回bool参数
    lua_pushboolean(L, isKeyState);

    return 1;
}
// 按键检测 按键是否按下（返回一次true）
static int cpp_GetKeyPress_oneR(lua_State* L) {
    // 参数数量
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "need 2 parameters");
    }

    // 检查参数类型
    luaL_checktype(L, 1, LUA_TNUMBER);  // 键值
    luaL_checktype(L, 2, LUA_TNUMBER);  // 模式 0:窗口活动时才返回true 1:无其他条件，对应键值按下返回true

    int Key_ = lua_tonumber(L, 1);
    int Mode_ = lua_tonumber(L, 2);

    bool isKeyState = false;
    //https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
    if (GetAsyncKeyState(Key_) & 0x01) {
        if (Mode_ == 0) {
            isKeyState = true;
        }
        if (Mode_ == 1) {
            if (isWindowActive)
            {
                isKeyState = true;
            }
        }
    }

    // 返回bool参数
    lua_pushboolean(L, isKeyState);

    return 1;
}
// 单个 HTTP GET 函数
static int cpp_http_get(lua_State* L) {
    // 参数检查
    if (lua_gettop(L) != 1) {
        return luaL_error(L, "http_get need 1 parameter: url");
    }

    luaL_checktype(L, 1, LUA_TSTRING);
    const char* url = lua_tostring(L, 1);

    HttpResponse* http_response = MakeHttpRequest(url, "GET");

    // 创建返回表
    lua_newtable(L);

    lua_pushstring(L, "text");
    lua_pushstring(L, http_response->text.c_str());
    lua_settable(L, -3);

    lua_pushstring(L, "status_code");
    lua_pushinteger(L, http_response->status_code);
    lua_settable(L, -3);

    lua_pushstring(L, "ok");
    lua_pushboolean(L, http_response->ok);
    lua_settable(L, -3);

    lua_pushstring(L, "error_message");
    if (!http_response->error_message.empty()) {
        lua_pushstring(L, http_response->error_message.c_str());
    }
    else {
        lua_pushnil(L);
    }
    lua_settable(L, -3);

    // 释放内存
    delete http_response;

    return 1;
}

// 单个 HTTP POST 函数
static int cpp_http_post(lua_State* L) {
    // 参数检查
    int argc = lua_gettop(L);
    if (argc < 1 || argc > 2) {
        return luaL_error(L, "http_post need 1 or 2 parameters: url, [data]");
    }

    luaL_checktype(L, 1, LUA_TSTRING);
    const char* url = lua_tostring(L, 1);
    const char* data = (argc >= 2) ? lua_tostring(L, 2) : "";

    HttpResponse* http_response = MakeHttpRequest(url, "POST", data);

    // 创建返回表
    lua_newtable(L);
    lua_pushstring(L, "text");
    lua_pushstring(L, http_response->text.c_str());
    lua_settable(L, -3);

    lua_pushstring(L, "status_code");
    lua_pushinteger(L, http_response->status_code);
    lua_settable(L, -3);

    lua_pushstring(L, "ok");
    lua_pushboolean(L, http_response->ok);
    lua_settable(L, -3);

    lua_pushstring(L, "error_message");
    if (!http_response->error_message.empty()) {
        lua_pushstring(L, http_response->error_message.c_str());
    }
    else {
        lua_pushnil(L);
    }
    lua_settable(L, -3);

    delete http_response;
    return 1;
}
// HTTP 请求函数
static int cpp_http_request(lua_State* L) {
    // 参数检查
    int argc = lua_gettop(L);
    if (argc < 2 || argc > 3) {
        return luaL_error(L, "http_request need 2 or 3 parameters: method, url, [data]");
    }

    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);

    const char* method = lua_tostring(L, 1);
    const char* url = lua_tostring(L, 2);
    const char* data = (argc >= 3) ? lua_tostring(L, 3) : "";

    HttpResponse* http_response = MakeHttpRequest(url, method, data);

    // 创建返回表
    lua_newtable(L);
    lua_pushstring(L, "text");
    lua_pushstring(L, http_response->text.c_str());
    lua_settable(L, -3);

    lua_pushstring(L, "status_code");
    lua_pushinteger(L, http_response->status_code);
    lua_settable(L, -3);

    lua_pushstring(L, "ok");
    lua_pushboolean(L, http_response->ok);
    lua_settable(L, -3);

    lua_pushstring(L, "error_message");
    if (!http_response->error_message.empty()) {
        lua_pushstring(L, http_response->error_message.c_str());
    }
    else {
        lua_pushnil(L);
    }
    lua_settable(L, -3);

    delete http_response;
    return 1;
}

// 带 headers 的高级 HTTP 请求函数
static int cpp_http_request_ex(lua_State* L) {
    // 参数检查
    int argc = lua_gettop(L);
    if (argc < 2 || argc > 4) {
        return luaL_error(L, "http_request_ex need 2-4 parameters: method, url, [data], [headers]");
    }

    luaL_checktype(L, 1, LUA_TSTRING);
    luaL_checktype(L, 2, LUA_TSTRING);

    const char* method = lua_tostring(L, 1);
    const char* url = lua_tostring(L, 2);
    const char* data = (argc >= 3) ? lua_tostring(L, 3) : "";

    // 处理 headers 表（可选）
    std::map<std::string, std::string> headers;
    if (argc >= 4 && lua_istable(L, 4)) {
        lua_pushnil(L);
        while (lua_next(L, 4) != 0) {
            if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
                const char* key = lua_tostring(L, -2);
                const char* value = lua_tostring(L, -1);
                headers[key] = value;
            }
            lua_pop(L, 1);
        }
    }

    HttpResponse* http_response = MakeHttpRequest(url, method, data, headers);

    // 创建返回表
    lua_newtable(L);
    lua_pushstring(L, "text");
    lua_pushstring(L, http_response->text.c_str());
    lua_settable(L, -3);

    lua_pushstring(L, "status_code");
    lua_pushinteger(L, http_response->status_code);
    lua_settable(L, -3);

    lua_pushstring(L, "ok");
    lua_pushboolean(L, http_response->ok);
    lua_settable(L, -3);

    lua_pushstring(L, "error_message");
    if (!http_response->error_message.empty()) {
        lua_pushstring(L, http_response->error_message.c_str());
    }
    else {
        lua_pushnil(L);
    }
    lua_settable(L, -3);

    delete http_response;
    return 1;
}
// API方法注册
static int cpp_GETAPI(lua_State* L) {
    // 创建API table
    lua_createtable(L, 0, 12);  // 数量要对

    // 注册
    lua_pushcfunction(L, cpp_GetWindowWH);
    lua_setfield(L, -2, "GetWindowWH");

    lua_pushcfunction(L, cpp_GetMouse);
    lua_setfield(L, -2, "GetMouse");

    lua_pushcfunction(L, cpp_DrawLine);
    lua_setfield(L, -2, "DrawLine");

    lua_pushcfunction(L, cpp_DrawCircle);
    lua_setfield(L, -2, "DrawCircle");

    lua_pushcfunction(L, cpp_DrawFillCircle);
    lua_setfield(L, -2, "DrawFillCircle");

    lua_pushcfunction(L, cpp_DrawFillRectangle);
    lua_setfield(L, -2, "DrawFillRectangle");

    lua_pushcfunction(L, cpp_GetKeyPress);
    lua_setfield(L, -2, "GetKeyPress");

    lua_pushcfunction(L, cpp_GetKeyPress_oneR);
    lua_setfield(L, -2, "GetKeyPress_oneR");

    // HTTP函数
    lua_pushcfunction(L, cpp_http_get);
    lua_setfield(L, -2, "http_get");

    lua_pushcfunction(L, cpp_http_post);
    lua_setfield(L, -2, "http_post");

    lua_pushcfunction(L, cpp_http_request);
    lua_setfield(L, -2, "http_request");

    lua_pushcfunction(L, cpp_http_request_ex);
    lua_setfield(L, -2, "http_request_ex");

    return 1;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    char* argv[] = { nullptr };
    START_EASYLOGGINGPP(0, argv);
    // 初始化日志配置
    SimpleLogger::initialize();

    HWND main_hwnd = LsDrawEX::initDraw(MainWindow);
    setbkmode(TRANSPARENT);
    SetWindowLongPtr(main_hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
    LOG(INFO) << "MainWindow created successfully";

    // 分配一个新的控制台窗口
    AllocConsole();
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "r", stdin);
    freopen_s(&stream, "CONOUT$", "w", stderr);

    // 创建一个简单的窗口（可选，如果需要处理窗口消息）
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"CandLUA_ped_Debug";
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        0,                              // 扩展样式
        L"CandLUA_ped_Debug",          // 窗口类名
        L"编辑器调试",                 // 窗口标题
        WS_OVERLAPPEDWINDOW,           // 风格
        CW_USEDEFAULT, CW_USEDEFAULT,  // x, y 位置
        CW_USEDEFAULT, CW_USEDEFAULT,  // 宽, 高
        NULL,                          // 父窗口
        NULL,                          // 菜单
        hInstance,                     // 实例句柄
        NULL                           // 创建参数
    );
    if (hwnd == NULL) {
        std::cerr << "Failed to create window" << std::endl;
        LOG(ERROR) << "Failed to create window";
        return 1;
    }
    // 隐藏窗口（如果不需要显示窗口）
    ShowWindow(hwnd, SW_HIDE);

    // LUA
    lua_State* L = luaL_newstate();
    luaL_openlibs(L); 
    // 注册
    lua_register(L, "print", custom_print);
    lua_register(L, "GetBzdAPI", cpp_GETAPI);

    std::string LuaFilePath = GetExePath() + "\\EditorCore.lua";
    if (luaL_dofile(L, LuaFilePath.c_str()) != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::cerr << "LUA 错误: " << error << std::endl;
        LOG(ERROR) << "LUA 错误: " << error;
        lua_pop(L, 1); // 弹出错误信息
        lua_close(L);
        if(error != nullptr) //<-这里和if空了个格,不然注释会暗掉// 听说这样会更健壮
        {
            std::string message_STR = "LUA 错误:" + std::string(error);
            MessageBoxA(main_hwnd, message_STR.c_str(), "程序因错误停止", MB_ICONERROR | MB_OK);
        }
        else
        {
            MessageBoxA(main_hwnd, "未知错误", "程序因错误停止", MB_ICONERROR | MB_OK);
        }
        return false;
    }

    // 检查update函数是否存在
    lua_getglobal(L, "update");
    bool hasUpdateFunction = lua_isfunction(L, -1);

    if (!hasUpdateFunction) {
        std::cerr << "LUA脚本中无update方法，update为入口方法" << std::endl;
        LOG(WARNING) << "LUA脚本中无update方法，update为入口方法";
        lua_pop(L, 1); // 弹出非函数的值
    }

    LsDrawEX::FPScontorl MainFPS;
    MainFPS.init_FPS(120);

    while (!g_bWindowClosed) {
        LsDrawEX::GetMouse(main_hwnd, mouseV2);
        isWindowActive = LsDrawEX::isWindowActive(main_hwnd);
        MainFPS.time_Begin();

        if (hasUpdateFunction) {
            // 每次循环都需要重新获取update函数
            lua_getglobal(L, "update");

            // 调用LUA的update函数
            int pcallResult = lua_pcall(L, 0, LUA_MULTRET, 0);
            if (pcallResult != LUA_OK) {
                const char* error = lua_tostring(L, -1);
                std::cerr << "LUA 运行错误: " << error << std::endl;
                LOG(ERROR) << "LUA 运行错误: " << error;
                lua_pop(L, 1); // 弹出错误信息
                // 可以选择继续或退出
            }
            else {
                // 清理返回值，保持栈平衡
                int returnCount = lua_gettop(L);
                if (returnCount > 0) {
                    lua_pop(L, returnCount);
                }
            }
        }
        else {
            // 如果没有update函数，执行一些默认操作
            static int frameCount = 0;
            frameCount++;
            if (frameCount % 60 == 0) {
                std::cout << "无update方法,持续帧数: " << frameCount << std::endl;
            }
        }

        LsDrawEX::drawFlush();
        MainFPS.time_Sleep();
    }

    LOG(INFO) << "程序关闭";
    lua_close(L);

    // 清理控制台
    FreeConsole();

    return 0;
}