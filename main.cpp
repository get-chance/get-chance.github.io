#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

#include <glfw3.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

constexpr float PI = 3.14159265358979323846f;  // float��
constexpr double PI_D = 3.14159265358979323846; // double��

// ������ ũ��
const int WIDTH = 1920;
const int HEIGHT = 1080;

// �÷��̾� ����
float characterX = WIDTH / 2.0f;
float characterY = HEIGHT / 2.0f;

int playerHP = 100;
bool gameOver = false;

// �����̴� ����
bool isBlinking = false;
float blinkTimer = 0.0f;
const float BLINK_DURATION = 3.0f;  // 3�� ������

// �Է� ����
bool spacePressed = false;

// ���콺 ��ǥ
double mouseX = 0, mouseY = 0;

// ���� ����
struct Shield {
    float x, y;
    float angle;
    float radius;
};
Shield shield = { characterX + 60, characterY, 90.0f * PI/180.0f, 40.0f };

// �̻��� ����
struct Missile {
    float x, y; // �̻����� ��ġ
    float vx, vy; // �̻����� �ӵ�
    float reflectCooldown = 0.0f; // �߰�: �ݻ� ��Ÿ�� (��)
};
std::vector<Missile> missiles;


// ����
struct Particle {
    float x, y;
    float vx, vy;
    float life;
};
std::vector<Particle> particles;


// Ű �Է�
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS)
            spacePressed = true;
        else if (action == GLFW_RELEASE)
            spacePressed = false;
    }
}

// AABB �浹 �˻� �Լ�
bool checkCollision(float x1, float y1, float w1, float h1,
    float x2, float y2, float w2, float h2) {
    return !(x1 + w1 < x2 || x1 > x2 + w2 ||
        y1 + h1 < y2 || y1 > y2 + h2);
}

// ���콺 �̵�
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = HEIGHT - ypos; // OpenGL ��ǥ ����
}

// �̻��� ����
void createMissile() {
    Missile m;
    m.x = WIDTH; // �̻����� ������ ������ ���
    m.y = rand() % HEIGHT; // ������ y ��ǥ���� ����
    m.vx = -3.0f; // �������� �̵�
    m.vy = rand() % 3 - 1; // �缱���� ������ �ӵ�
    missiles.push_back(m);
}

/*
// �̻���-���� �浹 �˻�
bool checkMissileShieldCollision(Missile& m) {
    float dx = m.x - shield.x;
    float dy = m.y - shield.y;
    float distSq = dx * dx + dy * dy;

    if (distSq <= shield.radius * shield.radius) {
        // �̻����� ���� ���ʿ� �ִ��� üũ
        float missileDirX = m.vx;
        float missileDirY = m.vy;
        float shieldDirX = cos(shield.angle);
        float shieldDirY = sin(shield.angle);

        float dot = missileDirX * shieldDirX + missileDirY * shieldDirY;
        return dot < 0.0f; // �տ��� ���� ��츸 true
    }
    return false;
}
*/

/*
bool checkMissileShieldCollision(Missile& m) {
    if (m.reflectCooldown > 0.0f)
        return false; // ��Ÿ�� ���̸� �浹 ����

    float dx = m.x - shield.x;
    float dy = m.y - shield.y;
    float distSq = dx * dx + dy * dy;

    if (distSq <= shield.radius * shield.radius) {
        float angleToMissile = atan2(dy, dx);
        float shieldAngle = shield.angle;

        // ���а� �ٶ󺸴� ���� ��90�� ���̿� �ִ��� Ȯ��
        float deltaAngle = angleToMissile - shieldAngle;

        // -�� ~ +�� ������ ����ȭ
        while (deltaAngle > 3.141592f) deltaAngle -= 2 * 3.141592f;
        while (deltaAngle < -3.141592f) deltaAngle += 2 * 3.141592f;

        if (std::abs(deltaAngle) <= 3.141592f / 2.0f) {
            return true; // �ݿ� ���ʿ� ���� ���� �浹 ����
        }
    }
    return false;
}
*/

