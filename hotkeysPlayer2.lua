
keyboardHotkeys = {
    -- movement
    --{["name"]="horizontal", ["key"]="SDLK_LEFT", ["axisValue"]=-1},
    --{["name"]="horizontal", ["key"]="SDLK_RIGHT", ["axisValue"]=1},
    --{["name"]="vertical", ["key"]="SDLK_UP", ["axisValue"]=-1},
    --{["name"]="vertical", ["key"]="SDLK_DOWN", ["axisValue"]=1},
    
    {["name"]="horizontal", ["key"]="SDLK_KP_4", ["axisValue"]=-1},
    {["name"]="horizontal", ["key"]="SDLK_KP_6", ["axisValue"]=1},
    {["name"]="vertical", ["key"]="SDLK_KP_8", ["axisValue"]=-1},
    {["name"]="vertical", ["key"]="SDLK_KP_2", ["axisValue"]=1},
    
    {["name"]="attackSword", ["key"]="SDLK_KP_0"},
    {["name"]="attackBow", ["key"]="SDLK_KP_ENTER"},
    {["name"]="jump", ["key"]="SDLK_KP_7"},
    {["name"]="action", ["key"]="SDLK_KP_9"}
}

-- TODO maybe use this for more control about different hat buttons
--    {["name"]="horizontal", ["hat"]=0, ["hatDirection"]=4, ["hatAxisValue"]=1},
joystickHotkeys = {
    -- movement
    {["name"]="horizontal", ["axis"]=0},
    {["name"]="vertical", ["axis"]=1},
    {["name"]="horizontal", ["hat"]=0, ["hatDirection"]="horizontal"},
    {["name"]="vertical", ["hat"]=0, ["hatDirection"]="vertical"},
    
    {["name"]="kickLeft", ["button"]= 0},
    {["name"]="kickRight", ["button"]=1},
    {["name"]="hitLeft", ["button"]=2},
    {["name"]="hitRight", ["button"]=3},
    {["name"]="action", ["button"]=4}
}
print("hotkeys are valid");
