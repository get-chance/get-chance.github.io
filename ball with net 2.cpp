// === ball with net (Soft-body TrapNet 적용본) ===
#include <glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>

constexpr float M_PI = 3.14159265358979323846f;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const float MIN_RADIUS = 30.0f;
const float MAX_RADIUS = 50.0f;
const float MIN_JUMP_VEL = 1000.0f;
const float MAX_JUMP_VEL = 1500.0f;
const float MIN_HORIZONTAL_SPEED = 100.0f;
const float MAX_HORIZONTAL_SPEED = 150.0f;
const float GRAVITY = -980.0f;

const float PLAYER_WIDTH = 60.0f;
const float PLAYER_HEIGHT = 60.0f;
const float PLAYER_SPEED = 300.0f;

const float BULLET_SPEED = 500.0f;
const float FIRE_COOLDOWN = 0.3f;
const float BULLET_RADIUS = 8.0f;

struct Ball {
    float x, y;
    float vy;
    float radius;
    float jumpVelocity;
    float vx;
    float r, g, b;
    int hp;
};

struct Player {
    float x, y;
};

struct Bullet {
    float x, y;
    float vx, vy;
    bool active;
};

enum class TrapNetState { Flying, Expanding, Falling };

struct NetNode {
    float x, y;
    float vx = 0, vy = 0;
};

struct NetSpring {
    int a, b;
    float restLength;
};

struct TrapNet {
    float x, y; // 초기값 : 플레이어에서 발사된 위치, update 되면서 마우스로 찍은 점과 일치하게 됨
    float tx, ty; // 그물의 시작점(마우스로 찍은 점)
    float dx, dy; // 그물이 날아가는 방향의 방향벡터(길이 : 1)
    float speed = 500.0f;

    float vx = 0.0f, vy = 0.0f;

    float length = 0.0f;
    float maxLength = 300.0f;
    float growSpeed = 600.0f;
    float widthBase = 60.0f;
    float widthTop = 400.0f;

    TrapNetState state = TrapNetState::Flying;
    bool active = true;

    std::vector<NetNode> nodes;
    std::vector<NetSpring> springs;
};
const float horizontalStep = 20.0f; //20.0f
const float verticalStep = 30.0f;
const float verticalDensityMultiplier = 3.0f; //3.0f

std::vector<TrapNet> trapNets;
int weaponMode = 1;  // 1: 총, 2: 그물

template <typename T>
T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

void drawCircle(float cx, float cy, float r, float red, float green, float blue, int segments = 40) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(red, green, blue);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        float dx = cos(angle) * r;
        float dy = sin(angle) * r;
        glVertex2f(cx + dx, cy + dy);
    }
    glEnd();
}

void drawRect(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // 디버깅용
    // 점 크기 설정 (선택)
    glPointSize(5.0f);  // 점이 작아서 안 보일 수 있으므로 크기 지정

    // 초록색으로 점 그리기
    glColor3f(0.0f, 1.0f, 0.0f);  // 초록색
    glBegin(GL_POINTS);
    glVertex2f(x, y);         // x, y에 점 그리기
    glEnd();
}

void drawHPBar(float x, float y, float radius, int hp, int maxHp) {
    float barWidth = radius * 2;
    float barHeight = 5.0f;
    float startX = x - radius;
    float startY = y + radius + 5;

    drawRect(startX, startY, barWidth, barHeight, 0.3f, 0.3f, 0.3f);
    float hpRatio = (float)hp / maxHp;
    drawRect(startX, startY, barWidth * hpRatio, barHeight, 0.0f, 1.0f, 0.0f);
}

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

bool checkCollision(const Ball& ball, const Player& player) {
    float closestX = std::max(player.x, std::min(ball.x, player.x + PLAYER_WIDTH));
    float closestY = std::max(player.y, std::min(ball.y, player.y + PLAYER_HEIGHT));
    float dx = ball.x - closestX;
    float dy = ball.y - closestY;
    return (dx * dx + dy * dy) < (ball.radius * ball.radius);
}

bool checkBulletCollision(const Ball& ball, const Bullet& bullet) {
    float dx = ball.x - bullet.x;
    float dy = ball.y - bullet.y;
    return (dx * dx + dy * dy) < (ball.radius + BULLET_RADIUS) * (ball.radius + BULLET_RADIUS);
}