bool checkMissileShieldCollision(Missile& m) {
    if (m.reflectCooldown > 0.0f)
        return false; // ��Ÿ�� ���̸� �浹 ����

    float dx = m.x - shield.x;
    float dy = m.y - shield.y;
    float distSq = dx * dx + dy * dy;

    if (distSq <= shield.radius * shield.radius) {
        // �̻����� ���� ���� �ȿ� ����

        // ������ �ٶ󺸴� ���� ����(?)
        float shieldDirX = cos(shield.angle);
        float shieldDirY = sin(shield.angle);

        // �̻��ϰ� ���� �߽� ������ ����
        float toMissileX = dx;
        float toMissileY = dy;

        // ���� ����ȭ
        float len = sqrt(toMissileX * toMissileX + toMissileY * toMissileY);
        if (len == 0) return false;

        toMissileX /= len;
        toMissileY /= len;

        // ���� ����� �̻��� ��ġ ������ ����
        float dot = shieldDirX * toMissileX + shieldDirY * toMissileY;

        // dot > 0 �̸� ���� ����, dot < 0 �̸� ���� ����
        if (dot > 0.0f) {
            return true; // ���� ���ʿ��� �¾����� �ݻ�
        }
    }
    return false;
}


void reflectMissile(Missile& m) {
    float dx = m.x - shield.x;
    float dy = m.y - shield.y;
    float len = sqrt(dx * dx + dy * dy);

    if (len == 0) return; // ���� �߽ɰ� �ʹ� ������ ����

    // �ݻ���� ���� ���� (���� �߽� �� �̻��� ����)
    float nx = dx / len;
    float ny = dy / len;

    // �Ի� ���Ϳ� ���� ������ ����
    float dot = m.vx * nx + m.vy * ny;

    // �ݻ� ����: R = V - 2(N��V)N
    m.vx = m.vx - 2.0f * dot * nx;
    m.vy = m.vy - 2.0f * dot * ny;

    m.reflectCooldown = 1.0f; // �߰�: 1�� ��Ÿ�� ����
}


/*
// �̻��� �ݻ� ó��
void reflectMissile(Missile& m) {
    float nx = m.x - shield.x;
    float ny = m.y - shield.y;
    float len = sqrt(nx * nx + ny * ny);
    nx /= len;
    ny /= len;

    float dot = m.vx * nx + m.vy * ny;
    m.vx = m.vx - 2.0f * dot * nx;
    m.vy = m.vy - 2.0f * dot * ny;
}
*/

// �̻��� ������Ʈ
void updateMissiles() {
    for (auto& m : missiles) {
        m.x += m.vx;
        m.y += m.vy;

        if (m.reflectCooldown > 0.0f) {
            m.reflectCooldown -= 1.0f / 60.0f; // 60FPS ����
            if (m.reflectCooldown < 0.0f) m.reflectCooldown = 0.0f;
        }

        if (checkMissileShieldCollision(m)) {
            reflectMissile(m);
        }

        // ���� ���� ����
        // �̻��� ���� ���͸� ����
        float len = std::sqrt(m.vx * m.vx + m.vy * m.vy);
        float dirX = m.vx / len;
        float dirY = m.vy / len;

        // �̻��� ���� ��ġ ��� (���� �ƴ� ��)
        float tailX = m.x - dirX * 20.0f;
        float tailY = m.y - dirY * 20.0f;

        // �Ҳ� ���� ����
        Particle p;
        p.x = tailX + (rand() % 6 - 3); // ��¦ ������
        p.y = tailY + (rand() % 6 - 3);
        p.vx = (rand() % 10 - 5) * 0.1f; // ������ �ӵ�
        p.vy = (rand() % 10 - 5) * 0.1f;
        p.life = 0.5f;
        particles.push_back(p);
    }

    // �̻����� ȭ�� ������ ������ ����
    missiles.erase(std::remove_if(missiles.begin(), missiles.end(), [](Missile& m) {
        return (m.x < -50 || m.x > WIDTH + 50 || m.y < -50 || m.y > HEIGHT + 50);
        }), missiles.end());
}

