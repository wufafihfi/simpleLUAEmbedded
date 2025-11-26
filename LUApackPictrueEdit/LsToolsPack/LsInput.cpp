#include "LsInput.h"

namespace LsInput {
    // 静态成员函数实现，用于创建类的实例
    FHInput::FHInput() {
        Initialize();
    }
    // 按键按下（脉冲）
    bool FHInput::GetKeyPress(bool isTrue)
    {
        if (isTrue)
        {
            if (!keyFlag && !keyDown)
            {
                keyFlag = true;
                keyDown = true;

                return true;
            }
        }
        else
        {
            keyFlag = false;
            keyDown = false;
        }

        return false;
    }
    // 按键按下（开关）
    bool FHInput::GetKeyPressLong(bool isTrue)
    {
        if (isTrue)
        {
            if (!keyDown_Long)
            {
                keyFlag_Long = !keyFlag_Long;
                keyDown_Long = true;
            }
        }
        else
        {
            keyDown_Long = false;
        }

        if (keyFlag_Long)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    // 按键松开
    bool FHInput::GetKeyRelese(bool isTrue) {
        if (isTrue && !keyFlag) {
            keyFlag = true;
        }
        if (!isTrue && keyFlag) {
            keyFlag = false;
            return true;
        }
        return false;
    }
}