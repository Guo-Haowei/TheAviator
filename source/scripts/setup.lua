require 'linalg';

SEA = {
    RADIUS = 240,
}

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

function Geometry:scale(x, y, z)
    local t = mat4:scale(x, y, z);
    for n = 1, #self.points do
        self.points[n] = t * self.points[n];
    end
end

function Geometry:translate(x, y, z)
    local t = mat4:translate(x, y, z);
    for n = 1, #self.points do
        self.points[n] = t * self.points[n];
    end
end

local function createCloud(angle)
    local height = math.random(60, 140) + SEA.RADIUS;
    local numBlocks = 3 + math.random(0, 3);

    for i = 1, numBlocks do
        local geometry = {};
        geometry.shape = 'cube';
    end
end

-- void Sky::createCloud(float angle) {
--   float height = Maths::rand(60.0f, 140.0f) + SEA::RADIUS;
--   Cloud* cloud = new Cloud();
--   clouds.push_back(cloud);

--   int index = clouds.size();
--   int nBlocks = 3 + Maths::rand(0, 3);
--   float cloudScale = Maths::rand(1.5f, 2.5f);
--   for (int i = 0; i < nBlocks; ++i) {
--     glm::vec3 position((float)i * 5.0f * cloudScale, Maths::rand(0.0f, 4.0f), Maths::rand(0.0f, 4.0f));
--     float scale = 8.0f * Maths::rand(0.5f, 0.9f) * cloudScale;
--     Entity* entity = new Entity(Geometry::cube, position, cloudColor, glm::vec3(scale), 1.0f, false, false);
--     entity->changeRotation(0.0f, Maths::rand(0.0f, 2 * PI), Maths::rand(0.0f, 2.0f * PI));

--     cloud->add(entity);
--     Entity::addEntity(entity);
--   }

--   glm::vec3 cloudPos(glm::cos(angle) * height, glm::sin(angle) * height - SEA::RADIUS, Maths::rand(-320.0f, -120.0f));
--   cloud->translate(cloudPos.x, cloudPos.y, cloudPos.z);
--   cloud->rotate(0.0f, 0.0f, angle + PI / 2.0f, cloudPos);
-- }

local function createSky(numClouds)
    local height = math.random(60, 140) + SEA.RADIUS;
    local cube = Geometry:makeCube();
    local s = math.random(6, 10);
    cube:scale(s, s, s);
    -- cube:translate(0, height, -200);

    local points = {};

    for n = 1, #cube.points do
        print(cube.points[n]);
        local len = #points;
        points[len + 1] = cube.points[n].x;
        points[len + 2] = cube.points[n].y;
        points[len + 3] = cube.points[n].z;
    end

    Model.AddCubeMesh("clouds", points);
    -- local stepAngle = 2.0 * math.pi / numClouds;
    -- for i = 0, numClouds - 1 do
    --     CreateCloud(stepAngle * i);
    --     break;
    -- end
end

createSky(20);