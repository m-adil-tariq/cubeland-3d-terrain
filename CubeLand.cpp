#include <GLFW/glfw3.h>
#define CALLBACK __stdcall
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>

#define WORLD_SIZE   70
#define WORLD_SCALE  1.0f
#define CAM_HEIGHT   1.5f
#define PLAYER_SPEED 7.0f
#define PI           3.14159265f
#define MAX_TREES    25

float camX = 8.0f, camY = 3.0f, camZ = 8.0f;
float yaw = 0.0f, pitch = 0.0f;
float dirX = 0.0f, dirY = 0.0f, dirZ = -1.0f;
float lastMouseX = 400, lastMouseY = 300;
bool firstMouse = true;
float sensitivity = 0.1f;

int hitX = -1, hitY = -1, hitZ = -1;
int placeX = -1, placeY = -1, placeZ = -1;

int solid[WORLD_SIZE][WORLD_SIZE] = {0};
int world[WORLD_SIZE][WORLD_SIZE];
int height[WORLD_SIZE][WORLD_SIZE];
int treeX[MAX_TREES];
int treeZ[MAX_TREES];
int treeCount = 0;

bool isSolid(float x, float z) {
    int ix = (int)x, iz = (int)z;
    if (ix < 0 || ix >= WORLD_SIZE || iz < 0 || iz >= WORLD_SIZE)
        return true;
    return solid[ix][iz] == 1;
}

void drawCube(float x, float y, float z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        // front
        glNormal3f( 0,  0,  1);
        glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        // back
        glNormal3f( 0,  0, -1);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        // top
        glNormal3f( 0,  1,  0);
        glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        // bottom
        glNormal3f( 0, -1,  0);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
        // right
        glNormal3f( 1,  0,  0);
        glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
        // left
        glNormal3f(-1,  0,  0);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
        glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawString(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
}

void drawStringLarge(float x, float y, const char* text) {
    glRasterPos2f(x + 1, y);
    for (int i = 0; text[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
}
void drawStringXL(float x, float y, const char* text) {
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            glRasterPos2f(x + dx, y + dy);
            for (int i = 0; text[i]; i++)
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
        }
    }
}
void drawHUD(int screenW, int screenH) {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenW, 0, screenH, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(10,   0);
        glVertex2f(340,  0);
        glVertex2f(340, 185);
        glVertex2f(10,  185);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.4f, 1.0f, 0.4f);  drawString(20, 155, "WASD       - Move");
    glColor3f(1.0f, 0.8f, 0.2f);  drawString(20, 125, "Mouse      - Look around");
    glColor3f(1.0f, 0.4f, 0.4f);  drawString(20,  95, "Left Click - Break block");
    glColor3f(0.4f, 0.6f, 1.0f);  drawString(20,  65, "Right Click- Place block");
    glColor3f(0.8f, 0.4f, 1.0f);  drawString(20,  35, "H          - Place a room");
    glColor3f(1.0f, 1.0f, 1.0f);  drawString(20,   5, "ESC        - Quit");

    glColor3f(0.8f, 0.1f, 0.1f); drawStringXL(20,  screenH - 50, "C");
    glColor3f(0.8f, 0.4f, 0.0f); drawStringXL(46,  screenH - 50, "u");
    glColor3f(0.6f, 0.6f, 0.0f); drawStringXL(72,  screenH - 50, "b");
    glColor3f(0.0f, 0.6f, 0.1f); drawStringXL(98,  screenH - 50, "e");
    glColor3f(0.0f, 0.4f, 0.8f); drawStringXL(124, screenH - 50, "L");
    glColor3f(0.4f, 0.0f, 0.8f); drawStringXL(150, screenH - 50, "a");
    glColor3f(0.7f, 0.0f, 0.5f); drawStringXL(176, screenH - 50, "n");
    glColor3f(0.6f, 0.2f, 0.0f); drawStringXL(202, screenH - 50, "d");

    char coords[64];
    sprintf(coords, "X: %.1f  Y: %.1f  Z: %.1f", camX, camY, camZ);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawStringLarge(screenW - 250, screenH - 40, coords);

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawTree(float x, float z) {
    int ix = (int)x, iz = (int)z;
    int base = height[ix][iz]; 
    
    for (int y = base; y <= base + 2; y++)
        drawCube(x, y, z, 0.4f, 0.25f, 0.1f);
        
    for (int dx = -1; dx <= 1; dx++)
        for (int dz = -1; dz <= 1; dz++)
            drawCube(x + dx, base + 3, z + dz, 0.2f, 0.6f, 0.1f);
}

void placeHome(int ox, int oz) {
    // W=wall, .=floor, D=door
    // edit this map to redesign the house
    const char* layout[] = {
        "WWWWWW",
        "W....W",
        "W.....",
        "W.....",
        "WD.D.W",  
        "WWWWWW",  
    };

    int rows = 6, cols = 6, wallH = 4;

    for (int z = 0; z < rows; z++) {
        for (int x = 0; x < cols; x++) {
            int wx = ox + x;
            int wz = oz + z;
            if (wx < 0 || wx >= WORLD_SIZE || wz < 0 || wz >= WORLD_SIZE) continue;

            char cell = layout[z][x];

            if (cell == 'W') {
                height[wx][wz] = wallH + 1;
                world[wx][wz]  = 2;      
                solid[wx][wz]  = 1;
            } else if (cell == 'D') {
                height[wx][wz] = 1;         
                world[wx][wz]  = 0;
                solid[wx][wz]  = 0;
            } else {
                height[wx][wz] = 1;         
                world[wx][wz]  = 1;         
                solid[wx][wz]  = 0;
            }
        }
    }
}

void drawTerrain() {
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int z = 0; z < WORLD_SIZE; z++) {
            int block = world[x][z];
            int h     = height[x][z];
            if (block == 3) continue;

            float r, g, b;
            if      (block == 0) { r = 0.4f; g = 0.7f; b = 0.3f; }
            else if (block == 1) { r = 0.6f; g = 0.4f; b = 0.2f; }
            else                 { r = 0.5f; g = 0.5f; b = 0.5f; }

            for (int y = 0; y < h; y++)
                drawCube(x * WORLD_SCALE, y * WORLD_SCALE, z * WORLD_SCALE, r, g, b);
        }
    }
    for (int i = 0; i < treeCount; i++)
        drawTree(treeX[i] * WORLD_SCALE, treeZ[i] * WORLD_SCALE);
}

