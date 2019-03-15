#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <QImage>
#include <QVector>
#include <memory>
#include <QMatrix4x4>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram normalShaderProgram,
                         gouraudShaderProgram,
                         phongShaderProgram,
                         waterShaderProgram;

    // Uniforms for the normal shader.
    GLint uniformModelViewTransformNormal;
    GLint uniformProjectionTransformNormal;
    GLint uniformNormalTransformNormal;

    // Uniforms for the gouraud shader.
    GLint uniformModelViewTransformGouraud;
    GLint uniformProjectionTransformGouraud;
    GLint uniformNormalTransformGouraud;

    GLint uniformMaterialGouraud;
    GLint uniformLightPositionGouraud;
    GLint uniformLightColourGouraud;

    GLint uniformTextureSamplerGouraud;

    // Uniforms for the phong shader.
    GLint uniformModelViewTransformPhong;
    GLint uniformProjectionTransformPhong;
    GLint uniformNormalTransformPhong;

    GLint uniformMaterialPhong;
    GLint uniformLightPositionPhong;
    GLint uniformLightColourPhong;

    GLint uniformTextureSamplerPhong;

    // Uniforms for the water shader.
    GLint uniformModelViewTransformWater;
    GLint uniformProjectionTransformWater;
    GLint uniformNormalTransformWater;

    GLint uniformMaterialWater;
    GLint uniformLightPositionWater;
    GLint uniformLightColourWater;

    GLint uniformAmplitudeWater;
    GLint uniformFrequencyWater;
    GLint uniformPhaseWater;
    GLint uniformTimeWater;

    // Buffers
    GLuint grid_meshVAO;
    GLuint grid_meshVBO;
    GLuint grid_meshSize;

    GLuint cat_meshVAO;
    GLuint cat_meshVBO;
    GLuint cat_meshSize;

    GLuint cube_meshVAO;
    GLuint cube_meshVBO;
    GLuint cube_meshSize;

    GLuint cat2_meshVAO;
    GLuint cat2_meshVBO;
    GLuint cat2_meshSize;

    GLuint cube2_meshVAO;
    GLuint cube2_meshVBO;
    GLuint cube2_meshSize;

    // Texture
    GLuint *texturePtr = new GLuint[4];

    // Transforms
    QMatrix4x4 projectionTransform;
    float timeInc = 0;

    // Transforms for cat
    float scaleCat = 0.5f;
    QVector3D rotationCat;
    QMatrix3x3 modelNormalTransformCat;
    QMatrix4x4 modelTransformCat;

    // Transforms for cube
    float scaleCube = 1.2f;
    QVector3D rotationCube;
    QMatrix3x3 modelNormalTransformCube;
    QMatrix4x4 modelTransformCube;

    // Transforms for cat2
    float scaleCat2 = 2.f;
    QVector3D rotationCat2;
    QMatrix3x3 modelNormalTransformCat2;
    QMatrix4x4 modelTransformCat2;

    // Transforms for cube2
    float scaleCube2 = 15.f;
    QVector3D rotationCube2;
    QMatrix3x3 modelNormalTransformCube2;
    QMatrix4x4 modelTransformCube2;

    // Phong model constants.
    QVector4D material = {0.5, 0.5, 1, 5};
    QVector3D lightPosition = {1, 100, 1};
    QVector3D lightColour = {1, 1, 1};

    // Transforms for water
    float scaleWater = 1.f;
    QVector3D rotationWater;
    QMatrix3x3 modelNormalTransformWater;
    QMatrix4x4 modelTransformWater;

    // Wave properties.
    int numWaves = 8;
    float movement = 0;
    float amplitude[8] = {0.02, 0.03, 0.01, 0.01, 0.05, 0.01, 0.02, 0.03};
    float frequency[8] = {2.5, 6.0, 3.0, 3.5, 6.0, 1.0, 9.0, 4.0};
    float phase[8]     = {2.2, 0.7, 0.0, 0.3, 2.3, 1.1, 3.0, 0.9};

    // Wave material properties.
    QVector4D materialWater = {0.5, 0.5, 1, 5};
    QVector3D lightPositionWater = {-10, 10, 10};
    QVector3D lightColourWater = {1, 1, 1};

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD, WATER
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();
    void loadMesh(const char * filename, GLuint *meshSize, GLuint *meshVAO, GLuint *meshVBO);

    // Loads texture data into the buffer of texturePtr.
    void loadTextures();
    void loadTexture(QString file, GLuint texturePtr);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();

    void updateNormalUniforms(QMatrix3x3 meshNormalTrans, QMatrix4x4 meshTrans);
    void updateGouraudUniforms(QMatrix3x3 meshNormalTrans, QMatrix4x4 meshTrans);
    void updatePhongUniforms(QMatrix3x3 meshNormalTrans, QMatrix4x4 meshTrans);
    void updateWaterUniforms();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // The current shader to use.
    ShadingMode currentShader = PHONG;
};

#endif // MAINVIEW_H
