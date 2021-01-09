#ifndef GLSEA_H
#define GLSEA_H
#include <glm/glm.hpp>
#include "Program.h"
#include "Buffer.h"
#include <memory>

class GLSea
{
public:
    GLSea(float width, float height, float cameraTheta);
    void render(glm::vec3& eyePos, glm::mat4& proj, glm::mat4& view, glm::vec3& sun, double time=0);
    void generateMesh();
    void generateWaves();
private:
    GLint vertexLocation{};
    void prepare_grid();
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
    std::unique_ptr<Buffer> ibo;
//    GLuint tex{};
//    GLuint normal_tex{};
//    GLuint spec_tex{};
    int m_viewLoc;
    int m_timeLoc;
    const GLuint gridsize{1000};
    const float size{100.0f};

    float cameraTheta{0.0};
    glm::vec4 vboParams{};
    float width_;
    float height_;
    int vboSize{};

    // WAVES PARAMETERS (INPUT)
    GLuint waveTex;
    int nbWaves = 60;
    glm::vec4 *waves = nullptr;
    float lambdaMin = 0.02;
    float lambdaMax = 30.0;
    float heightMax = 0.32;
    float waveDirection = 2.4;
    float U0 = 10.0;
    float waveDispersion = 1.25f;

    // WAVE STATISTICS (OUTPUT)
    float sigmaXsq = 0.0;
    float sigmaYsq = 0.0;
    float meanHeight = 0.0;
    float heightVariance = 0.0;
    float amplitudeMax = 0.0;

    // RENDERING OPTIONS

    float gridSize = 8.0;
    float nyquistMin = 1.0;
    float nyquistMax = 1.5;
    float seaColor[4] = {10.0 / 255.0, 40.0 / 255.0, 120.0 / 255.0, 0.1};
    float hdrExposure = 0.4;
    bool grid {false};
public:
    float getCameraTheta() const;

    void setCameraTheta(float cameraTheta);

    float getWidth() const;

    void setWidth(float width);

    float getHeight() const;

    void setHeight(float height);
};

#endif // GLSEA_H
