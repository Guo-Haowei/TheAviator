vec4 = {};
vec4.__index = vec4;

function vec4:new(x, y, z, w)
    local v = {};
    setmetatable(v, vec4);
    v.x = x or 0.0;
    v.y = y or 0.0;
    v.z = z or 0.0;
    v.w = w or 0.0;
    return v;
end

function vec4:__tostring()
    return '{' .. self.x .. ',' .. self.y .. ',' .. self.z .. ',' .. self.w .. '}';
end

function vec4.__mul(lhs, rhs)
    if getmetatable(lhs) == vec4 and getmetatable(rhs) == vec4 then
        return lhs.x * rhs.x +
               lhs.y * rhs.y +
               lhs.z * rhs.z +
               lhs.w * rhs.w;
    else
        error("Type mismatch", 2);
    end
end

mat4 = {};
mat4.__index = mat4;

function mat4:new()
    local m = {};
    setmetatable(m, mat4);
    m[1] = vec4:new(1, 0, 0, 0);
    m[2] = vec4:new(0, 1, 0, 0);
    m[3] = vec4:new(0, 0, 1, 0);
    m[4] = vec4:new(0, 0, 0, 1);
    return m;
end

function mat4:scale(x, y, z)
    local m = {};
    setmetatable(m, mat4);
    m[1] = vec4:new(x, 0, 0, 0);
    m[2] = vec4:new(0, y, 0, 0);
    m[3] = vec4:new(0, 0, z, 0);
    m[4] = vec4:new(0, 0, 0, 1);
    return m;
end

function mat4:translate(x, y, z)
    local m = {};
    setmetatable(m, mat4);
    m[1] = vec4:new(1, 0, 0, x);
    m[2] = vec4:new(0, 1, 0, y);
    m[3] = vec4:new(0, 0, 1, z);
    m[4] = vec4:new(0, 0, 0, 1);
    return m;
end

function mat4:__tostring()
    return '{' ..
        tostring(self[1]) .. ',' ..
        tostring(self[2]) .. ',' ..
        tostring(self[3]) .. ',' ..
        tostring(self[4]) .. '}';
end

function mat4.__mul(lhs, rhs)
    local is_rhs_vector = getmetatable(rhs) == vec4;
    if getmetatable(lhs) == mat4 and getmetatable(rhs) == vec4 then
        return vec4:new(lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs, lhs[4] * rhs);
    else
        error("Type mismatch", 2);
    end
    -- local is_lhs_vector = getmetatable(rhs)
    -- if type(lhs) == "number" and is_rhs_vector then
    --     return vector.new(rhs.x * lhs, rhs.y * lhs)
    -- elseif type(rhs) == "number" and is_lhs_vector then
    --     return vector.new(lhs.x * rhs, lhs.y * rhs)
    -- elseif is_rhs_vector and is_lhs_vector then
    --     return vector.new(lhs.x * rhs.x, lhs.y * rhs.y)
    -- else
    -- end
end

-- -- operator overloading
-- function vector.__add(lhs, rhs)
--     assert(is_vector(lhs) and is_vector(rhs), "Type mismatch: vector expected.")
--     return vector.new(lhs.x + rhs.x, lhs.x + rhs.x)
-- end

-- function vector.__sub(lhs, rhs)
--     assert(is_vector(lhs) and is_vector(rhs), "Type mismatch: vector expected.")
--     return vector.new(lhs.x - rhs.x, lhs.x - rhs.x)
-- end

-- function vector.__unm(t)
--     assert(is_vector(t), "Type mismatch: vector expected.")
--     return vector.new(-t.x, -t.y)
-- end

