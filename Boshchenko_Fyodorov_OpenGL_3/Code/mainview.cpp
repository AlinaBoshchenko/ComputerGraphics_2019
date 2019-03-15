#include "mainview.h"
#include "model.h"
#include "vertex.h"

#include <math.h>
#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";

    glDeleteTextures(1, texturePtr);

    destroyModelBuffers();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    createShaderProgram();
    loadMesh(":/models/grid.obj", &grid_meshSize, &grid_meshVAO, &grid_meshVBO);
    loadMesh(":/models/cat.obj", &cat_meshSize, &cat_meshVAO, &cat_meshVBO);
    loadMesh(":/models/sphere.obj", &cube_meshSize, &cube_meshVAO, &cube_meshVBO);

    loadTextures();

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
    timer.start(1000.0 / 60.0);
}

void MainView::createShaderProgram()
{
    // Create Normal Shader program
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader_normal.glsl");
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader_normal.glsl");
    normalShaderProgram.link();

    // Create Gouraud Shader program
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader_gouraud.glsl");
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader_gouraud.glsl");
    gouraudShaderProgram.link();

    // Create Phong Shader program
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();

    // Create Water Shader program
    waterShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader_water.glsl");
    waterShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader_water.glsl");
    waterShaderProgram.link();

    // Get the uniforms for the normal shader.
    uniformModelViewTransformNormal  = normalShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformNormal = normalShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformNormal     = normalShaderProgram.uniformLocation("normalTransform");

    // Get the uniforms for the gouraud shader.
    uniformModelViewTransformGouraud  = gouraudShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformGouraud = gouraudShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformGouraud     = gouraudShaderProgram.uniformLocation("normalTransform");
    uniformMaterialGouraud            = gouraudShaderProgram.uniformLocation("material");
    uniformLightPositionGouraud       = gouraudShaderProgram.uniformLocation("lightPosition");
    uniformLightColourGouraud         = gouraudShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerGouraud      = gouraudShaderProgram.uniformLocation("textureSampler");

    // Get the uniforms for the phong shader.
    uniformModelViewTransformPhong  = phongShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong     = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong            = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong       = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColourPhong         = phongShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerPhong      = phongShaderProgram.uniformLocation("textureSampler");

    // Get the uniforms for the water shader.
    uniformModelViewTransformWater  = waterShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformWater = waterShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformWater     = waterShaderProgram.uniformLocation("normalTransform");
    uniformMaterialWater            = waterShaderProgram.uniformLocation("material");
    uniformLightPositionWater       = waterShaderProgram.uniformLocation("lightPosition");
    uniformLightColourWater         = waterShaderProgram.uniformLocation("lightColour");
    uniformAmplitudeWater           = waterShaderProgram.uniformLocation("amplitude");
    uniformFrequencyWater           = waterShaderProgram.uniformLocation("frequency");
    uniformPhaseWater               = waterShaderProgram.uniformLocation("phase");
    uniformTimeWater                = waterShaderProgram.uniformLocation("t");
}

