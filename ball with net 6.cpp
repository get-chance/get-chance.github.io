#include <glfw3.h>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>

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
    float radius;
    float vx;
    float vy;
    float jumpVelocity;
    float r, g, b;
    int hp;

    bool stopped = false;
    bool trapped = false;
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

struct Vec2 {
    float x, y;
    Vec2 operator+(const Vec2& other) const { return { x + other.x, y + other.y }; }
    Vec2 operator-(const Vec2& other) const { return { x - other.x, y - other.y }; }
    Vec2 operator*(float s) const { return { x * s, y * s }; }

    bool operator<(const Vec2& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

struct TrapNet {
    float x, y; // �ʱⰪ : �÷��̾�� �߻�� ��ġ, update �Ǹ鼭 ���콺�� ���� ���� ��ġ�ϰ� ��
    float tx, ty; // �׹��� ������(���콺�� ���� ��)
    float dx, dy; // �׹��� ���ư��� ������ ���⺤��(���� : 1)
    float vx = 0.0f, vy = 0.0f;
    float speed = 500.0f;

    float length = 0.0f;
    float maxLength = 300.0f;
    float growSpeed = 600.0f;
    float widthBase = 60.0f;
    float widthTop = 400.0f;

    TrapNetState state = TrapNetState::Flying;
    bool active = true;

    std::vector<Vec2> shapeA;
    std::vector<Vec2> shape42;
    std::vector<Vec2> targetPoints;
    float time = 0.0f;
    float prevtime = 0.0f;

    std::vector<int> meetedBalls;
};
const float horizontalStep = 20.0f; 
const float verticalStep = 30.0f;
const float verticalDensityMultiplier = 3.0f;

std::vector<TrapNet> trapNets;
int weaponMode = 1;  // 1: ��, 2: �׹�

template <typename T>
T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

// ���� ����
Vec2 lerp(const Vec2& a, const Vec2& b, float t) {
    return a + (b - a) * t;
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

    // ������
    // �� ũ�� ���� (����)
    glPointSize(5.0f);  // ���� �۾Ƽ� �� ���� �� �����Ƿ� ũ�� ����

    // �ʷϻ����� �� �׸���
    glColor3f(0.0f, 1.0f, 0.0f);  // �ʷϻ�
    glBegin(GL_POINTS);
    glVertex2f(x, y);         // x, y�� �� �׸���
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

// �ٰ����� �� �Ǵ� ������ �׸��� �Լ�
void drawConvexPolygonGLFW(const std::vector<Vec2>& polygon, bool filled = false, float lineWidth = 3.0f,
    float r = 1.0f, float g = 0.0f, float b = 0.0f) {
    if (polygon.size() < 3) return;

    glColor3f(r, g, b);
    if (filled) {
        glLineWidth(lineWidth); // �� �β� ����
        glBegin(GL_POLYGON);   // ���θ� ä�� �ٰ���
    }
    else {
        glBegin(GL_LINE_LOOP); // �����θ� �ѷ��� �ٰ���
    }

    for (const auto& pt : polygon) {
        glVertex2f(pt.x, pt.y);
    }

    glEnd();

    if (!filled) {
        glLineWidth(1.0f); // �� �β� ���󺹱� (�ٸ� �׸��� ������ ���̱� ����)
    }
}

// ������ �׸��� �Լ�
void drawPoints(const std::vector<Vec2>& points, float r, float g, float b) {
    glColor3f(r, g, b);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& p : points) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

// ������ �ٰ����� �ܰ������� ���� ����� ������
Vec2 getClosestPointOnPolygon(const Vec2& p, const std::vector<Vec2>& polygon) {
    float minDist2 = 1e9;
    Vec2 closest;

    for (size_t i = 0; i < polygon.size(); ++i) {
        Vec2 a = polygon[i];
        Vec2 b = polygon[(i + 1) % polygon.size()];
        Vec2 ab = b - a;
        float t = std::max(0.0f, std::min(1.0f,
            ((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / (ab.x * ab.x + ab.y * ab.y)));
        Vec2 proj = { a.x + t * ab.x, a.y + t * ab.y };
        float dist2 = (proj.x - p.x) * (proj.x - p.x) + (proj.y - p.y) * (proj.y - p.y);
        if (dist2 < minDist2) {
            minDist2 = dist2;
            closest = proj;
        }
    }

    return closest;
}

void drawTrapNet(const TrapNet& net) {
    if (!net.active) return;

    if (net.state == TrapNetState::Flying) { // Flying ���´� �׹��� ���콺�� Ŭ���� ������ �̵��� ������ ���� �׸��� ����
        // Flying ���¿����� ���� ���� �׸���
        drawCircle(net.x, net.y, 10.0f, 1.0f, 0.0f, 0.0f);  // ������ 10¥�� ���� ��
        return;  // Flying ���¿����� �׹� Ȯ�� X
    }
    else if (net.state == TrapNetState::Expanding) { // �׹��� �������� ����
        drawConvexPolygonGLFW(net.shapeA, false, 5.0f, 1.0f, 0.0f, 0.0f);  // ���� ��

        float nx = -net.dy;  // ��������
        float ny = net.dx;

        // 4�� ������ ��� (��ٸ���)
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

        // 1 ���μ� (����-�Ʒ��� ����)
        float totalLen = sqrtf(net.length * net.length);  // �����Ÿ�
        int numH = std::max(2, int(totalLen / horizontalStep));
        for (int i = 0; i <= numH; ++i) {
            float t = float(i) / numH;
            float startX = (1 - t) * hx1 + t * hx3;
            float startY = (1 - t) * hy1 + t * hy3;
            float endX = (1 - t) * hx2 + t * hx4;
            float endY = (1 - t) * hy2 + t * hy4;
            glVertex2f(startX, startY);
            glVertex2f(endX, endY);
            // �������� ������ ��ٸ����� ������ ������, ���� �̾��� �� ����, �Ʒ����� ����
        }

        // 2 ���μ� (����~�Ʒ��� ����)
        float topLen = sqrtf((hx1 - hx2) * (hx1 - hx2) + (hy1 - hy2) * (hy1 - hy2)); // ���콺 Ŭ�������� ����� ���� �糡 �������� ����
        int numV = std::max(2, int((topLen / verticalStep) * verticalDensityMultiplier));  // �е� ��� ����
        for (int i = 0; i <= numV; ++i) {
            float s = float(i) / numV;
            float topX = (1 - s) * hx1 + s * hx2;
            float topY = (1 - s) * hy1 + s * hy2;
            float bottomX = (1 - s) * hx3 + s * hx4; // ���콺 ��ġ�� ���� �� �� ���� �ִ� ��
            float bottomY = (1 - s) * hy3 + s * hy4;
            glVertex2f(topX, topY);
            glVertex2f(bottomX, bottomY);
        }

        glEnd();
    }
    
    else { //(net.state == TrapNetState::Falling) {

        // �� �κ��� �׹��� ���� �ּ� �ٰ����� ���ε� ���� �ð��� ���� ���������Ͽ�, ���������� ���� �׸��� �ڵ���
        std::vector<Vec2> currentPoints;
        for (size_t i = 0; i < net.targetPoints.size(); ++i)
            currentPoints.push_back(lerp(net.shape42[i], net.targetPoints[i], net.time));

        drawPoints(currentPoints, 1.0f, 0.0f, 0.0f);
    }
}

void fireTrapNet(float px, float py, float mouseX, float mouseY) {
    TrapNet net;
    net.x = px;
    net.y = py;
    net.tx = mouseX;
    net.ty = mouseY;

    // �׹��� �߻�Ǵ� ������ ����
    float dx = mouseX - px;
    float dy = mouseY - py;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0) len = 1.0f;
    net.dx = dx / len;
    net.dy = dy / len;

    // �׹��� �߻�Ǵ� ������ �ӵ��� ����
    //net.vx = net.dx * net.speed;
    //net.vy = net.dy * net.speed;

    trapNets.push_back(net);
}

bool pointInConvexQuad(float px, float py, const std::vector<std::pair<float, float>>& quad) {
    int sign = 0;
    for (int i = 0; i < 4; ++i) {
        float x1 = quad[i].first, y1 = quad[i].second;
        float x2 = quad[(i + 1) % 4].first, y2 = quad[(i + 1) % 4].second;

        float dx1 = x2 - x1;
        float dy1 = y2 - y1;
        float dx2 = px - x1;
        float dy2 = py - y1;
        float cross = dx1 * dy2 - dy1 * dx2;

        if (cross != 0) {
            if (sign == 0)
                sign = (cross > 0) ? 1 : -1;
            else if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0))
                return false;
        }
    }
    return true;
}

bool isCircleMostlyInsideQuad(float cx, float cy, float r, const std::vector<std::pair<float, float>>& quad, float threshold = 0.6f) {
    const int NUM_SAMPLES = 24;
    int insideCount = 0;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        float angle = 2 * M_PI * i / NUM_SAMPLES;
        float px = cx + std::cos(angle) * r;
        float py = cy + std::sin(angle) * r;

        if (pointInConvexQuad(px, py, quad)) {
            ++insideCount;
        }
    }

    return (insideCount >= NUM_SAMPLES * threshold);
}

void findBallsMostlyInsideTrapezoid(
    const TrapNet& net,
    std::unordered_map<int, Ball>& balls,
    std::vector<int>& result
) {
    //std::vector<Ball*> result;

    float nx = -net.dy;
    float ny = net.dx;

    float hx1 = net.x + nx * net.widthBase * 0.5f;
    float hy1 = net.y + ny * net.widthBase * 0.5f;
    float hx2 = net.x - nx * net.widthBase * 0.5f;
    float hy2 = net.y - ny * net.widthBase * 0.5f;

    float tx = net.x + net.dx * net.maxLength;
    float ty = net.y + net.dy * net.maxLength;
    float hx3 = tx + nx * net.widthTop * 0.5f;
    float hy3 = ty + ny * net.widthTop * 0.5f;
    float hx4 = tx - nx * net.widthTop * 0.5f;
    float hy4 = ty - ny * net.widthTop * 0.5f;

    std::vector<std::pair<float, float>> quad = {
        {hx1, hy1}, {hx3, hy3}, {hx4, hy4}, {hx2, hy2}
    };

    for (auto& it : balls) {
        Ball& ball = it.second;  // key�� it.first���� ����
        if (isCircleMostlyInsideQuad(ball.x, ball.y, ball.radius, quad, 0.6f)) {
            result.push_back(it.first);
        }
    }

    return;
}


float cross(const Vec2& O, const Vec2& A, const Vec2& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// �� ���� ���� �ٱ� ���� �� ���� �� ���ø�
std::vector<Vec2> sampleCircleBoundaryPoints(const Ball& b, int samplesPerCircle = 8) {
    std::vector<Vec2> points;
    for (int i = 0; i < samplesPerCircle; ++i) {
        float angle = 2.0f * M_PI * i / samplesPerCircle;
        points.push_back({
            b.x + b.radius * std::cos(angle),
            b.y + b.radius * std::sin(angle)
        });
    }
    return points;
}

// Convex Hull (Andrew's Monotone Chain)
std::vector<Vec2> convexHull(std::vector<Vec2> P) {
    int n = P.size(), k = 0;
    if (n <= 3) return P;

    std::sort(P.begin(), P.end());
    std::vector<Vec2> H(2 * n);

    // Lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k - 2], H[k - 1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }
    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; --i) {
        while (k >= t && cross(H[k - 2], H[k - 1], P[i]) <= 0) k--;
        H[k++] = P[i];
    }

    H.resize(k - 1); // remove duplicate endpoint
    return H;
}

// ?? ���� �Լ�
std::vector<Vec2> computeMinimumBoundingPolygonForCircles(std::unordered_map<int, Ball>& balls, const std::vector<int>& ball_ids) {
    std::vector<Vec2> allPoints;

    for (const int ball_id: ball_ids) {
        try {
            Ball& ball = balls.at(ball_id);
            auto sampled = sampleCircleBoundaryPoints(ball);
            allPoints.insert(allPoints.end(), sampled.begin(), sampled.end());
        }
        catch (const std::out_of_range& e) {
            std::cerr << "ID not found!\n";
        }
    }
    /*
    for (const Ball* circle : circles) {
        auto sampled = sampleCircleBoundaryPoints(*circle);
        allPoints.insert(allPoints.end(), sampled.begin(), sampled.end());
    }
    */

    return convexHull(allPoints);
}

void generateTrapezoid42(TrapNet& net) {

    const int numTop = 6;
    const int numRight = 15;
    const int numBottom = 6;
    const int numLeft = 15;

    float nx = -net.dy;
    float ny = net.dx;

    // ������ 4�� ���
    float bx1 = net.x + nx * net.widthBase * 0.5f;
    float by1 = net.y + ny * net.widthBase * 0.5f;
    float bx2 = net.x - nx * net.widthBase * 0.5f;
    float by2 = net.y - ny * net.widthBase * 0.5f;

    float tx = net.x + net.dx * net.length;
    float ty = net.y + net.dy * net.length;
    float tx1 = tx + nx * net.widthTop * 0.5f;
    float ty1 = ty + ny * net.widthTop * 0.5f;
    float tx2 = tx - nx * net.widthTop * 0.5f;
    float ty2 = ty - ny * net.widthTop * 0.5f;

    // 1. ����: bx1 -> bx2 
    for (int i = 0; i < numTop; ++i) {
        float t = float(i) / (numTop - 1);
        float x = (1 - t) * bx1 + t * bx2;
        float y = (1 - t) * by1 + t * by2;
        net.shape42.push_back({ x, y });
    }

    // 2. ������ ��: bx2 -> tx2 
    for (int i = 0; i < numRight; ++i) {
        float t = float(i) / (numRight - 1);
        float x = (1 - t) * bx2 + t * tx2;
        float y = (1 - t) * by2 + t * ty2;
        net.shape42.push_back({ x, y });
    }

    // 3. �Ʒ���: tx2 -> tx1 
    for (int i = 0; i < numBottom; ++i) {
        float t = float(i) / (numBottom - 1);
        float x = (1 - t) * tx2 + t * tx1;
        float y = (1 - t) * ty2 + t * ty1;
        net.shape42.push_back({ x, y });
    }

    // 4. ���� ��: tx1 -> bx1 
    for (int i = 0; i < numLeft; ++i) {
        float t = float(i) / (numLeft - 1);
        float x = (1 - t) * tx1 + t * bx1;
        float y = (1 - t) * ty1 + t * by1;
        net.shape42.push_back({ x, y });
    }

    return;
}

size_t count;

void updateTrapNets(float deltaTime, std::unordered_map<int, Ball>& balls) {
    for (auto& net : trapNets) {
        if (!net.active) continue;

        if (net.state == TrapNetState::Flying) {
            float tx = net.tx - net.x;
            float ty = net.ty - net.y;
            float distSq = tx * tx + ty * ty; // (���콺�� �������� ���������� �Ÿ�)�� ���� // ��� ����
            float moveStep = net.speed * deltaTime;

            if (distSq <= moveStep * moveStep) {
                net.x = net.tx;
                net.y = net.ty;
                net.state = TrapNetState::Expanding; // ���콺�� ���� ���� ��ġ�� �����ϰų� �Ѿ�� ���º���
            }
            else {
                // �׹�(������)�� ���� ��ġ ������Ʈ
                net.x += net.dx * moveStep;
                net.y += net.dy * moveStep;
            }
        }
        else if (net.state == TrapNetState::Expanding) {
            net.length += net.growSpeed * deltaTime; // �������� update. �׸��� ���� drawTrapNet����.

            if (net.meetedBalls.empty()) { // �׹��� ������ �� �ѹ��� �׹��� �ɸ� ���� Ž��
                findBallsMostlyInsideTrapezoid(net, balls, net.meetedBalls); // �׹��� �ɸ� ���� Ž����, net.meetedBalls�� ����
                net.shapeA = computeMinimumBoundingPolygonForCircles(balls, net.meetedBalls); // �ɸ� ���� ���δ� �ּ� �ٰ����� ã��

                for (const int ball_id : net.meetedBalls) { // �׹��� ���� �ɷȰ�, �׹��� �������� ���� ���� ������
                    try {
                        Ball& ball = balls.at(ball_id);
                        
                        ball.stopped = true; // ��, ���� ��ġ ������Ʈ�� ����(���� ������Ʈ �κ��� ���� ������ ����)
                        std::cout << count << " : ���� ������ ���۵˴ϴ�." << '\n';
                    }
                    catch (const std::out_of_range& e) {
                        std::cerr << "ID not found!\n";
                    }
                }
            }

            if (net.length >= net.maxLength) { // �׹��� �� ��������
                net.length = net.maxLength;

                for (const int ball_id : net.meetedBalls) { // �׹��� ���� �ɷȰ�, �׹��� �������� ���� ���� ������
                    try {
                        Ball& ball = balls.at(ball_id);

                        ball.stopped = false; // ��, ���� ��ġ ������Ʈ�� ����(���� ������Ʈ �κ��� ���� ������ ����)
                        ball.trapped = true;
                        std::cout << count << " : ���� ������ �����Ǿ����ϴ�" << ball.stopped << ball.trapped << '\n';

                        ball.vy = 0; // ���� ��ġ ������Ʈ���� �߷¿� ���� �ӵ��� �ϰ��ϴµ�, ���� ���� �ӵ��� ���� �ִٸ�, �ٷ� �������� �ʱ� ������, 0���� �����ϵ���
                    }
                    catch (const std::out_of_range& e) {
                        std::cerr << "ID not found!\n";
                    }
                }
                count++;

                generateTrapezoid42(net); // �׹� ���� ����(���������� ȭ�鿡 �׷��� ��, ���� ���� ���������� �ʾ���)

                if (!net.shapeA.empty()) { // �׹��� ������ �� ���� �ɷ��� ����,
                    for (const auto& p : net.shape42) // �׹� ���� ���ؼ�
                        net.targetPoints.push_back(getClosestPointOnPolygon(p, net.shapeA)); // ���� ���δ� �ּ� �ٰ����� ���ν�ų ���� ��ġ�� ã�Ƴ�
                }

                for (const int ball_id : net.meetedBalls) { // �׹��� ���� �ɷȰ�, �׹��� �������� ���� ���� ������
                    try {
                        Ball& ball = balls.at(ball_id);
                        std::cout << "��Ȯ��" << ball.stopped << ball.trapped << '\n';
                    }
                    catch (const std::out_of_range& e) {
                        std::cerr << "ID not found!\n";
                    }
                }
                
                net.state = TrapNetState::Falling;
            }
        }
        
        else if (net.state == TrapNetState::Falling) {
            static float speed = 0.003f;
            if (net.time < 1.0f) net.time += speed; // ���� ���δ� �ٰ����� ���ε� ���� generateTrapezoid42�� ������ �׹� ���� �������� ��Ű�� ���ؼ��� �ð��� �ʿ���
            // ���� �����ؼ� �׸��� ���� drawTrapnet�Լ��� �����.
            // �ð��� ������Ʈ
            
            
            // ���� �׹��� �ɷ��� ��, �׹��� �Ʒ��� ����Ʈ���� ����, �׹��� ��ǥ ������Ʈ, ���� ��ǥ ������Ʈ�� main �������� �̷���.
            net.vy += GRAVITY * deltaTime;
            
            for (Vec2& p : net.shape42) {
                p.y += net.vy * deltaTime;
            }

            for (Vec2& p : net.targetPoints) {
                p.y += net.vy * deltaTime;
            }
            
        }
        
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

    std::unordered_map<int, Ball> balls;
    std::vector<Bullet> bullets;
    int ballIdGenerator = 0;
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
                balls.insert({ ballIdGenerator++, newBall });
            }

            fireCooldown -= deltaTime;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && fireCooldown <= 0.0f) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                mouseY = WINDOW_HEIGHT - mouseY;

