#ifndef SPINESKINNEDMESHITEM_H
#define SPINESKINNEDMESHITEM_H

#include "spinemeshitem.h"
#include "bonetransform.h"

class SpineSkinnedMeshItem : public SpineMeshItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList boneVertices READ boneVertices WRITE setBoneVertices NOTIFY boneVerticesChanged)
    Q_PROPERTY(QVariantList weights READ weights WRITE setWeights NOTIFY weightsChanged)
    Q_PROPERTY(QList<int> boneCounts READ boneCounts WRITE setBoneCounts NOTIFY boneCountChanged)
    Q_PROPERTY(QList<QObject*> bones READ bones WRITE setBones NOTIFY bonesChanged)

public:
    explicit SpineSkinnedMeshItem(QQuickItem *parent = 0);

    virtual QVariantList vertices() const;

    QVariantList weights() const;
    QVariantList boneVertices() const;
    QList<int> boneCounts() const;
    QList<QObject*> bones() const;

signals:

    void weightsChanged(QVariantList arg);
    void boneVerticesChanged(QVariantList arg);
    void boneCountChanged(QList<int> arg);
    void bonesChanged(QList<QObject*> arg);

public slots:

    void setWeights(QVariantList arg);
    void setBoneVertices(QVariantList arg);
    void setBoneCounts(QList<int> arg);
    void setBones(QList<QObject*> arg);

    void updateMesh();

protected:
    virtual void updatePolish();
//    virtual void itemChange(ItemChange change, const ItemChangeData &value);

private:
    void calculateVertices();

private:
    QVariantList m_weights;
    QVariantList m_boneVertices;
    QList<int> m_boneCounts;
    QList<QObject*> m_bones;
};

#endif // SPINESKINNEDMESHITEM_H
