#include <QDebug>

#include "spinemeshitem.h"
#include "spinemeshrenderer.h"
#include "rendercmdscache.h"

#include <QOpenGLFramebufferObjectFormat>

SpineMeshItem::SpineMeshItem(QQuickItem *parent) :
    QQuickFramebufferObject(parent)
  , m_dataChanged(false)
  , m_image(NULL)
  , m_image2(NULL)
  , m_rendererNeedsMeshupdate(false)
{
//    qDebug() << Q_FUNC_INFO;    //    setFlag(QQuickItem::ItemHasContents);
    setTextureFollowsItemSize(true);
}

QQuickFramebufferObject::Renderer *SpineMeshItem::createRenderer() const
{
    qDebug() << Q_FUNC_INFO << m_source << m_image << m_image2 << m_image->size();
    m_renderer = new SpineMeshRenderer;
    m_renderer->setSource(m_image);
    if (m_image2)
        m_renderer->setSource2(m_image2);
    return m_renderer;
}

QVariantList SpineMeshItem::vertices() const
{
    return m_vertices;
}

QVariantList SpineMeshItem::uvs() const
{
    return m_uvs;
}

QList<int> SpineMeshItem::edges() const
{
    return m_edges;
}

QList<int> SpineMeshItem::triangles() const
{
    return m_triangles;
}

QRectF SpineMeshItem::meshRect() const
{
    return m_meshRect;
}

QRectF SpineMeshItem::origMeshRect() const
{
    return m_origMeshRect;
}

void SpineMeshItem::renderToCache(QQuickFramebufferObject::Renderer *renderer, RenderCmdsCache *cache)
{
//    if (!m_fbo) {
//        QOpenGLFramebufferObjectFormat format;
//        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
//        m_fbo = new QOpenGLFramebufferObject(size, format);
//        m_texture = m_window->createTextureFromId(m_fbo->texture(), size);
//        m_logoRenderer = new LogoRenderer();
//        m_logoRenderer->initialize();
//        setTexture(m_texture);
//    }
    //    Q_ASSERT(renderer);
    //    cache->clear();

    //    Q_ASSERT(renderer);

    //    SpineMeshRenderer* spineMeshRenderer = (SpineMeshRenderer*)renderer;

    //    cache->setSkeletonRect(QRectF(0,0,50,50));//rect);

    //    cache->bindShader(RenderCmdsCache::ShaderTexture);
    //    int additive = -1;
    //    Color color;
    ////    const float* uvs = 0;
    ////    float vertices[8] = {0.0,0.0,0.0,1.0,1.0,1.0,1.0,0.0};
    ////    float uvs[8] = {0.0,0.0,0.0,1.0,1.0,1.0,1.0,0.0};
    //    float vertices[8] = {-50.0,-50.0,50.0,-50.0,50.0,50.0,50.0,-50.0};
    //    float uvs[8] = {0.0,0.0,0.0,1.0,1.0,1.0,1.0,0.0};
    //    int verticesCount = 4;
    //    int triangles[6] = {0,1,2,2,0,3 };
    ////    const int* triangles = {0,1,2,1,3,2 };
    //    int trianglesCount = 2;
    //    float r = 0, g = 0, b = 0, a = 0;
    ////    for (int i = 0, n = mspSkeleton->slotsCount; i < n; i++) {
    ////        spSlot* slot = mspSkeleton->drawOrder[i];
    ////        if (!slot->attachment)
    ////            continue;

    ////        Texture *texture = 0;

    ////            spMeshAttachment* attachment = (spMeshAttachment*)slot->attachment;
    ////            spMeshAttachment_computeWorldVertices(attachment, slot, mWorldVertices);
    ////            texture = getTexture(attachment);
    ////            uvs = attachment->uvs;
    ////            verticesCount = attachment->verticesCount;
    ////            triangles = attachment->triangles;
    ////            trianglesCount = attachment->trianglesCount;
    //            r = 1.0;//attachment->r;
    //            g = 1.0;//attachment->g;
    //            b = 1.0;//attachment->b;
    //            a = 1.0;//attachment->a;

    //            color.r = color.g = color.b = color.a = 1.0;
    ////            color = QColor(1,1,1,1);
    ////                color.a = mspSkeleton->a * slot->a * a * 255;
    ////                float multiplier = mPremultipliedAlapha ? color.a : 255;
    ////                color.r = mspSkeleton->r * slot->r * r * multiplier;
    ////                color.g = mspSkeleton->g * slot->g * g * multiplier;
    ////                color.b = mspSkeleton->b * slot->b * b * multiplier;
    ////            cache->drawTriangles(spineMeshRenderer->getOpenGLTexture(), vertices, uvs, verticesCount);
    //                cache->drawTriangles(spineMeshRenderer->getOpenGLTexture(), vertices, uvs, verticesCount, triangles, trianglesCount, color);
    ////            }// END if (texture)


    ////    }// END for (int i = 0, n = skeleton->slotsCount; i < n; i++)
    //    cache->cacheTriangleDrawCall();

    //    cache->pointSize(4.0);
    //    cache->drawColor(0, 0, 255, 255);
    //    cache->drawPoint(Point(0, 0));
    //    cache->drawLine(Point(0,0), Point(255,255));
}

void SpineMeshItem::setDataChanged(bool changed)
{
    m_dataChanged = changed;
}

bool SpineMeshItem::isDataChanged() const
{
    return m_dataChanged;
}

QString SpineMeshItem::source() const
{
    return m_source;
}

QString SpineMeshItem::source2() const
{
    return m_source2;
}

float SpineMeshItem::blendingFactor() const
{
    return m_blendingFactor;
}

