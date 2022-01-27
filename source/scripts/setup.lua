require 'linalg';

SEA = {
    RADIUS = 200,
};

COLOR = {
	RED = 0xf25346,
	WHITE = 0xd8d0d1,
	BROWN = 0x59332e,
	PINK = 0xF5986E,
	BROWNDARK = 0x23190f,
	BLUE = 0x68c3c0,
};

Geometry = {};
Geometry.__index = Geometry;

function Geometry:makeCube()
   local cube = {};
   setmetatable(cube, Geometry);
   cube.shape = 'cube';
   cube.points = {};
   cube.points[1] = vec4:new(0.5, 0.5, 0.5, 1.0);
   cube.points[2] = vec4:new(0.5, 0.5, -0.5, 1.0);
   cube.points[3] = vec4:new(0.5, -0.5, 0.5, 1.0);
   cube.points[4] = vec4:new(0.5, -0.5, -0.5, 1.0);
   cube.points[5] = vec4:new(-0.5, 0.5, -0.5, 1.0);
   cube.points[6] = vec4:new(-0.5, 0.5, 0.5, 1.0);
   cube.points[7] = vec4:new(-0.5, -0.5, -0.5, 1.0);
   cube.points[8] = vec4:new(-0.5, -0.5, 0.5, 1.0);
   return cube;
end

function Geometry:applyMat(mat)
    for n = 1, #self.points do
        self.points[n] = mat * self.points[n];
    end
end

function Geometry:scale(x, y, z)
    self:applyMat(mat4:scale(x, y, z));
end

function Geometry:translate(x, y, z)
    self:applyMat(mat4:translate(x, y, z));
end

function Geometry:rotateX(rad)
    self:applyMat(mat4:rotateX(rad));
end

function Geometry:rotateY(rad)
    self:applyMat(mat4:rotateY(rad));
end

function Geometry:rotateZ(rad)
    self:applyMat(mat4:rotateZ(rad));
end

local function createCloud(angle)
    local numPieces = 3 + 3 * math.random();
    local offset = 0;
    local scaleFactor = 0.3;

    for i = 1, numPieces do
        local cube = Geometry.makeCube();
        local scale = 20 * (scaleFactor + (1 - scaleFactor) * math.random());

        -- local
        cube:scale(scale, scale, scale);
        cube:rotateY(0.5 * math.pi * math.random());
        cube:rotateZ(0.5 * math.pi * math.random());
        cube:translate((i - 1) * 10, 3 * math.random(), 3 * math.random());

        -- global
        local height = SEA.RADIUS * (1.4 + 0.3 * math.random());
        cube:translate(0, height, -1.3 * height);
        cube:rotateZ(angle);
        
        local points = {};
        for n = 1, #cube.points do
            -- print(cube.points[n]);
            local len = #points;
            points[len + 1] = cube.points[n].x;
            points[len + 2] = cube.points[n].y;
            points[len + 3] = cube.points[n].z;
        end
        Model.AddCubeMesh("sky", COLOR.WHITE, points);

        offset = offset + 0.8 * scale;
    end
end

local function createSky(numClouds)
    local stepAngle = 2.0 * math.pi / numClouds;
    for i = 1, numClouds do
        createCloud(stepAngle * i);
    end
end

createSky(20);