                float px = player.x + PLAYER_WIDTH / 2.0f;
                float py = player.y + PLAYER_HEIGHT;

                if (weaponMode == 1) {
                    // �� �߻�
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
                    // �׹� �߻�
                    fireTrapNet(px, py, mouseX, mouseY);
                }

                fireCooldown = FIRE_COOLDOWN;
            }

            for (auto& it : balls) {
                Ball& ball = it.second;  // key�� it.first���� ����
                if (!ball.stopped) {
                    if (!ball.trapped) {
                        ball.x += ball.vx * deltaTime;
                        ball.vy += GRAVITY * deltaTime;
                        ball.y += ball.vy * deltaTime;

                        if (ball.y <= ball.radius) {
                            ball.y = ball.radius;
                            ball.vy = ball.jumpVelocity;
                        }
                    }  else {
                        ball.vy += GRAVITY * deltaTime;
                        ball.y += ball.vy * deltaTime;
                    }
                }

                if (checkCollision(ball, player)) {
                    gameOver = true;
                }
            }

            // �׹� ������Ʈ
            updateTrapNets(deltaTime, balls);

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
                for (auto& it : balls) {
                    Ball& ball = it.second;  // key�� it.first���� ����
                    if (checkBulletCollision(ball, bullet)) {
                        ball.hp--;
                        bullet.active = false;
                        break;
                    }
                }
            }

            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
            for (auto it = balls.begin(); it != balls.end(); ) {
                const Ball& ball = it->second;

                if (ball.hp <= 0 || (ball.x - ball.radius > WINDOW_WIDTH || ball.y < 0)) {
                    it = balls.erase(it); // erase�� ���� iterator ��ȯ
                }
                else {
                    ++it;
                }
            }        }
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

        for (const auto& it : balls) {
            const Ball& ball = it.second;
            drawCircle(ball.x, ball.y, ball.radius, ball.r, ball.g, ball.b);
            drawHPBar(ball.x, ball.y, ball.radius, ball.hp, (ball.radius > 20.0f) ? 4 : 2);
        }

        for (const auto& bullet : bullets) {
            if (bullet.active)
                drawCircle(bullet.x, bullet.y, BULLET_RADIUS, 1.0f, 1.0f, 1.0f);
        }

        for (auto& net : trapNets) {
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