void drawGameOverMessage() {
    const float boxWidth = 300;
    const float boxHeight = 100;
    float x = (WINDOW_WIDTH - boxWidth) / 2.0f;
    float y = (WINDOW_HEIGHT - boxHeight) / 2.0f;

    drawRect(x, y, boxWidth, boxHeight, 0.2f, 0.2f, 0.2f);
    drawRect(x + 100, y + 60, 100, 10, 1.0f, 0.0f, 0.0f);
    drawRect(x + 90, y + 30, 120, 10, 1.0f, 1.0f, 1.0f);
    drawRect(x + 60, y + 10, 180, 5, 0.0f, 1.0f, 0.0f);
}



void drawTrapNet(const TrapNet& net) {
    if (!net.active) return;

    if (net.state == TrapNetState::Flying) {
        // Flying 상태에서는 빨간 공을 그린다
        drawCircle(net.x, net.y, 10.0f, 1.0f, 0.0f, 0.0f);  // 반지름 10짜리 빨간 공
        return;  // Flying 상태에서는 그물 확장 X
    }
    else if (net.state == TrapNetState::Expanding) {

        float nx = -net.dy;  // 수직벡터
        float ny = net.dx;

        // 4개 꼭짓점 계산 (사다리꼴)
        float hx1 = net.x + nx * net.widthBase * 0.5f;
        float hy1 = net.y + ny * net.widthBase * 0.5f;
        float hx2 = net.x - nx * net.widthBase * 0.5f;
        float hy2 = net.y - ny * net.widthBase * 0.5f;

        float tx = net.x + net.dx * net.length;
        float ty = net.y + net.dy * net.length;
        float hx3 = tx + nx * net.widthTop * 0.5f;
        float hy3 = ty + ny * net.widthTop * 0.5f;
        float hx4 = tx - nx * net.widthTop * 0.5f;
        float hy4 = ty - ny * net.widthTop * 0.5f;

        glColor3f(0.4f, 0.9f, 1.0f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);

        // 1 가로선 (윗변-아랫변 평행)
        float totalLen = sqrtf(net.length * net.length);  // 직선거리
        int numH = std::max(2, int(totalLen / horizontalStep));
        for (int i = 0; i <= numH; ++i) {
            float t = float(i) / numH;
            float startX = (1 - t) * hx1 + t * hx3;
            float startY = (1 - t) * hy1 + t * hy3;
            float endX = (1 - t) * hx2 + t * hx4;
            float endY = (1 - t) * hy2 + t * hy4;
            glVertex2f(startX, startY); 
            glVertex2f(endX, endY);
            // 시작점과 끝점은 사다리꼴의 옆변에 있으며, 둘이 이었을 때 윗변, 아랫변과 평행
        }

        // 2 세로선 (윗변~아랫변 연결)
        float topLen = sqrtf((hx1 - hx2) * (hx1 - hx2) + (hy1 - hy2) * (hy1 - hy2)); // 마우스 클리적점과 가까운 변의 양끝 꼭짓점의 길이
        int numV = std::max(2, int((topLen / verticalStep) * verticalDensityMultiplier));  // 밀도 배수 적용
        for (int i = 0; i <= numV; ++i) {
            float s = float(i) / numV;
            float topX = (1 - s) * hx1 + s * hx2;
            float topY = (1 - s) * hy1 + s * hy2;
            float bottomX = (1 - s) * hx3 + s * hx4; // 마우스 터치한 점과 먼 변 위에 있는 점
            float bottomY = (1 - s) * hy3 + s * hy4;
            glVertex2f(topX, topY);
            glVertex2f(bottomX, bottomY);
        }

        glEnd();
    }
    else { //(net.state == TrapNetState::Falling) {
        glColor3f(0.4f, 0.9f, 1.0f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);

        for (const auto& spring : net.springs) {
            const NetNode& a = net.nodes[spring.a];
            const NetNode& b = net.nodes[spring.b];
            glVertex2f(a.x, a.y);
            glVertex2f(b.x, b.y);
        }

        glEnd();
    }
    
}

void fireTrapNet(float px, float py, float mouseX, float mouseY) {
    TrapNet net;
    net.x = px;
    net.y = py;
    net.tx = mouseX;
    net.ty = mouseY;

    // 그물이 발사되는 방향을 구함
    float dx = mouseX - px;
    float dy = mouseY - py;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0) len = 1.0f;
    net.dx = dx / len;
    net.dy = dy / len;

    // 그물이 발사되는 방향의 속도를 구함
    net.vx = net.dx * net.speed;
    net.vy = net.dy * net.speed;

    trapNets.push_back(net);
}