bool SpineMeshItem::rendererNeedsMeshUpdate() const
{
    return m_rendererNeedsMeshupdate;
}

//QRectF SpineMeshItem::calculateSkeletonRect()
//{
//    if (!isSkeletonValid())
//        return QRectF();

//    float minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;
//    for (int i = 0; i < mspSkeleton->slotsCount; ++i) {
//        spSlot* slot = mspSkeleton->slots[i];
//        if (!slot->attachment)
//            continue;

//        int verticesCount;
//        if (slot->attachment->type == SP_ATTACHMENT_REGION) {
//            spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
//            spRegionAttachment_computeWorldVertices(attachment, slot->bone, mWorldVertices);
//            verticesCount = 8;
//        } else if (slot->attachment->type == SP_ATTACHMENT_MESH) {
//            spMeshAttachment* mesh = (spMeshAttachment*)slot->attachment;
//            spMeshAttachment_computeWorldVertices(mesh, slot, mWorldVertices);
//            verticesCount = mesh->verticesCount;
//        } else if (slot->attachment->type == SP_ATTACHMENT_SKINNED_MESH) {
//            spSkinnedMeshAttachment* mesh = (spSkinnedMeshAttachment*)slot->attachment;
//            spSkinnedMeshAttachment_computeWorldVertices(mesh, slot, mWorldVertices);
//            verticesCount = mesh->uvsCount;
//        } else
//            continue;

//        for (int ii = 0; ii < verticesCount; ii += 2) {
//            float x = mWorldVertices[ii], y = mWorldVertices[ii + 1];
//            minX = qMin(minX, x);
//            minY = qMin(minY, y);
//            maxX = qMax(maxX, x);
//            maxY = qMax(maxY, y);
//        }
//    }

//    QRectF rect(minX, minY, maxX - minX, maxY - minY);
//    return rect;
//}



void SpineMeshItem::setVertices(QVariantList arg)
{
    if (m_vertices == arg)
        return;

    m_vertices = arg;

    polish();

    emit verticesChanged(arg);
}

void SpineMeshItem::setUvs(QVariantList arg)
{
    if (m_uvs == arg)
        return;

    m_uvs = arg;
    polish();

    emit uvsChanged(arg);
}

void SpineMeshItem::setEdges(QList<int> arg)
{
    if (m_edges == arg)
        return;

    m_edges = arg;

    polish();

    emit edgesChanged(arg);
}

void SpineMeshItem::setTriangles(QList<int> arg)
{
    if (m_triangles == arg)
        return;

    m_triangles = arg;

    polish();

    emit trianglesChanged(arg);
}

void SpineMeshItem::setSource(QString arg)
{
    qDebug() << Q_FUNC_INFO << arg;
    if (m_source == arg)
        return;

    m_source = arg;

    if (m_image) {
        delete m_image;

    }

    m_image = new QImage(":"+ m_source);

    qDebug() << Q_FUNC_INFO << m_image->size();

    polish();

    emit sourceChanged(arg);
}

void SpineMeshItem::setMeshRect(QRectF arg)
{
    if (m_meshRect == arg) {
        return;
    }

    m_meshRect = arg;

    if (m_origMeshRect == QRectF()) {
        m_origMeshRect = m_meshRect;
    }

    setSize(m_meshRect.size());
    setX(m_origMeshRect.left() + m_origMeshRect.width() - m_meshRect.width() - m_origMeshRect.right() + m_meshRect.right());
//    setY(-(m_meshRect.height() - m_origMeshRect.height()) + m_origMeshRect.top() - m_meshRect.top() - m_origMeshRect.bottom());
    setY(-m_meshRect.height() + m_origMeshRect.height() + m_origMeshRect.top() - m_meshRect.top() - m_origMeshRect.bottom());
//    setPosition(-m_meshRect.center());

//    qDebug() << Q_FUNC_INFO << arg << arg.top() << arg.bottom();

    emit meshRectChanged(arg);
}

void SpineMeshItem::setSource2(QString arg)
{
    if (m_source2 == arg)
        return;

    m_source2 = arg;

    if (m_image2) {
        delete m_image2;

    }

    m_image2 = new QImage(":"+ m_source2);

    emit source2Changed(arg);
}

void SpineMeshItem::setBlendingFactor(float arg)
{
    if (m_blendingFactor == arg)
        return;

    m_blendingFactor = arg;
    emit blendingFactorChanged(arg);
}

void SpineMeshItem::setRendererNeedsMeshUpdate(bool value)
{
    m_rendererNeedsMeshupdate = value;
}

void SpineMeshItem::updatePolish()
{
    setRendererNeedsMeshUpdate(true);
    setMeshRect(calculateMeshRect());
    update();
    QQuickItem::updatePolish();
}

QRectF SpineMeshItem::calculateMeshRect() const
{
    float minX = FLT_MAX;
    float maxX = FLT_MIN;
    float minY = FLT_MAX;
    float maxY = FLT_MIN;

    for(int i = 0; i < m_vertices.count(); i+=2) {
        if (m_vertices[i].toFloat() < minX) {
            minX = m_vertices[i].toFloat();
        }
        if (m_vertices[i].toFloat() > maxX) {
            maxX = m_vertices[i].toFloat();
        }
        if (m_vertices[i+1].toFloat() < minY) {
            minY = m_vertices[i+1].toFloat();
        }
        if (m_vertices[i+1].toFloat() > maxY) {
            maxY = m_vertices[i+1].toFloat();
        }
    }

    return QRectF(minX, minY, (maxX - minX), maxY - minY);
}

//QSGNode *SpineMeshItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData)
//{
//    qDebug() << Q_FUNC_INFO << "test";
//    QQuickFramebufferObject::updatePaintNode(node, nodeData);
//}