// ���� ������Ʈ
void updateParticles() {
    for (auto& p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.life -= 1.0f / 60.0f; // �����Ӹ��� ���� ����
    }

    // ���� ���� ����
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle& p) {
        return p.life <= 0.0f;
        }), particles.end());
}

// ���� ��ġ�� ����Ű�� ���� ������Ʈ
void updateShield() {
    float dx = mouseX - characterX;
    float dy = mouseY - characterY;
    shield.angle = atan2(dy, dx);
    shield.x = characterX + cos(shield.angle) * 60.0f;
    shield.y = characterY + sin(shield.angle) * 60.0f;
}

// �÷��̾� ������Ʈ
void update() {
    if (spacePressed)
        characterY += 14.0f;
    else
        characterY -= 10.0f;

    if (characterY < 0) characterY = 0;
    if (characterY > HEIGHT) characterY = HEIGHT;

    // �̻��� �浹 ó�� (�����̴� ���ȿ��� �浹 üũ �� ��)
    if (!isBlinking && !gameOver) {
        for (auto it = missiles.begin(); it != missiles.end(); ) {
            if (checkCollision(characterX - 25, characterY - 25, 50, 50,  // �÷��̾� ũ��
                it->x - 10, it->y - 5, 20, 10)) {  // �̻��� ũ��

                playerHP -= 11; // HP ����
                if (playerHP <= 0) {
                    playerHP = 0;
                    gameOver = true;
                }

                isBlinking = true;
                blinkTimer = BLINK_DURATION;
                it = missiles.erase(it); // �浹�� �̻��� ����!
            }
            else {
                ++it; // �浹 �� ������ ���� �̻��Ϸ� �Ѿ
            }
        }
    }

    // �����̴� Ÿ�̸�
    if (isBlinking) {
        blinkTimer -= 1.0f / 60.0f; // �����Ӵ� ����
        if (blinkTimer <= 0.0f) {
            isBlinking = false;
            blinkTimer = 0.0f;
        }
    }

    updateShield();
}

void drawText(const char* text, float x, float y, float r, float g, float b, float scale = 2.0f) {
    char buffer[99999]; // ����� ũ��
    int num_quads;

    glPushMatrix(); // ���� ��Ʈ���� ����
    glTranslatef(x, y, 0); // (x, y) ��ġ�� �̵�
    glScalef(scale, scale, 1.0f); // ���� ũ�� Ȯ��
    glScalef(1.0f, -1.0f, 1.0f); // Y�� ������

    glColor3f(r, g, b);
    num_quads = stb_easy_font_print(0, 0, (char*)text, NULL, buffer, sizeof(buffer));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix(); // ���� ��Ʈ������ ����
}

void renderHPBar() {
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float x = 20.0f;
    float y = HEIGHT - 70.0f; // �� ��ġ
    float textOffsetY = 50.0f; // �ؽ�Ʈ�� �ٺ��� ���� ����

    // 1. HP �ؽ�Ʈ ("HP" + ��ġ)
    char hpText[32];
    snprintf(hpText, sizeof(hpText), "HP: %d", playerHP);
    drawText(hpText, x, y + textOffsetY, 1.0f, 1.0f, 1.0f); // ��� ����

    // 2. HP Bar ��� (ȸ��)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + barWidth, y);
    glVertex2f(x + barWidth, y + barHeight);
    glVertex2f(x, y + barHeight);
    glEnd();

    // 3. ���� HP�� ����ϴ� ä�� �κ� (�ʷ�)
    float hpRatio = static_cast<float>(playerHP) / 100.0f;
    glColor3f(0.0f, 1.0f, 0.0f); // ���߿� hpRatio�� ���� �� �ٲ��� ���� �־�
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + barWidth * hpRatio, y);
    glVertex2f(x + barWidth * hpRatio, y + barHeight);
    glVertex2f(x, y + barHeight);
    glEnd();
}

