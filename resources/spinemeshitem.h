#ifndef SPINEMESHITEM_H
#define SPINEMESHITEM_H

#include <QQuickFramebufferObject>

class RenderCmdsCache;
class SpineMeshRenderer;

class SpineMeshItem : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList vertices READ vertices WRITE setVertices NOTIFY verticesChanged)
    Q_PROPERTY(QVariantList uvs READ uvs WRITE setUvs NOTIFY uvsChanged)
    Q_PROPERTY(QList<int> edges READ edges WRITE setEdges NOTIFY edgesChanged)
    Q_PROPERTY(QList<int> triangles READ triangles WRITE setTriangles NOTIFY trianglesChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString source2 READ source2 WRITE setSource2 NOTIFY source2Changed)
    Q_PROPERTY(float blendingFactor READ blendingFactor WRITE setBlendingFactor NOTIFY blendingFactorChanged)

public:
    explicit SpineMeshItem(QQuickItem *parent = 0);

    virtual Renderer *createRenderer() const;

    virtual QVariantList vertices() const;
    QVariantList uvs() const;
    QList<int> edges() const;
    QList<int> triangles() const;
    QRectF meshRect() const;
    QRectF origMeshRect() const;

    void renderToCache(QQuickFramebufferObject::Renderer* renderer, RenderCmdsCache* cache);

    void setDataChanged(bool changed);
    bool isDataChanged() const;
    QString source() const;

    QString source2() const;

    float blendingFactor() const;

    bool rendererNeedsMeshUpdate() const;

protected:
    //virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);
    QRectF calculateMeshRect() const;

    virtual void updatePolish();

private:

signals:

    void verticesChanged(QVariantList arg);
    void uvsChanged(QVariantList arg);
    void edgesChanged(QList<int> arg);
    void trianglesChanged(QList<int> arg);
    void sourceChanged(QString arg);
    void meshRectChanged(QRectF arg);

    void source2Changed(QString arg);

    void blendingFactorChanged(float arg);

public slots:

    void setVertices(QVariantList arg);
    void setUvs(QVariantList arg);
    void setEdges(QList<int> arg);
    void setTriangles(QList<int> arg);
    void setSource(QString arg);
    void setMeshRect(QRectF arg);

    void setSource2(QString arg);

    void setBlendingFactor(float arg);
    void setRendererNeedsMeshUpdate(bool value);

protected:
    bool m_dataChanged;
    QVariantList m_vertices;

private:
    QVariantList m_uvs;
    QList<int> m_edges;
    QList<int> m_triangles;
    QString m_source;
    mutable SpineMeshRenderer* m_renderer;
    QRectF m_meshRect;
    QRectF m_origMeshRect;

    QImage* m_image;
    QImage* m_image2;
    QString m_source2;
    float m_blendingFactor;

    bool m_rendererNeedsMeshupdate;
};

#endif // SPINEMESHITEM_H
