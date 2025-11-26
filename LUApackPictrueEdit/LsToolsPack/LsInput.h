#ifndef LSINPUT
#define LSINPUT

#include <iostream>

namespace LsInput {
    class FHInput {
    private:
        // 非静态成员变量
        bool keyFlag = false;
        bool keyFlag_Long = false;
        bool keyDown = false;
        bool keyDown_Long = false;

    public:
        // 静态成员函数声明
        FHInput();

        // 非静态成员函数声明
        bool GetKeyPress(bool isTrue);
        bool GetKeyPressLong(bool isTrue);
        bool GetKeyRelese(bool isTrue);

        // 初始化成员变量
        void Initialize() {
            keyFlag = false;
            keyFlag_Long = false;
            keyDown = false;
            keyDown_Long = false;
        }
    };
}

#endif