void castRay() {
    hitX = hitY = hitZ = -1;
    placeX = placeY = placeZ = -1;

    int ix = (int)floor(camX);
    int iy = (int)floor(camY);
    int iz = (int)floor(camZ);

    int stepX = dirX >= 0 ? 1 : -1;
    int stepY = dirY >= 0 ? 1 : -1;
    int stepZ = dirZ >= 0 ? 1 : -1;

    float tDeltaX = fabsf(dirX) > 0.0001f ? fabsf(1.0f / dirX) : 1e30f;
    float tDeltaY = fabsf(dirY) > 0.0001f ? fabsf(1.0f / dirY) : 1e30f;
    float tDeltaZ = fabsf(dirZ) > 0.0001f ? fabsf(1.0f / dirZ) : 1e30f;

    float tMaxX = fabsf(((stepX > 0 ? floor(camX) + 1 : ceil(camX) - 1) - camX) / (dirX + 1e-30f));
    float tMaxY = fabsf(((stepY > 0 ? floor(camY) + 1 : ceil(camY) - 1) - camY) / (dirY + 1e-30f));
    float tMaxZ = fabsf(((stepZ > 0 ? floor(camZ) + 1 : ceil(camZ) - 1) - camZ) / (dirZ + 1e-30f));

    float maxDist = 6.0f;

    while (true) {
        int prevX = ix, prevY = iy, prevZ = iz;

        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            if (tMaxX > maxDist) break;
            ix += stepX;
            tMaxX += tDeltaX;
        } else if (tMaxY < tMaxZ) {
            if (tMaxY > maxDist) break;
            iy += stepY;
            tMaxY += tDeltaY;
        } else {
            if (tMaxZ > maxDist) break;
            iz += stepZ;
            tMaxZ += tDeltaZ;
        }

        if (ix < 0 || ix >= WORLD_SIZE || iz < 0 || iz >= WORLD_SIZE) break;

        if (world[ix][iz] != 3 && iy < height[ix][iz]) {
            hitX = ix; hitY = iy; hitZ = iz;
            placeX = prevX; placeY = prevY; placeZ = prevZ;
            return;
        }

        for (int i = 0; i < treeCount; i++) {
            int base = height[treeX[i]][treeZ[i]];
            if (ix == treeX[i] && iz == treeZ[i] && iy >= base && iy <= base + 2) {
                hitX = ix; hitY = iy; hitZ = iz;
                placeX = prevX; placeY = prevY; placeZ = prevZ;
                return;
            }
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos; lastMouseY = ypos;
        firstMouse = false; return;
    }
    float dx = (xpos - lastMouseX) * sensitivity;
    float dy = (lastMouseY - ypos) * sensitivity;
    lastMouseX = xpos; lastMouseY = ypos;

    yaw   += dx;
    pitch += dy;
    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    dirX = cos(yaw * PI / 180.0f) * cos(pitch * PI / 180.0f);
    dirY = sin(pitch * PI / 180.0f);
    dirZ = sin(yaw * PI / 180.0f) * cos(pitch * PI / 180.0f);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_PRESS) return;

    castRay();

    if (button == GLFW_MOUSE_BUTTON_LEFT && hitX != -1) {
        if (height[hitX][hitZ] > 0) {
            height[hitX][hitZ]--;
            if (height[hitX][hitZ] <= 1) solid[hitX][hitZ] = 0;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && placeX != -1 && placeY != -1 && placeZ != -1) {
        if (placeX >= 0 && placeX < WORLD_SIZE && placeZ >= 0 && placeZ < WORLD_SIZE) {
            height[placeX][placeZ]++;
            if (world[placeX][placeZ] == 3) world[placeX][placeZ] = 0;
            if (placeX != (int)camX || placeZ != (int)camZ)
                if (height[placeX][placeZ] > 1) solid[placeX][placeZ] = 1;
        }
    }
}

