#define _USE_MATH_DEFINES
#include <cmath>
#include "glsea.h"
#include "utils.h"
#include <cmrc/cmrc.hpp>
//#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "Defines.h"

CMRC_DECLARE(glsl_resources);

GLSea::GLSea(float width, float height, float cameraTheta) :width_(width),height_(height), cameraTheta(cameraTheta) {
    m_program = std::make_unique<Program>();
    auto fs = cmrc::glsl_resources::get_filesystem();
    std::string shader_src{fs.open("glsl/ocean.glsl").begin()};
    std::string atmosphere_src{fs.open("glsl/atmosphere.glsl").begin()};
    m_program->addVertexShader(("#version 330\n"
                                "#define highp\n"
                                "#define mediump\n"
                                "#define lowp\n"
                                "#define _VERTEX_\n"+atmosphere_src+ shader_src).c_str());
    m_program->addFragmentShader(("#version 330\n"
                                  "#define highp\n"
                                  "#define mediump\n"
                                  "#define lowp\n"
                                  "#define _FRAGMENT_\n"
                                  "#define SUN_CONTRIB\n" +atmosphere_src+ shader_src).c_str());
    m_program->link();
    m_program->bind();
    assert(glGetError() == 0);
    vertexLocation = glGetAttribLocation(m_program->programId(), "vertex");
//    glUniform1i(glGetUniformLocation(m_program->programId(), "skyIrradianceSampler"), IRRADIANCE_UNIT);
//    glUniform1i(glGetUniformLocation(m_program->programId(), "inscatterSampler"), INSCATTER_UNIT);
//    glUniform1i(glGetUniformLocation(m_program->programId(), "transmittanceSampler"), TRANSMITTANCE_UNIT);
//    glUniform1i(glGetUniformLocation(m_program->programId(), "skySampler"), SKY_UNIT);
    glUniform1i(glGetUniformLocation(m_program->programId(), "wavesSampler"), USV_WAVE_UNIT);
    m_program->release();
    vbo = std::make_unique<Buffer>();
    ibo = std::make_unique<Buffer>();
    vbo->create();
    ibo->create();
    glActiveTexture(GL_TEXTURE0 + USV_WAVE_UNIT);
    glGenTextures(1, &waveTex);
    glBindTexture(GL_TEXTURE_1D, waveTex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    generateWaves();
    assert(glGetError() == 0);
    prepare_grid();
    assert(glGetError() == 0);
}


void GLSea::generateMesh()
{
    vbo->bind();
    float horizon = tan(cameraTheta);
    float s = std::min(1.1f, 0.5f + horizon * 0.5f);

    float vmargin = 0.0;
    float hmargin = 0.1;

    vboParams = {width_, height_, gridSize, cameraTheta};
    glm::vec4 *data = new glm::vec4[int(ceil(height_ * (s + vmargin) / gridSize) + 5) * int(ceil(width_ * (1.0 + 2.0 * hmargin) / gridSize) + 5)];

    int n = 0;
    int nx = 0;
    for (float j = height_ * s - 0.1f; j > -height_ * vmargin - gridSize; j -= gridSize) {
        nx = 0;
        for (float i = -width_ * hmargin; i < width_ * (1.0 + hmargin) + gridSize; i += gridSize) {
            data[n++] = glm::vec4(-1.0 + 2.0 * i / width_, -1.0 + 2.0 * j / height_, 0.0, 1.0);
            nx++;
        }
    }

    glBufferData(GL_ARRAY_BUFFER, n * 16, data, GL_STATIC_DRAW);
    delete[] data;

    ibo->bind();
    vboSize = 0;
    GLuint *indices = new GLuint[6 * int(ceil(height_ * (s + vmargin) / gridSize) + 4) * int(ceil(width_ * (1.0 + 2.0 * hmargin) / gridSize) + 4)];

    int nj = 0;
    for (float j = height_ * s - 0.1f; j > -height_ * vmargin; j -= gridSize) {
        int ni = 0;
        for (float i = -width_ * hmargin; i < width_ * (1.0 + hmargin); i += gridSize) {
            indices[vboSize++] = ni + (nj + 1) * nx;
            indices[vboSize++] = (ni + 1) + (nj + 1) * nx;
            indices[vboSize++] = (ni + 1) + nj * nx;
            indices[vboSize++] = ni + (nj + 1) * nx;
            indices[vboSize++] = (ni + 1) + nj * nx;
            indices[vboSize++] = ni + nj * nx;
            ni++;
        }
        nj++;
    }

    glBufferData(GL_ARRAY_BUFFER, vboSize * 4, indices, GL_STATIC_DRAW);
    delete[] indices;
    ibo->release();
}

// ----------------------------------------------------------------------------
// WAVES GENERATION
// ----------------------------------------------------------------------------

long lrandom(long *seed)
{
    *seed = (*seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return *seed;
}

float frandom(long *seed)
{
    long r = lrandom(seed) >> (31 - 24);
    return r / (float)(1 << 24);
}

inline float grandom(float mean, float stdDeviation, long *seed)
{
    float x1, x2, w, y1;
    static float y2;
    static int use_last = 0;

    if (use_last) {
        y1 = y2;
        use_last = 0;
    } else {
        do {
            x1 = 2.0f * frandom(seed) - 1.0f;
            x2 = 2.0f * frandom(seed) - 1.0f;
            w  = x1 * x1 + x2 * x2;
        } while (w >= 1.0f);
        w  = sqrt((-2.0f * log(w)) / w);
        y1 = x1 * w;
        y2 = x2 * w;
        use_last = 1;
    }
    return mean + y1 * stdDeviation;
}

#define srnd() (2*frandom(&seed) - 1)

void GLSea::generateWaves()
{
    long seed = 1234567;
    float min = log(lambdaMin) / log(2.0f);
    float max = log(lambdaMax) / log(2.0f);

    sigmaXsq = 0.0;
    sigmaYsq = 0.0;
    meanHeight = 0.0;
    heightVariance = 0.0;
    amplitudeMax = 0.0;

    delete[] waves;
    waves = new glm::vec4[nbWaves];

#define nbAngles 5 // even
#define angle(i) (1.5*(((i)%nbAngles)/(float)(nbAngles/2)-1))
#define dangle() (1.5/(float)(nbAngles/2))

    float Wa[nbAngles]; // normalised gaussian samples
    int index[nbAngles]; // to hash angle order
    float s=0;
    for (int i = 0; i < nbAngles; i++) {
        index[i] = i;
        float a = angle(i);
        s += Wa[i] = exp(-0.5*a*a);
    }
    for (int i = 0; i < nbAngles; i++) {
        Wa[i] /= s;
    }

    for (int i = 0; i < nbWaves; ++i) {
        float x = i / (nbWaves - 1.0f);

        float lambda = pow(2.0f, (1.0f - x) * min + x * max);
        float ktheta = grandom(0.0f, 1.0f, &seed) * waveDispersion;
        float knorm = 2.0f * M_PI / lambda;
        float omega = sqrt(9.81f * knorm);
        float amplitude;

        float step = (max-min) / (nbWaves-1); // dlambda/di
        float omega0 = 9.81 / U0;
        if ((i%(nbAngles)) == 0) { // scramble angle ordre
            for (int k = 0; k < nbAngles; k++) {   // do N swap in indices
                int n1 = lrandom(&seed)%nbAngles, n2 = lrandom(&seed)%nbAngles, n;
                n = index[n1];
                index[n1] = index[n2];
                index[n2] = n;
            }
        }
        ktheta = waveDispersion * (angle(index[(i)%nbAngles]) + 0.4*srnd()*dangle());
        ktheta *= 1.0 / (1.0 + 40.0*pow(omega0/omega,4));
        amplitude = (8.1e-3*9.81*9.81) / pow(omega,5) * exp(-0.74*pow(omega0/omega,4));
        amplitude *= 0.5*sqrt(2*M_PI*9.81/lambda)*nbAngles*step;
        amplitude = 3*heightMax*sqrt(amplitude);

        if (amplitude > 1.0f / knorm) {
            amplitude = 1.0f / knorm;
        } else if (amplitude < -1.0f / knorm) {
            amplitude = -1.0f / knorm;
        }

        waves[i].x = amplitude;
        waves[i].y = omega;
        waves[i].z = knorm * cos(ktheta);
        waves[i].w = knorm * sin(ktheta);
        sigmaXsq += pow(cos(ktheta), 2.0f) * (1.0 - sqrt(1.0 - knorm * knorm * amplitude * amplitude));
        sigmaYsq += pow(sin(ktheta), 2.0f) * (1.0 - sqrt(1.0 - knorm * knorm * amplitude * amplitude));
        meanHeight -= knorm * amplitude * amplitude * 0.5f;
        heightVariance += amplitude * amplitude * (2.0f - knorm * knorm * amplitude * amplitude) * 0.25f;
        amplitudeMax += fabs(amplitude);
    }

    float var = 4.0f;
    amplitudeMax = 2.0f * var * sqrt(heightVariance);

    glActiveTexture(GL_TEXTURE0 + USV_WAVE_UNIT);
    assert(glGetError() == 0);
    glBindTexture(GL_TEXTURE_1D, waveTex);
    assert(glGetError() == 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    assert(glGetError() == 0);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, nbWaves, 0, GL_RGBA, GL_FLOAT, waves);
    assert(glGetError() == 0);
}


void GLSea::render(glm::vec3& eyePos, glm::mat4& proj, glm::mat4& view, glm::vec3& sunPos, double time) {
    if (vboParams.x != width_ || vboParams.y != height_ || vboParams.z != gridSize || vboParams.w != cameraTheta) {
        generateMesh();
    }

//    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();

    glm::mat4 screenToCamera = glm::inverse(proj);
    glm::mat4 cameraToWorld = glm::inverse(view);

    float worldToWind[4];
    worldToWind[0] = cos(waveDirection);
    worldToWind[1] = sin(waveDirection);
    worldToWind[2] = -sin(waveDirection);
    worldToWind[3] = cos(waveDirection);
    float windToWorld[4];
    windToWorld[0] = cos(waveDirection);
    windToWorld[1] = -sin(waveDirection);
    windToWorld[2] = sin(waveDirection);
    windToWorld[3] = cos(waveDirection);


    glm::vec3 sun = glm::normalize(sunPos);

    glUniformMatrix4fv(glGetUniformLocation(m_program->programId(), "screenToCamera"), 1, true, glm::value_ptr(screenToCamera));
    glUniformMatrix4fv(glGetUniformLocation(m_program->programId(), "cameraToWorld"), 1, true, glm::value_ptr(cameraToWorld));
    glm::mat4 worldToScreen = proj * view;
    glUniformMatrix4fv(glGetUniformLocation(m_program->programId(), "worldToScreen"), 1, true, glm::value_ptr(worldToScreen));
    glUniformMatrix2fv(glGetUniformLocation(m_program->programId(), "worldToWind"), 1, true, worldToWind);
    glUniformMatrix2fv(glGetUniformLocation(m_program->programId(), "windToWorld"), 1, true, windToWorld);
    glUniform3f(glGetUniformLocation(m_program->programId(), "worldCamera"), eyePos.x, eyePos.y, eyePos.z);
    glUniform3f(glGetUniformLocation(m_program->programId(), "worldSunDir"), sun.x, sun.y, sun.z);
    glUniform1f(glGetUniformLocation(m_program->programId(), "hdrExposure"), hdrExposure);

    glUniform1f(glGetUniformLocation(m_program->programId(), "nbWaves"), nbWaves);
    glUniform1f(glGetUniformLocation(m_program->programId(), "heightOffset"), -meanHeight);
    glUniform2f(glGetUniformLocation(m_program->programId(), "sigmaSqTotal"), sigmaXsq, sigmaYsq);
    glUniform1f(glGetUniformLocation(m_program->programId(), "time"), time);

    glUniform4f(glGetUniformLocation(m_program->programId(), "lods"),
                gridSize,
                atan(2.0 / height_) * gridSize, // angle under which a screen pixel is viewed from the camera * gridSize
                log(lambdaMin) / log(2.0f),
                (nbWaves - 1.0f) / (log(lambdaMax) / log(2.0f) -  log(lambdaMin) / log(2.0f)));
    glUniform1f(glGetUniformLocation(m_program->programId(), "nyquistMin"), nyquistMin);
    glUniform1f(glGetUniformLocation(m_program->programId(), "nyquistMax"), nyquistMax);

    glUniform3f(glGetUniformLocation(m_program->programId(), "seaColor"), seaColor[0] * seaColor[3], seaColor[1] * seaColor[3], seaColor[2] * seaColor[3]);
    assert(glGetError() == 0);
    if (grid) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo->bufferId());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->bufferId());
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) nullptr);
//    glVertexPointer(4, GL_FLOAT, 16, 0);
    glDrawElements(GL_TRIANGLES, vboSize, GL_UNSIGNED_INT, nullptr);
    glDisableVertexAttribArray(vertexLocation);
    assert(glGetError() == 0);
    vbo->release();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_program->release();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void GLSea::prepare_grid() {
    std::vector<GLfloat> vertices;
    vertices.reserve(gridsize * gridsize * 2);
    auto step = size / gridsize;
    auto zero = -size * 0.5f;
    for (size_t i = 0; i < gridsize; ++i)
        for (size_t j = 0; j < gridsize; ++j) {
            vertices.push_back(zero + i * step);
            vertices.push_back(zero + j * step);
        }

    std::vector<GLuint> indices;
    for (GLuint i = 0; i < gridsize - 1; ++i)
        for (GLuint j = 0; j < gridsize - 1; ++j) {
            // first triangle
            auto r = i * gridsize;
            indices.push_back(r + j + gridsize);
            indices.push_back(r + j + 1);
            indices.push_back(r + j);
            // second triangle
            indices.push_back(r + j + gridsize);
            indices.push_back(r + j + gridsize + 1);
            indices.push_back(r + j + 1);
        }

    vbo->bind();
    vbo->allocate(vertices.data(), static_cast<int>(data_sizeof(vertices)));
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(), static_cast<int>(data_sizeof(indices)));
    ibo->release();
}

float GLSea::getWidth() const {
    return width_;
}

void GLSea::setWidth(float width) {
    width_ = width;
}

float GLSea::getHeight() const {
    return height_;
}

void GLSea::setHeight(float height) {
    height_ = height;
}

float GLSea::getCameraTheta() const {
    return cameraTheta;
}

void GLSea::setCameraTheta(float cameraTheta_) {
    GLSea::cameraTheta = cameraTheta_;
}