// ���� ������
void renderParticles() {
    for (auto& p : particles) {
        float alpha = p.life * 2.0f;
        glColor4f(1.0f, 0.5f + 0.5f * (p.life), 0.0f, alpha);

        glBegin(GL_QUADS);
        glVertex2f(p.x - 2, p.y - 2);
        glVertex2f(p.x + 2, p.y - 2);
        glVertex2f(p.x + 2, p.y + 2);
        glVertex2f(p.x - 2, p.y + 2);
        glEnd();
    }
}

// ���� ������
void renderShield() {
    int numSegments = 20;
    float angleStep = PI / numSegments;

    glColor3f(0.0f, 1.0f, 1.0f); // û�ϻ�

    // �߽ɰ� shield.angle�� �������� -90�� ~ +90�� ������ ����Ʈ�� ��� ������.
    // ��, ������ �ݿ��� �׷��ִ� �ڵ�
    glBegin(GL_LINE_STRIP);
    for (int i = -numSegments / 2; i <= numSegments / 2; i++) {
        float angle = shield.angle + i * angleStep;
        float x = shield.x + cos(angle) * shield.radius; // cos, sin�� ������ �Ű������� �޴´�.
        float y = shield.y + sin(angle) * shield.radius;
        glVertex2f(x, y);
    }
    glEnd();

    // ������ 2��
    float offset = 10.0f;
    float dirX = cos(shield.angle);
    float dirY = sin(shield.angle);
    float sideX = -dirY;
    float sideY = dirX;

    for (float o : { -offset, offset }) {
        glBegin(GL_LINES);
        glVertex2f(shield.x + sideX * o, shield.y + sideY * o);
        glVertex2f(shield.x + sideX * o + dirX * 20.0f, shield.y + sideY * o + dirY * 20.0f);
        glEnd();
    }
}

// ������
void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    renderParticles();

    // �÷��̾� �׸��� (�����̴� ���ȿ��� ������)
    if (isBlinking) {
        float alpha = (std::sin(blinkTimer * 10.0f) * 0.5f + 0.5f) * 0.5f + 0.5f;
        glColor4f(1.0f, 0.0f, 0.0f, alpha);
    }
    else {
        glColor3f(1.0f, 0.5f, 0.0f); // �⺻ ����
    }
    //glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(characterX - 25, characterY - 25);
    glVertex2f(characterX + 25, characterY - 25);
    glVertex2f(characterX + 25, characterY + 25);
    glVertex2f(characterX - 25, characterY + 25);
    glEnd();

    // ����
    renderShield();

    // �̻���
    for (auto& m : missiles) {
        glPushMatrix(); // ���� ��ǥ�� ����

        // �̻��� ��ġ�� �̵�
        glTranslatef(m.x, m.y, 0.0f);

        // �̻��� ȸ�� (���⿡ ����)
        float angle = atan2(m.vy, m.vx) * 180.0f / 3.141592f;
        glRotatef(angle + 180, 0.0f, 0.0f, 1.0f); // ȸ�� ����

        // �̻��� ���� (ȸ��)
        glColor3f(0.7f, 0.7f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(-20, -5); // -20
        glVertex2f(0, -5);
        glVertex2f(0, 5);
        glVertex2f(-20, 5); // - 20
        glEnd();

        // �̻��� �Ӹ� (���� �ﰢ��)
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0, 0);
        glVertex2f(10, 7);
        glVertex2f(10, -7);
        glEnd();

        glPopMatrix(); // ���� ��ǥ�� ����
    }

    renderHPBar(); 
}

// ����
int main() {
    srand(static_cast<unsigned int>(time(0)));

    if (!glfwInit()) {
        std::cout << "GLFW �ʱ�ȭ ����!" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Shield Reflect Game", nullptr, nullptr);
    if (!window) {
        std::cout << "������ ���� ����!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double lastTime = glfwGetTime();
    double missileCooldown = 1.0;

    while (!glfwWindowShouldClose(window)) {
        if (gameOver) {
            std::cout << "���� ����! �÷��̾ ����߽��ϴ�." << std::endl;
            break;
        }

        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= missileCooldown) {
            createMissile();
            lastTime = currentTime;
        }

        update();
        updateMissiles();
        updateParticles();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
