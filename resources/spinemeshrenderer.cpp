#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions>

#include "spinemeshitem.h"
#include "spinemeshrenderer.h"
#include "rendercmdscache.h"

SpineMeshRenderer::SpineMeshRenderer() :
    m_texture(NULL)
  , m_texture2(NULL)
  , m_initialized(false)
  , m_meshChanged(true)
{
    qDebug() << Q_FUNC_INFO;
    //    //    glGenBuffers(1, &vertexBuffer);
    //    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), vertices.constData(), GL_STATIC_DRAW);

    QOpenGLShader *vshader1 = new QOpenGLShader(QOpenGLShader::Vertex, &program1);
    const char *vsrc1 =
            "attribute highp vec2 a_position;\n"
            "attribute lowp vec4 a_color;\n"
            "attribute mediump vec2 a_texCoord;"
            ""
            "varying lowp vec4 v_color;\n"
            "varying mediump vec2 v_texCoord;\n"
            ""
            "void main(void)\n"
            "{\n"
            "    v_color = vec4(1,1,1,1);\n"
            "    gl_Position = vec4(a_position.xy,0,1);\n"
            "    v_texCoord = a_texCoord;\n"
            "}\n";
    vshader1->compileSourceCode(vsrc1);

    QOpenGLShader *fshader1 = new QOpenGLShader(QOpenGLShader::Fragment, &program1);
    // !!!!!!!!!!! TODO NOTE, for some reason the texture color is off by half
    const char *fsrc1 =
            "varying lowp vec4 v_color;\n"
            "varying mediump vec2 v_texCoord;\n"
            "uniform sampler2D u_texture; \n"
            "void main() {\n"
            "   gl_FragColor = texture2D(u_texture, v_texCoord);\n"
            "   //gl_FragColor = vec4(v_texCoord.x, v_texCoord.y, 1, 1 );//texture2D(u_texture, v_texCoord);\n"
            "}\n";
    fshader1->compileSourceCode(fsrc1);

    program1.addShader(vshader1);
    program1.addShader(fshader1);
    program1.link();

    vertexAttr1 = program1.attributeLocation("a_position");
    uvAttr1 = program1.attributeLocation("a_texCoord");
    //    normalAttr1 = program1.attributeLocation("normal");
    //    matrixUniform1 = program1.uniformLocation("matrix");

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //    m_fAngle = 0;
    //    m_fScale = 1;
    //    createGeometry();

//    QImage joo("dude_face.png");
////    joo.fill(QColor(255,0,0, 255));


//    m_texture = new QOpenGLTexture(joo/*QImage("head.png")*/ );

//    m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
//    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);




    // multi texturing

    QOpenGLShader *vshader2 = new QOpenGLShader(QOpenGLShader::Vertex, &multiTextureProgram);
    const char *vsrc2 =
            "attribute highp vec2 a_position;\n"
            "attribute lowp vec4 a_color;\n"
            "attribute mediump vec2 a_texCoord;"
            ""
            "varying lowp vec4 v_color;\n"
            "varying mediump vec2 v_texCoord;\n"
            ""
            "void main(void)\n"
            "{\n"
            "    v_color = vec4(1,1,1,1);\n"
            "    gl_Position = vec4(a_position.xy,0,1);\n"
            "    v_texCoord = a_texCoord;\n"
            "}\n";
    vshader2->compileSourceCode(vsrc2);

    QOpenGLShader *fshader2 = new QOpenGLShader(QOpenGLShader::Fragment, &multiTextureProgram);
    // !!!!!!!!!!! TODO NOTE, for some reason the texture color is off by half
    const char *fsrc2 =
            "varying lowp vec4 v_color;\n"
            "varying mediump vec2 v_texCoord;\n"
            "uniform sampler2D u_texture;\n"
            "uniform sampler2D u_texture2;\n"
            "void main() {\n"
            "   gl_FragColor = texture2D(u_texture, v_texCoord) + texture2D(u_texture2, v_texCoord);\n"
            "}\n";
    fshader2->compileSourceCode(fsrc2);

    multiTextureProgram.addShader(vshader2);
    multiTextureProgram.addShader(fshader2);
    multiTextureProgram.link();

    vertexAttr2 = multiTextureProgram.attributeLocation("a_position");
    uvAttr2 = multiTextureProgram.attributeLocation("a_texCoord");
    uniform2 = multiTextureProgram.uniformLocation("fuk");
}

void SpineMeshRenderer::initialize()
{
}

