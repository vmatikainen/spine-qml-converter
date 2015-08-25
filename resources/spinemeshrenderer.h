#ifndef SPINEMESHRENDERER_H
#define SPINEMESHRENDERER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QQuickFrameBufferObject>

class RenderCmdsCache;

class SpineMeshRenderer : public QQuickFramebufferObject::Renderer
{
public:
    SpineMeshRenderer();

//    QOpenGLTexture *getOpenGLTexture(/*Texture* texture*/);

    virtual QOpenGLFramebufferObject* createFramebufferObject(const QSize & size);

    void setSource(QImage* image);
    void setSource2(QImage* image);

protected:
    virtual void render();
    virtual void synchronize(QQuickFramebufferObject *);

public slots:
    void updateMesh();

private:
    void initialize();

private:
    bool m_meshChanged;

    QOpenGLShaderProgram program1;
    QOpenGLShaderProgram multiTextureProgram;
    RenderCmdsCache* mCache;
    
    QOpenGLTexture *m_texture;
    QOpenGLTexture *m_texture2;
    QImage *m_image;

    bool m_initialized;

    GLuint vertexBuffer;
    QVector<GLfloat> vertices;
    QVector<GLfloat> m_boneVertices;
    QVector<GLfloat> m_boneWeights;
    QVector<GLchar> m_boneIndices;
    QVector<GLushort> m_triangles;
    QVector<GLfloat> uvs;
    QVector<QVector2D> uvs_vec;
    int vertexAttr1;
    int uvAttr1;
    int vertexAttr2;
    int uvAttr2;
    int uniform2;
    int boneVertexAttr;
    int boneIndexAttr;
    int boneWeightAttr;
//     int normalAttr1;
//     int matrixUniform1;
};

#endif // SPINEMESHRENDERER_H