void MainView::loadMesh(const char * filename, GLuint *meshSize, GLuint *meshVAO, GLuint *meshVBO)
{
    Model model(filename);
    model.unitize();
    QVector<float> meshData = model.getVNTInterleaved();

    *meshSize = model.getVertices().size();

    // Generate VAO
    glGenVertexArrays(1, meshVAO);
    glBindVertexArray(*meshVAO);

    // Generate VBO
    glGenBuffers(1, meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set vertex normals to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set vertex texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MainView::loadTextures()
{
    glGenTextures(4, texturePtr);

    loadTexture(":/textures/cat_diff.png", texturePtr[0]);
    loadTexture(":/textures/ea.jpg", texturePtr[1]);
    loadTexture(":/textures/space.jpg", texturePtr[2]);
     loadTexture(":/textures/rug_logo.png", texturePtr[3]);
}

void MainView::loadTexture(QString file, GLuint texturePtr)
{
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, texturePtr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClearColor(0.5f, 0.1f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the texture(s)
    glActiveTexture(GL_TEXTURE0);

    // The switch works as such:
    // switch(x)
    //    case:
    //       bind shaders
    //       update the models
    //       bind appropriate VAO
    //       update uniforms
    //       draw the object
    //
    // The above case is repeated and the correct VAO/Uniforms
    // are used when needed. Phong + Normal + Gouraud will have 4 object.
    // The expection is that waterShader does not have anything fancy, just the water.

    // Choose the selected shader.
    QOpenGLShaderProgram *shaderProgram;
    switch (currentShader) {
        case NORMAL:
            timeInc += 0.003;
            shaderProgram = &normalShaderProgram;
            shaderProgram->bind();
            updateModelTransforms();

            glBindVertexArray(cat_meshVAO);
            updateNormalUniforms(modelNormalTransformCat, modelTransformCat);
            glDrawArrays(GL_TRIANGLES, 0, cat_meshSize);

            updateNormalUniforms(modelNormalTransformCat2, modelTransformCat2);
            glDrawArrays(GL_TRIANGLES, 0, cat_meshSize);

            glBindVertexArray(cube_meshVAO);
            updateNormalUniforms(modelNormalTransformCube, modelTransformCube);
            glDrawArrays(GL_TRIANGLES, 0, cube_meshSize);
            updateNormalUniforms(modelNormalTransformCube2, modelTransformCube2);
            glDrawArrays(GL_TRIANGLES, 0, cube_meshSize);
            break;
        case GOURAUD:
            timeInc += 0.003;
            shaderProgram = &gouraudShaderProgram;
            shaderProgram->bind();
            updateModelTransforms();

            glBindTexture(GL_TEXTURE_2D, texturePtr[0]);
            glBindVertexArray(cat_meshVAO);
            updateGouraudUniforms(modelNormalTransformCat, modelTransformCat);
            glDrawArrays(GL_TRIANGLES, 0, cat_meshSize);
            glBindTexture(GL_TEXTURE_2D, texturePtr[3]);
            updateGouraudUniforms(modelNormalTransformCat2, modelTransformCat2);
            glDrawArrays(GL_TRIANGLES, 0, cat_meshSize);

            glBindTexture(GL_TEXTURE_2D, texturePtr[1]);
            glBindVertexArray(cube_meshVAO);
            updateGouraudUniforms(modelNormalTransformCube, modelTransformCube);
            glDrawArrays(GL_TRIANGLES, 0, cube_meshSize);

            glBindTexture(GL_TEXTURE_2D, texturePtr[2]);
            updateGouraudUniforms(modelNormalTransformCube2, modelTransformCube2);
            glDrawArrays(GL_TRIANGLES, 0, cube_meshSize);


            break;
        case PHONG:
            timeInc += 0.003;
            shaderProgram = &phongShaderProgram;
            shaderProgram->bind();
            updateModelTransforms();

            glBindTexture(GL_TEXTURE_2D, texturePtr[0]);
            glBindVertexArray(cat_meshVAO);
            updatePhongUniforms(modelNormalTransformCat, modelTransformCat);
            glDrawArrays(GL_TRIANGLES, 0, cat_meshSize);
            glBindTexture(GL_TEXTURE_2D, texturePtr[3]);
            updatePhongUniforms(modelNormalTransformCat2, modelTransformCat2);
            glDrawArrays(GL_TRIANGLES, 0, cat_meshSize);

            glBindTexture(GL_TEXTURE_2D, texturePtr[1]);
            glBindVertexArray(cube_meshVAO);
            updatePhongUniforms(modelNormalTransformCube, modelTransformCube);
            glDrawArrays(GL_TRIANGLES, 0, cube_meshSize);

            glBindTexture(GL_TEXTURE_2D, texturePtr[2]);
            updateGouraudUniforms(modelNormalTransformCube2, modelTransformCube2);
            glDrawArrays(GL_TRIANGLES, 0, cube_meshSize);

            break;
        case WATER:
            movement += 0.003;
            shaderProgram = &waterShaderProgram;
            shaderProgram->bind();
            updateWaterUniforms();

            glBindVertexArray(grid_meshVAO);
            glDrawArrays(GL_TRIANGLES, 0, grid_meshSize);
            break;
    }
    shaderProgram->release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    updateProjectionTransform();
}


// The next four functions below are to edit the uniforms (Normal, Gouraud, Phong, Water),
// the data that will edit them is sent through as parameters
void MainView::updateNormalUniforms(QMatrix3x3 meshNormalTrans, QMatrix4x4 meshTrans){
    glUniformMatrix4fv(uniformProjectionTransformNormal, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformNormal, 1, GL_FALSE, meshTrans.data());
    glUniformMatrix3fv(uniformNormalTransformNormal, 1, GL_FALSE, meshNormalTrans.data());
}

void MainView::updateGouraudUniforms(QMatrix3x3 meshNormalTrans, QMatrix4x4 meshTrans){
    glUniformMatrix4fv(uniformProjectionTransformGouraud, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformGouraud, 1, GL_FALSE, meshTrans.data());
    glUniformMatrix3fv(uniformNormalTransformGouraud, 1, GL_FALSE, meshNormalTrans.data());

    glUniform4fv(uniformMaterialGouraud, 1, &material[0]);
    glUniform3fv(uniformLightPositionGouraud, 1, &lightPosition[0]);
    glUniform3fv(uniformLightColourGouraud, 1, &lightColour[0]);

    glUniform1i(uniformTextureSamplerGouraud, 0); // Redundant now, but useful when you have multiple textures.
}

void MainView::updatePhongUniforms(QMatrix3x3 meshNormalTrans, QMatrix4x4 meshTrans){
    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, meshTrans.data());
    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, meshNormalTrans.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3fv(uniformLightColourPhong, 1, &lightColour[0]);

    glUniform1i(uniformTextureSamplerPhong, 0);
}

void MainView::updateWaterUniforms(){
    glUniformMatrix4fv(uniformProjectionTransformWater, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformWater, 1, GL_FALSE, modelTransformWater.data());
    glUniformMatrix3fv(uniformNormalTransformWater, 1, GL_FALSE, modelNormalTransformWater.data());

    glUniform4fv(uniformMaterialWater, 1, &materialWater[0]);
    glUniform3fv(uniformLightPositionWater, 1, &lightPositionWater[0]);
    glUniform3fv(uniformLightColourWater, 1, &lightColourWater[0]);

    glUniform1fv(uniformAmplitudeWater, numWaves, amplitude);
    glUniform1fv(uniformFrequencyWater, numWaves, frequency);
    glUniform1fv(uniformPhaseWater, numWaves, phase);
    glUniform1f(uniformTimeWater,  movement);
}

void MainView::updateProjectionTransform(){
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms(){
    // The first cat, this one rotates around a single axis
    modelTransformCat.setToIdentity();
    modelTransformCat.translate(1.5, 0, -4);
    modelTransformCat.scale(scaleCat);
    modelTransformCat.rotate(QQuaternion::fromEulerAngles(rotationCat + QVector3D(timeInc,0.0f,0.0f)*5)); // The rotation
    modelNormalTransformCat = modelTransformCat.normalMatrix();

    // The first sphere - EARTH, this one moves around in the z-plane in a circle
    modelTransformCube.setToIdentity();
    modelTransformCube.translate(-0.5 - 1 * cos(timeInc) / 10, -0.3 + sin(timeInc) / 10,-4); // The translation
    modelTransformCube.scale(scaleCube / 1.5);
    modelTransformCube.rotate(QQuaternion::fromEulerAngles(rotationCube)) ;
    modelNormalTransformCube = modelTransformCube.normalMatrix();

    // The Second sphere - SPACE, this sphere has its scale continously change
    modelTransformCube2.setToIdentity();
    modelTransformCube2.translate(0, 0, -20);
    modelTransformCube2.scale(scaleCube2);
    modelTransformCube2.rotate(QQuaternion::fromEulerAngles(rotationCube2 + QVector3D(timeInc,-timeInc,timeInc)*2)) ; // The rotaion
    modelNormalTransformCube2 = modelTransformCube2.normalMatrix();

    // The second cat, this cat rotates around all axis
    modelTransformCat2.setToIdentity();
    modelTransformCat2.translate(-1.5, 1, -4);
    modelTransformCat2.scale(scaleCat2 * fmod(timeInc / 10,3) * 0.1 + 0.2); //  The scaling
    modelTransformCat2.rotate(QQuaternion::fromEulerAngles(rotationCat2)) ;
    modelNormalTransformCat2 = modelTransformCat2.normalMatrix();

    // The water model, this model does not have anything extra added to it
    modelTransformWater.setToIdentity();
    modelTransformWater.translate(0,0,-4);
    modelTransformWater.scale(scaleWater);
    modelTransformWater.rotate(QQuaternion::fromEulerAngles(rotationWater)) ;
    modelNormalTransformWater = modelTransformWater.normalMatrix();

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers(){
    glDeleteBuffers(1, &grid_meshVBO);
    glDeleteVertexArrays(1, &grid_meshVAO);
    glDeleteBuffers(1, &cat_meshVBO);
    glDeleteVertexArrays(1, &cat_meshVAO);
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ){
    rotationCat = { static_cast<float>(rotateX), static_cast<float>(rotateY) , static_cast<float>(rotateZ) };
    rotationCube = { static_cast<float>(rotateX), static_cast<float>(rotateY) , static_cast<float>(rotateZ) };
    rotationCat2 = { static_cast<float>(rotateX), static_cast<float>(rotateY) , static_cast<float>(rotateZ) };
    rotationCube2 = { static_cast<float>(rotateX), static_cast<float>(rotateY) , static_cast<float>(rotateZ) };
    rotationWater = { static_cast<float>(rotateX), static_cast<float>(rotateY) , static_cast<float>(rotateZ) };

    updateModelTransforms();
}

void MainView::setScale(int newScale){
    scaleCat = static_cast<float>(newScale) / 100.f;
    scaleCube = static_cast<float>(newScale) / 100.f;
    scaleCat2 = static_cast<float>(newScale) / 100.f;
   // scaleCube2 = static_cast<float>(newScale) / 100.f;
    scaleWater = static_cast<float>(newScale) / 100.f;

    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading){
    qDebug() << "Changed shading to" << shading;
    currentShader = shading;
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}