void SpineMeshRenderer::render()
{
//    qDebug() << Q_FUNC_INFO;
    //    if (!m_initialized) {
    //        initialize();
    //        m_initialized = true;
    //    }

    glDepthMask(true);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glDisable(GL_CULL_FACE);
//    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glEnable (GL_BLEND);
//    glDisable (GL_BLEND);
    // spine blend func when not additive, that is yet umimplemented
//    glBlendFunc (GL_ONE, GL_ONE);

//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc (GL_SRC_ALPHA, GL_ONE );

    //    QMatrix4x4 modelview;
    //    modelview.rotate(m_fAngle, 0.0f, 1.0f, 0.0f);
    //    modelview.rotate(m_fAngle, 1.0f, 0.0f, 0.0f);
    //    modelview.rotate(m_fAngle, 0.0f, 0.0f, 1.0f);
    //    modelview.scale(m_fScale);
    //    modelview.translate(0.0f, -0.2f, 0.0f);

    if (m_texture2 == 0) {
        program1.bind();
        m_texture->bind();
        //        program1.setUniformValue("u_texture", 0);

//                qDebug() << Q_FUNC_INFO << m_texture->isBound(0);
        //    glBindTexture(GL_TEXTURE_2D, m_texture);
        //    m_texture->bind();

    //    qDebug() << "huh " << uvs_vec << uvs;
    //    qDebug() << "huh2 " << vertices_vec << vertices;
    //    qDebug() << "huh3 " << m_triangles << m_triangles2;

        program1.enableAttributeArray(uvAttr1);
        program1.enableAttributeArray(vertexAttr1);
        //    program1.setAttributeArray(vertexAttr1, vertices_vec.constData());
        //    program1.setAttributeArray(uvAttr1, uvs_vec.constData());
        program1.setAttributeArray(vertexAttr1, vertices.constData(), 2 );
        program1.setAttributeArray(uvAttr1, uvs.constData(), 2);
        glDrawElements(GL_TRIANGLES, m_triangles.count(), GL_UNSIGNED_SHORT, m_triangles.constData());
        //    glDrawElements(GL_TRIANGLES, m_triangles2.count(), GL_UNSIGNED_SHORT, m_triangles2.constData());
        //    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        program1.disableAttributeArray(uvAttr1);
        program1.disableAttributeArray(vertexAttr1);

        //    program1.setUniformValue(matrixUniform1, modelview);
        //    paintQtLogo();
        program1.release();
    } else {
        multiTextureProgram.bind();
        m_texture->bind(0);
        multiTextureProgram.setUniformValue("u_texture", 0);
        m_texture2->bind(1);
        multiTextureProgram.setUniformValue("u_texture2", 1);

        // todo note bug, without this line the multitexturing doesn't work, don't ask me why
        m_texture->isBound(0);

        multiTextureProgram.enableAttributeArray(uvAttr2);
        multiTextureProgram.enableAttributeArray(vertexAttr2);
        multiTextureProgram.enableAttributeArray(vertexAttr2);
        multiTextureProgram.setAttributeArray(vertexAttr2, vertices.constData(), 2 );

        multiTextureProgram.enableAttributeArray(uvAttr2);
        multiTextureProgram.enableAttributeArray(vertexAttr2);
        multiTextureProgram.setAttributeArray(vertexAttr2, vertices.constData(), 2 );
        multiTextureProgram.setAttributeArray(uvAttr2, uvs.constData(), 2);
        glDrawElements(GL_TRIANGLES, m_triangles.count(), GL_UNSIGNED_SHORT, m_triangles.constData());
        multiTextureProgram.disableAttributeArray(uvAttr2);
        multiTextureProgram.disableAttributeArray(vertexAttr2);

        multiTextureProgram.release();
    }

    glDisable (GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void SpineMeshRenderer::synchronize(QQuickFramebufferObject *item)
{
    SpineMeshItem* spineMeshItem = qobject_cast<SpineMeshItem*>(item);
    if (spineMeshItem->rendererNeedsMeshUpdate()) {
        m_triangles.clear();
        vertices.clear();
        uvs.clear();
        QVariantList vert = spineMeshItem->vertices() ;
        QVariantList uvs = spineMeshItem->uvs();

        QRectF rect = spineMeshItem->meshRect();

        for(int i = 0; i < vert.count(); i+=2) {
            vertices.append(((vert[i].toFloat() - rect.left()) / rect.width() - 0.5) * 2);
            vertices.append(((vert[i+1].toFloat() - rect.top()) / rect.height() - 0.5) * -2);
        }

        for (int i = 0; i < uvs.count(); i++) {
            this->uvs.append(uvs[i].toFloat());
        }
        QList<int> tri = spineMeshItem->triangles();
        foreach(int t, tri) {
            m_triangles.append(t);
        }

        spineMeshItem->setRendererNeedsMeshUpdate(true);
    }
}

//QOpenGLTexture *SpineMeshRenderer::getOpenGLTexture(/*Texture *texture*/)
//{
//    //    if (!texture || texture->name().isEmpty())
//    //        return 0;

//    //    if (mTextureHash.contains(texture->name()))
//    //        return mTextureHash.value(texture->name());

//    //    if (!texture->image())
//    //        return 0;
//    if (m_texture != NULL)
//        return m_texture;

//    m_texture = new QOpenGLTexture(*m_image);
//    m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
//    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
//    //    mTextureHash.insert(texture->name(), tex);
//    return m_texture;
//    //    return tex;
//}

QOpenGLFramebufferObject *SpineMeshRenderer::createFramebufferObject(const QSize &size)
{
//    qDebug() << Q_FUNC_INFO << size;
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // TODO NOTE HACK I just doubled the size and troubles went away
    return new QOpenGLFramebufferObject(size , format);
}

void SpineMeshRenderer::setSource(QImage *image)
{
    qDebug() << Q_FUNC_INFO << image->size();
    if (m_texture) {
        delete m_texture;
    }

    m_texture = new QOpenGLTexture(*image);

    m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
}

void SpineMeshRenderer::setSource2(QImage *image)
{
//    qDebug() << Q_FUNC_INFO << image->size();
    if (m_texture2) {
        delete m_texture2;
    }

    m_texture2 = new QOpenGLTexture(*image);

    m_texture2->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_texture2->setMagnificationFilter(QOpenGLTexture::Linear);
}