// === Soft-body mesh 초기화 ===
void initializeTrapNetMesh(TrapNet& net);
// === Soft-body mesh 물리 ===
void updateTrapNetPhysics(TrapNet& net, float deltaTime);

void updateTrapNets(float deltaTime) {
    for (auto& net : trapNets) {
        if (!net.active) continue;

        if (net.state == TrapNetState::Flying) {
            float tx = net.tx - net.x;
            float ty = net.ty - net.y;
            float distSq = tx * tx + ty * ty; // (마우스로 찍은점과 빨간점간의 거리)의 제곱 // 계속 감소
            float moveStep = net.speed * deltaTime;

            if (distSq <= moveStep * moveStep) {
                net.x = net.tx;
                net.y = net.ty;
                net.state = TrapNetState::Expanding; // 마우스로 찍은 점의 위치와 동일하거나 넘어서면 상태변경
            }
            else {
                // 그물(빨간점)의 현재 위치 업데이트
                net.x += net.dx * moveStep;
                net.y += net.dy * moveStep;
            }
        }
        else if (net.state == TrapNetState::Expanding) {
            net.length += net.growSpeed * deltaTime; // 펼쳐짐을 update. 그리는 것은 updateTrapNet에서.
            if (net.length >= net.maxLength) {
                net.length = net.maxLength;

                initializeTrapNetMesh(net);
                net.state = TrapNetState::Falling;
            }
        }
        else if (net.state == TrapNetState::Falling) {
            updateTrapNetPhysics(net, deltaTime);
        }
    }
}

void initializeTrapNetMesh(TrapNet& net) {

    float nx = -net.dy; // (dx, dy)는 전방 방향
    float ny = net.dx; // (nx, ny)는 dx, dy를 왼쪽으로 90도 회전시킨 수직 벡터
    // 사다리꼴의 **폭(가로 방향)**을 만들기 위해 쓰인다

    float hx1 = net.x + nx * net.widthBase * 0.5f; // 펼쳐지기 시작한 점과 가까운 사다리꼴의 변
    float hy1 = net.y + ny * net.widthBase * 0.5f; // 의 끝 좌표
    float hx2 = net.x - nx * net.widthBase * 0.5f; //
    float hy2 = net.y - ny * net.widthBase * 0.5f;

    float tx = net.x + net.dx * net.length;
    float ty = net.y + net.dy * net.length;
    float hx3 = tx + nx * net.widthTop * 0.5f; // 펼쳐지기 시작한 점과 먼 사다리꼴의 변
    float hy3 = ty + ny * net.widthTop * 0.5f; // 의 끝 좌표
    float hx4 = tx - nx * net.widthTop * 0.5f;
    float hy4 = ty - ny * net.widthTop * 0.5f;

    // 1 사다리꼴을 세로로 나누는 수 = 가로선 (윗변-아랫변 평행)의 개수 : numH
    float totalLen = sqrtf(net.length * net.length);
    int numH = std::max(2, int(totalLen / horizontalStep));

    // 2 사다리꼴을 가로로 나누는 수 = 세로선 (윗변~아랫변 연결)의 개수 : numV
    float topLen = sqrtf((hx1 - hx2) * (hx1 - hx2) + (hy1 - hy2) * (hy1 - hy2));
    int numV = std::max(2, int((topLen / verticalStep) * verticalDensityMultiplier));

    net.nodes.clear();
    net.springs.clear();

    for (int v = 0; v <= numV; ++v) {
        // 윗변과 아랫변 사이에서 수직선을 하나 선택
        // 밖 for문은 각 세로줄 마다
        // (topX, topY) = (hx1, hy1) ~ (hx2, hy2) 양 끝점 포함
        // (bottomX, bottomY) = (hx/3, hy3) ~ (hx4, hy4) 양 끝점 포함
        float s = float(v) / numV; // s는 0.0부터 1.0까지 선형보간 인자
        float topX = (1 - s) * hx1 + s * hx2; // topX, topY: 윗변 위의 위치 (왼쪽 → 오른쪽)
        float topY = (1 - s) * hy1 + s * hy2;
        float bottomX = (1 - s) * hx3 + s * hx4; // bottomX, bottomY: 아랫변 위의 대응 위치
        float bottomY = (1 - s) * hy3 + s * hy4;

        for (int h = 0; h <= numH; ++h) {
            // 해당 세로선 위에서 위→아래 방향으로 점들을 찍음
            // 안 for문은 각 세로줄 위의 "가로줄 점"들 계산
            // (x, y) = (hx1, hy1) ~ (hx3, hy3) 위 아래로 양 끝점 포함
            float t = float(h) / numH; // t도 보간 인자 (0.0 ~ 1.0)
            float x = (1 - t) * topX + t * bottomX;
            float y = (1 - t) * topY + t * bottomY;
            net.nodes.push_back({ x, y, net.vx, net.vy }); // 위쪽부터 아래쪽까지 점을 찍으며, net.nodes에 추가
        }
    }
    // 사다리꼴 내부에(numV + 1)×(numH + 1) 개의 노드가 만들어짐
    size_t count = net.nodes.size();

    auto nodeIndex = [&](int v, int h) { return v * (numH + 1) + h; };

    for (int v = 0; v <= numV; ++v) {
        for (int h = 0; h <= numH; ++h) {
            int idx = nodeIndex(v, h);
            if (h < numH) {
                int right = nodeIndex(v, h + 1);
                float dx = net.nodes[idx].x - net.nodes[right].x;
                float dy = net.nodes[idx].y - net.nodes[right].y;
                net.springs.push_back({ idx, right, sqrtf(dx * dx + dy * dy) });
            }
            if (v < numV) {
                int down = nodeIndex(v + 1, h);
                float dx = net.nodes[idx].x - net.nodes[down].x;
                float dy = net.nodes[idx].y - net.nodes[down].y;
                net.springs.push_back({ idx, down, sqrtf(dx * dx + dy * dy) });
            }

            /*
            if (h < numH && v < numV) {
                int diag = nodeIndex(v + 1, h + 1);
                float dx = net.nodes[idx].x - net.nodes[diag].x;
                float dy = net.nodes[idx].y - net.nodes[diag].y;
                net.springs.push_back({ idx, diag, sqrtf(dx * dx + dy * dy) });
            }
            */
        }
    }
}

