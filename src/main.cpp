#include "mfw.h"
#include "renderer.h"

struct Quad {
    glm::vec2 size;
    glm::vec2 pos;

    Quad() = default;
    Quad(const glm::vec2& size = glm::vec2(1.0)
       , const glm::vec2& pos = glm::vec2(0.0))
        : size(size), pos(pos)
    {}
};

class BatchRenderer {
private:
    static const i32 MAX_BUF_SIZE = 3 * 1000;
    f32 vertexBuff[MAX_BUF_SIZE];
    int vbIndex = 0;

    u32 indexBuff[MAX_BUF_SIZE];
    int ibIndex = 0;

    bool full = false;

    mfw::VertexArray* vao;
    mfw::VertexBuffer* vbo;
    mfw::Texture2D* tex;
    mfw::IndexBuffer* ibo;
    mfw::ShaderProgram* shader;

public:
    BatchRenderer()
    {
        vao = new mfw::VertexArray();
        vbo = new mfw::VertexBuffer(nullptr, 0, GL_DYNAMIC_DRAW);
        ibo = new mfw::IndexBuffer(nullptr, 0);

        mfw::VertexBufferLayout layout;
        layout.add<f32>(3);
        layout.add<f32>(2);
        vao->applyBufferLayout(layout);
        vao->unbind();

        tex = new mfw::Texture2D("res/circle.png");
        tex->bind();

        shader = new mfw::ShaderProgram();
        shader->attachShader(GL_VERTEX_SHADER, "res/shaders/circle.vert");
        shader->attachShader(GL_FRAGMENT_SHADER, "res/shaders/circle.frag");
        shader->link();
        shader->bind();
    }

    inline bool bufferIsFull() const { return full; }

    void clearBuffer() {
        vbIndex = 0;
        ibIndex = 0;
        full = false;
    }

    void drawQuad(const Quad& q) {
        drawQuad(q.size, q.pos);
    }
    
    void drawQuad(const glm::vec2 size, const glm::vec2& pos) {
        if (vbIndex >= MAX_BUF_SIZE - 12 - 8) {
            full = true;
            return;
        }

        if (ibIndex >= MAX_BUF_SIZE - 6) {
            full = true;
            return;
        }

        int cur = vbIndex / 5;
        indexBuff[ibIndex++] = cur + 0;
        indexBuff[ibIndex++] = cur + 1;
        indexBuff[ibIndex++] = cur + 3;
        indexBuff[ibIndex++] = cur + 0;
        indexBuff[ibIndex++] = cur + 2;
        indexBuff[ibIndex++] = cur + 3;

        vertexBuff[vbIndex++] = pos.x;
        vertexBuff[vbIndex++] = pos.y;
        vertexBuff[vbIndex++] = 0;

        vertexBuff[vbIndex++] = 0;
        vertexBuff[vbIndex++] = 0;

        vertexBuff[vbIndex++] = pos.x;
        vertexBuff[vbIndex++] = size.y + pos.y;
        vertexBuff[vbIndex++] = 0;

        vertexBuff[vbIndex++] = 0;
        vertexBuff[vbIndex++] = 1;

        vertexBuff[vbIndex++] = size.x + pos.x;
        vertexBuff[vbIndex++] = pos.y;
        vertexBuff[vbIndex++] = 0;

        vertexBuff[vbIndex++] = 1;
        vertexBuff[vbIndex++] = 0;

        vertexBuff[vbIndex++] = size.x + pos.x;
        vertexBuff[vbIndex++] = size.y + pos.y;
        vertexBuff[vbIndex++] = 0;

        vertexBuff[vbIndex++] = 1;
        vertexBuff[vbIndex++] = 1;
    }

    void submit() {
        vbo->setBuffer(vertexBuff, vbIndex * sizeof(f32));
        ibo->setBuffer(indexBuff, ibIndex);

        shader->bind();
        tex->bind();
        shader->set3f("color", glm::vec3(1.0));
        shader->set1i("tex", 0);
        vao->bind();
        glDrawElements(GL_TRIANGLES, ibIndex, GL_UNSIGNED_INT, nullptr);
        vbIndex = 0;
        ibIndex = 0;
    }

    BatchRenderer& operator<<(const Quad& q) {
        drawQuad(q);
        return *this;
    }

};

class Circle {
private:
    Quad mash;
    f32 radius;
    glm::vec2 pos;

public:
    Circle(const Quad& q)
        : mash(q), radius(q.size[0]), pos(q.pos + glm::vec2(radius * 0.5))
    {}

    Circle(f32 radius, const glm::vec2& pos = glm::vec2(0))
        : mash(glm::vec2(radius), pos - glm::vec2(radius * 0.5)), radius(radius), pos(pos)
    {}

    inline f32 getRadius() const { return radius; }
    inline glm::vec2 getPos() const { return pos; }

    void setRadius(f32 radius) {
        this->radius = radius;
        mash.size = glm::vec2(radius);
        mash.pos = pos - glm::vec2(radius * 0.5);
    }

    void setPos(glm::vec2& pos) {
        mash.pos = this->pos = pos;
    }

    friend BatchRenderer& operator<<(BatchRenderer& renderer, const Circle& q) {
        renderer << q.mash;
        return renderer;
    }
};

class Simulation : public mfw::Application {
private:
    BatchRenderer renderer;

public:
    Simulation(): Application("Ball Simulation", 1280, 720)
    {}

    ~Simulation() {
    }

    virtual void Start() override {
        glClearColor(0.1, 0.1, 0.1, 1);
    }

    virtual void Update() override {
        auto window = &Application::Get().GetWindow();
        glViewport(0, 0, window->width(), window->height());
        glClear(GL_COLOR_BUFFER_BIT);

        {
            // START_CLOCK_TIMER("Render");

            glm::vec2 size = glm::vec2(10, 10);
            glm::vec2 ss = glm::vec2(1 / size.x, 1 / size.y);

            for (int i = 0; i < size.y; ++i) {
                for (int j = 0; j < size.x; ++j) {
                    Quad q(glm::vec2(ss), glm::vec2(j / size.x, i / size.y) - glm::vec2(0.5));
                    Circle c(q);
                    c.setRadius((sin(mfw::Time::GetCurrent()) * 0.5 + 0.6) / size.x);
                    renderer << c;
                    if (renderer.bufferIsFull()) {
                        renderer.submit();
                        renderer.clearBuffer();
                        renderer << c;
                    }
                }
            }
            renderer.submit();
        }
    }
};

mfw::Application* mfw::CreateApplication() {
    return new Simulation();
}
