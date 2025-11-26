local DkJson = require("dependency\\dkjson")
local bzdAPI = GetBzdAPI()

-- 读取
function load_config(filename)
    local file = io.open(filename, "r")
    if not file then return nil end
    
    local content = file:read("*a")
    file:close()
    
    return DkJson.decode(content)
end
-- 保存
function save_config(filename, config)
    local json_str = DkJson.encode(config, {indent = true})
    local file = io.open(filename, "w")
    file:write(json_str)
    file:close()
end

local windowHW = bzdAPI.GetWindowWH()
local MousePosition = bzdAPI.GetMouse()

local mjson = load_config("testJson.json")

-- 入口函数
function update()
    MousePosition = bzdAPI.GetMouse()
    print(mjson["cnm"])
    print(mjson["ntm"])
    bzdAPI.DrawLine({x=0,y=0},{x=windowHW.weight,y=windowHW.high},{r=100,g=255,b=100})
    bzdAPI.DrawLine({x=windowHW.weight/2 - 50,y=windowHW.high/2 + 50},{x=windowHW.weight/2 + 50,y=windowHW.high/2 - 50},{r=100,g=255,b=100})
    bzdAPI.DrawCircle({x=MousePosition.x,y=MousePosition.y},windowHW.high/2-1,{r=255,g=255,b=100})
    bzdAPI.DrawFillCircle({x=windowHW.weight/2,y=windowHW.high/2},10,{r=255,g=255,b=100},{r=100,g=255,b=255})
    bzdAPI.DrawFillRectangle({x=10,y=20},{x=100,y=200},{r=255,g=255,b=100},{r=255,g=100,b=255})
    if(bzdAPI.GetKeyPress_oneR(0x01,0))then
        print("key is pressing!")
    end
end