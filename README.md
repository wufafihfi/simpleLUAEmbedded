# BzdAPI
## 获取APItable
```lua
local bzdAPI = GetBzdAPI()
```
## 绘制相关的function
```lua
bzdAPI.DrawLine({x=0,y=0},{x=0,y=0},{r=255,g=255,b=255})
bzdAPI.DrawCircle({x=0,y=0},<半径>10,{r=255,g=255,b=255})
bzdAPI.DrawFillCircle({x=0,y=0},<半径>10,<填充色>{r=255,g=255,b=255},<边框色>{r=255,g=255,b=255})
bzdAPI.DrawFillRectangle({x=10,y=20},{x=100,y=200},{r=255,g=255,b=100},{r=255,g=100,b=255})
```
## 交互相关function
```lua
--检测按键按下（按下按键只返回一次true,需要松开按键才能继续）
bzdAPI.GetKeyPress_oneR(<虚拟键码>0x01,<检测模式,0为窗口活动才检测按键,1为程序可以检测到任何时候的按键>0)
--检测按键按下（按下按键则返回true,松开则为false）
bzdAPI.GetKeyPress(<虚拟键码>0x01,<检测模式,0为窗口活动才检测按键,1为程序可以检测到任何时候的按键>0)
```
## 数据获取相关function
```lua
bzdAPI.GetWindowWH() --获取窗口宽高 table格式{weight = <数字>,high = <数字>}
bzdAPI.GetMouse() --获取鼠标位置 table格式{x = <数字>,y = <数字>}
```