void updateTrapNetPhysics(TrapNet& net, float deltaTime) {
    const float springStiffness = 400.0f;
    const float damping = 0.98f;

    for (auto& node : net.nodes) {
        node.vy += GRAVITY * deltaTime;
        node.vx *= damping;
        node.vy *= damping;
        node.x += node.vx * deltaTime;
        node.y += node.vy * deltaTime;
    }

    for (const auto& spring : net.springs) {
        NetNode& a = net.nodes[spring.a];
        NetNode& b = net.nodes[spring.b];

        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist == 0) continue;

        float diff = (dist - spring.restLength) / dist;
        float force = springStiffness * diff;

        float fx = force * dx;
        float fy = force * dy;

        a.vx += fx * deltaTime;
        a.vy += fy * deltaTime;
        b.vx -= fx * deltaTime;
        b.vy -= fy * deltaTime;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bouncing Balls + Player", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    std::vector<Ball> balls;
    std::vector<Bullet> bullets;
    float spawnTimer = 0.0f;
    float nextSpawnTime = randomFloat(0.5f, 1.5f);
    float fireCooldown = 0.0f;

    Player player;
    player.x = WINDOW_WIDTH / 2.0f - PLAYER_WIDTH / 2.0f;
    player.y = PLAYER_HEIGHT;//60.0f;

    float startTime = glfwGetTime();
    float prevTime = startTime;
    bool gameOver = false;

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - prevTime;
        prevTime = currentTime;

        float survivalTime = currentTime - startTime;
        std::string title = "Survival Time: " + std::to_string((int)survivalTime) + "s";
        glfwSetWindowTitle(window, title.c_str());

        if (!gameOver) {
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
                weaponMode = 1;
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
                weaponMode = 2;

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                player.x -= PLAYER_SPEED * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                player.x += PLAYER_SPEED * deltaTime;
            player.x = clamp(player.x, 0.0f, (float)(WINDOW_WIDTH - PLAYER_WIDTH));

            spawnTimer += deltaTime;
            if (spawnTimer >= nextSpawnTime) {
                spawnTimer = 0.0f;
                nextSpawnTime = randomFloat(0.5f, 1.5f);

                Ball newBall;
                newBall.radius = randomFloat(MIN_RADIUS, MAX_RADIUS);
                newBall.jumpVelocity = randomFloat(MIN_JUMP_VEL, MAX_JUMP_VEL);
                newBall.vy = newBall.jumpVelocity;
                newBall.vx = randomFloat(MIN_HORIZONTAL_SPEED, MAX_HORIZONTAL_SPEED);
                newBall.x = -newBall.radius;
                newBall.y = newBall.radius;

                int colorIndex = rand() % 4;
                switch (colorIndex) {
                case 0: newBall.r = 1.0f; newBall.g = 0.6f; newBall.b = 0.6f; break;
                case 1: newBall.r = 0.8f; newBall.g = 0.6f; newBall.b = 1.0f; break;
                case 2: newBall.r = 0.6f; newBall.g = 1.0f; newBall.b = 0.6f; break;
                case 3: newBall.r = 1.0f; newBall.g = 1.0f; newBall.b = 0.6f; break;
                }

                newBall.hp = (newBall.radius > 20.0f) ? 4 : 2;
                balls.push_back(newBall);
            }

            fireCooldown -= deltaTime;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && fireCooldown <= 0.0f) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                mouseY = WINDOW_HEIGHT - mouseY;

                float px = player.x + PLAYER_WIDTH / 2.0f;
                float py = player.y + PLAYER_HEIGHT;

                if (weaponMode == 1) {
                    // 총 발사
                    float dx = mouseX - px;
                    float dy = mouseY - py;
                    float length = std::sqrt(dx * dx + dy * dy);
                    if (length == 0) length = 1;

                    Bullet b;
                    b.x = px;
                    b.y = py;
                    b.vx = BULLET_SPEED * dx / length;
                    b.vy = BULLET_SPEED * dy / length;
                    b.active = true;
                    bullets.push_back(b);
                }
                else if (weaponMode == 2) {
                    // 그물 발사
                    fireTrapNet(px, py, mouseX, mouseY);
                }

                fireCooldown = FIRE_COOLDOWN;
            }

            for (auto& ball : balls) {
                ball.x += ball.vx * deltaTime;
                ball.vy += GRAVITY * deltaTime;
                ball.y += ball.vy * deltaTime;

                if (ball.y <= ball.radius) {
                    ball.y = ball.radius;
                    ball.vy = ball.jumpVelocity;
                }

                if (checkCollision(ball, player)) {
                    gameOver = true;
                }
            }

            updateTrapNets(deltaTime);

            for (auto& bullet : bullets) {
                if (bullet.active) {
                    bullet.x += bullet.vx * deltaTime;
                    bullet.y += bullet.vy * deltaTime;
                    if (bullet.x < 0 || bullet.x > WINDOW_WIDTH || bullet.y < 0 || bullet.y > WINDOW_HEIGHT)
                        bullet.active = false;
                }
            }

            for (auto& bullet : bullets) {
                if (!bullet.active) continue;
                for (auto& ball : balls) {
                    if (checkBulletCollision(ball, bullet)) {
                        ball.hp--;
                        bullet.active = false;
                        break;
                    }
                }
            }

            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
            balls.erase(std::remove_if(balls.begin(), balls.end(), [](const Ball& b) { return b.hp <= 0 || b.x - b.radius > WINDOW_WIDTH; }), balls.end());
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            balls.clear();
            bullets.clear();
            trapNets.clear();

            player.x = WINDOW_WIDTH / 2.0f - PLAYER_WIDTH / 2.0f;
            player.y = 60.0f;
            startTime = glfwGetTime();
            prevTime = startTime;
            spawnTimer = 0.0f;
            nextSpawnTime = randomFloat(0.5f, 1.5f);
            gameOver = false;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (const auto& ball : balls) {
            drawCircle(ball.x, ball.y, ball.radius, ball.r, ball.g, ball.b);
            drawHPBar(ball.x, ball.y, ball.radius, ball.hp, (ball.radius > 20.0f) ? 4 : 2);
        }

        for (const auto& bullet : bullets) {
            if (bullet.active)
                drawCircle(bullet.x, bullet.y, BULLET_RADIUS, 1.0f, 1.0f, 1.0f);
        }

        for (const auto& net : trapNets) {
            drawTrapNet(net);
        }

        drawRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, 0.2f, 0.2f, 0.9f);

        if (gameOver) {
            drawGameOverMessage();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