void generateWorld() {
    srand((unsigned int)time(NULL));

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int z = 0; z < WORLD_SIZE; z++) {
            if (x == 0 || x == WORLD_SIZE - 1 || z == 0 || z == WORLD_SIZE - 1) {
                world[x][z]  = 2;
                height[x][z] = 1;
                continue;
            }

            int r = rand() % 10;
            if      (r < 7) world[x][z] = 0; 
            else if (r < 9) world[x][z] = 1; 
            else            world[x][z] = 2; 

            int h = rand() % 10;
            if      (h < 9) height[x][z] = 1; 
            else            height[x][z] = 2; 
        }
    }
}

int main(void) {
    if (!glfwInit()) return -1;
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "3D World", monitor, NULL);
    // GLFWwindow* window = glfwCreateWindow(mode->width - 100, mode->height - 100, "CubeLand", NULL, NULL);
    // glfwSetWindowPos(window, 50, 50);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    int screenW = mode->width, screenH = mode->height;
    int argc = 0;
    glutInit(&argc, nullptr);

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    float lastTime = glfwGetTime();
    generateWorld();

    for (int x = 0; x < WORLD_SIZE; x++)
        for (int z = 0; z < WORLD_SIZE; z++)
            if (height[x][z] > 1) solid[x][z] = 1;

    for (int x = 0; x < WORLD_SIZE; x++)
        for (int z = 0; z < WORLD_SIZE; z++)
            if (height[x][z] > 1) solid[x][z] = 1;

    srand(42);
    for (int i = 0; i < MAX_TREES; i++) {
        int tx = 1 + rand() % (WORLD_SIZE - 2);
        int tz = 1 + rand() % (WORLD_SIZE - 2);
        if (world[tx][tz] == 0 && height[tx][tz] == 1) {
            treeX[treeCount] = tx;
            treeZ[treeCount] = tz;
            solid[tx][tz] = 1;
            treeCount++;
        }
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_FLAT);

    // light direction — coming from top right
    float lightPos[] = { 1.0f, 2.0f, 1.0f, 0.0f }; // w=0 means directional
    float ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };  // dark base
    float diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };  // full white light

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime   = currentTime - lastTime;
        lastTime          = currentTime;

        int ix = (int)camX, iz = (int)camZ;
        if (ix >= 0 && ix < WORLD_SIZE && iz >= 0 && iz < WORLD_SIZE) {
            solid[ix][iz] = 0; 
            camY = height[ix][iz] * WORLD_SCALE + CAM_HEIGHT;
        }

        float nx = camX, nz = camZ;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            nx += dirX * PLAYER_SPEED * deltaTime;
            nz += dirZ * PLAYER_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            nx -= dirX * PLAYER_SPEED * deltaTime;
            nz -= dirZ * PLAYER_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            nx += dirZ * PLAYER_SPEED * deltaTime;
            nz -= dirX * PLAYER_SPEED * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            nx -= dirZ * PLAYER_SPEED * deltaTime;
            nz += dirX * PLAYER_SPEED * deltaTime;
        }
        
        static bool heldH = false;
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !heldH) {
            int ox = (int)camX - 3; 
            int oz = (int)camZ - 3;
            placeHome(ox, oz);
            heldH = true;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) heldH = false;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (!isSolid(nx, nz)) {
            camX = nx; camZ = nz;
        } else if (!isSolid(nx, camZ)) {
            camX = nx; 
        } else if (!isSolid(camX, nz)) {
            camZ = nz; 
        }

        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0f, (float)mode->width / (float)mode->height, 0.1f, 100.0f);  
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(
            camX, camY, camZ,
            camX + dirX, camY + dirY, camZ + dirZ,
            0.0f, 1.0f, 0.0f
        );

        drawTerrain();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
            glVertex2f(-0.02f, 0); glVertex2f(0.02f, 0);
            glVertex2f(0, -0.02f); glVertex2f(0, 0.02f);
        glEnd();

        drawHUD(screenW, screenH);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}