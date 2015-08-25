#include "spineskinnedmeshitem.h"

#include <qmatrix4x4.h>
#include <qtransform.h>

#include <private/qquicktranslate_p.h>

SpineSkinnedMeshItem::SpineSkinnedMeshItem(QQuickItem *parent) :
    SpineMeshItem(parent)
{
}

QVariantList SpineSkinnedMeshItem::vertices() const
{
    return m_vertices;
}

QVariantList SpineSkinnedMeshItem::weights() const
{
    return m_weights;
}

QVariantList SpineSkinnedMeshItem::boneVertices() const
{
    return m_boneVertices;
}

QList<int> SpineSkinnedMeshItem::boneCounts() const
{
    return m_boneCounts;
}

QList<QObject *> SpineSkinnedMeshItem::bones() const
{
    return m_bones;
}

void SpineSkinnedMeshItem::setWeights(QVariantList arg)
{
    if (m_weights == arg)
        return;

    m_weights = arg;
    emit weightsChanged(arg);
}

void SpineSkinnedMeshItem::setBoneVertices(QVariantList arg)
{
    if (m_boneVertices == arg)
        return;

    m_boneVertices = arg;
    polish();
    emit boneVerticesChanged(arg);
}

void SpineSkinnedMeshItem::setBoneCounts(QList<int> arg)
{
    if (m_boneCounts == arg)
        return;

    m_boneCounts = arg;
    polish();
    emit boneCountChanged(arg);
}

void SpineSkinnedMeshItem::setBones(QList<QObject *> arg)
{
    //    if (m_bones == arg)
    //        return;

    m_bones = arg;

    // note this assumes bones don't change _ever_, don't change them
    foreach(QObject* b, m_bones) {
        BoneTransform* bone = qobject_cast<BoneTransform*>(b);
        connect(bone, &BoneTransform::transformChanged, this, &SpineSkinnedMeshItem::updateMesh);
    }

    polish();

    emit bonesChanged(arg);
}

void SpineSkinnedMeshItem::updateMesh()
{
    polish();
}

void SpineSkinnedMeshItem::updatePolish()
{
    calculateVertices();
    SpineMeshItem::updatePolish();
}

void SpineSkinnedMeshItem::calculateVertices()
{
    m_vertices.clear();

    QObject* attachment = qvariant_cast<QObject*>(this->property("parent"));
    Q_ASSERT(attachment);
    QObject* slot = qvariant_cast<QObject*>(attachment->property("parent"));
    Q_ASSERT(slot);
    BoneTransform* parentBone = qvariant_cast<BoneTransform*>(slot->property("bone"));
    Q_ASSERT(parentBone);

    // todo note, I think swapping to transform, inverting and then back to matrix is faster than matrix inversion
    QTransform invertedParentBoneTransform = parentBone->matrix().toTransform().inverted();
    QMatrix4x4 invertedParentBoneMatrix(invertedParentBoneTransform);// = parentBone->matrix().toTransform().inverted();

    int bi = 0;
    for(int i = 0; i < m_bones.count(); ) {
        int boneCount = m_boneCounts[bi++];
//        qDebug() << boneCount;

        double wx = 0.0;
        double wy = 0.0;
        // yeah, another late night session
        // todo note, yeah so it was a lot problematic. Currently it also applies the root transform which might cause problems
        // its also somewhat suboptimal, but visually correct, except if it twists a lot and draws a face on itself, then it looks different from spine editor
        // the updating doesn't work correctly, need new bone structure
        for (int last_i = 0; last_i < boneCount; last_i++) {
            double weight = m_weights[i].toDouble();
            double vx = m_boneVertices[i*2].toDouble();
            double vy = m_boneVertices[i*2+1].toDouble();
            QMatrix4x4 local(QTransform().translate(vx,vy));

            BoneTransform* bone = qobject_cast<BoneTransform*>(m_bones.at(i));
            QMatrix4x4 boneWorld = bone->matrix();

            local = /*invertedParentBoneMatrix * */boneWorld * local;
            QTransform localTransform = local.toTransform() * invertedParentBoneTransform * weight;
            wx += localTransform.dx();//local.toTransform()*invertedParentBoneTransform).dx() * weight;
            wy += localTransform.dy();//local.toTransform()*invertedParentBoneTransform).dy() * weight;

            i++;
        }
        m_vertices.append(QVariant(wx));

        // note the inverse, its everywhere!
        m_vertices.append(QVariant(wy*-1));
    }
}
