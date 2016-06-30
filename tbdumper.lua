
local _M = {_VERSION = '1.0'}
local mt = { __index = _M }



function _M.var_dump(data, max_level, prefix)  
    if type(prefix) ~= "string" then  
        prefix = ""  
    end  
    if type(data) ~= "table" then  
        print(prefix .. tostring(data))  
    else  
        print(data)  
        if max_level ~= 0 then  
            local prefix_next = prefix .. "\t"
            print(prefix .. "{")  
            for k,v in pairs(data) do  
                io.stdout:write(prefix_next .. k .. " = ")  
                if type(v) ~= "table" or (type(max_level) == "number" and max_level <= 1) then  
                    print(v)  
                else  
                    if max_level == nil then  
                        _M.var_dump(v, nil, prefix_next)  
                    else  
                        _M.var_dump(v, max_level - 1, prefix_next)  
                    end  
                end  
            end  
            print(prefix .. "}")  
        end  
    end  
end  

function _M.print(data, max_level)  
    _M.var_dump(data, max_level or 10)  
end 

return setmetatable({ a=a, b=c }, mt)
