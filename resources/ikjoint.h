#ifndef IKJOINT_H
#define IKJOINT_H

#include <QObject>

#include "bonetransform.h"

class IKJoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BoneTransform* first READ first WRITE setFirst NOTIFY firstChanged)
    Q_PROPERTY(BoneTransform* second READ second WRITE setSecond NOTIFY secondChanged)
    Q_PROPERTY(BoneTransform* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(bool bendPositive READ bendPositive WRITE setBendPositive NOTIFY bendPositiveChanged)
    Q_PROPERTY(qreal mix READ mix WRITE setMix NOTIFY mixChanged)

public:
    explicit IKJoint(QObject *parent = 0);

    virtual ~IKJoint();

    BoneTransform* first() const;
    BoneTransform* second() const;
    BoneTransform* target() const;
    bool bendPositive() const;
    qreal mix() const;

    qreal getAngle(BoneTransform* bone);

signals:

    void firstChanged(BoneTransform* arg);
    void secondChanged(BoneTransform* arg);
    void targetChanged(BoneTransform* arg);
    void bendPositiveChanged(bool arg);
    void mixChanged(qreal arg);

public slots:
    void setFirst(BoneTransform* arg);
    void setSecond(BoneTransform* arg);
    void setTarget(BoneTransform* arg);
    void setBendPositive(bool arg);
    void setMix(qreal arg);

    void setUpdateTransforms();

private:
    void takeControlOfBones();
    void solveAngles();

private:
    BoneTransform* m_first;
    BoneTransform* m_second;
    BoneTransform* m_target;
    bool m_bendPositive;
    qreal m_mix;
    bool m_needsTransformUpdate;
    qreal m_angle1;
    qreal m_angle2;
};

#endif // IKJOINT_H
