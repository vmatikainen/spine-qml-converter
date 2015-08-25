#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

#include <QMatrix4x4>
#include <QQuickTransform>

class BoneTransform;
class IKJoint;

class BoneTransform : public QQuickTransform
{
    Q_OBJECT
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged)

    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal dx READ dx WRITE setDx NOTIFY dxChanged)
    Q_PROPERTY(qreal dy READ dy WRITE setDy NOTIFY dyChanged)

    Q_PROPERTY(qreal xScale READ xScale WRITE setXScale NOTIFY xScaleChanged)
    Q_PROPERTY(qreal yScale READ yScale WRITE setYScale NOTIFY yScaleChanged)
    Q_PROPERTY(qreal dxScale READ dxScale WRITE setDxScale NOTIFY dxScaleChanged)
    Q_PROPERTY(qreal dyScale READ dyScale WRITE setDyScale NOTIFY dyScaleChanged)

    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(qreal dangle READ dangle WRITE setDangle NOTIFY dangleChanged)

    Q_PROPERTY(bool inheritScale READ inheritScale WRITE setInheritScale NOTIFY inheritScaleChanged)
    Q_PROPERTY(bool inheritRotation READ inheritRotation WRITE setInheritRotation NOTIFY inheritRotationChanged)

    Q_PROPERTY(bool reverse READ reverse WRITE setReverse NOTIFY reverseChanged)

    Q_PROPERTY(BoneTransform* parentBone READ parentBone WRITE setParentBone NOTIFY parentBoneChanged)
    // lets make this unintuitive, if pointTo is set and joint not, then its singular point.
    // if both are set, then its the first bone in joint and controls the second.
    // else if joint is set and point not, then its the second bone and controlled by parent.
    Q_PROPERTY(BoneTransform* pointTo READ pointTo WRITE setPointTo NOTIFY pointToChanged)
    Q_PROPERTY(IKJoint* joint READ joint WRITE setJoint NOTIFY jointChanged)
//    Q_PROPERTY(BoneTransform* jointBone READ jointBone WRITE setJointBone NOTIFY jointBoneChanged)
//    Q_PROPERTY(bool bendPositive READ bendPositive WRITE setBendPositive NOTIFY bendPositiveChanged)
//    Q_PROPERTY(qreal mix READ mix WRITE setMix NOTIFY mixChanged) // unimplemented

public:
    explicit BoneTransform(QObject *parent = 0);

    virtual void applyTo(QMatrix4x4 *matrix) const;

    const QMatrix4x4& matrix() const;

    BoneTransform* parentBone() const;

    qreal y() const;
    qreal x() const;
    qreal dx() const;
    qreal dy() const;

    qreal xScale() const;
    qreal yScale() const;
    qreal dxScale() const;
    qreal dyScale() const;
    qreal angle() const;
    qreal dangle() const;

    bool inheritScale() const;
    bool inheritRotation() const;

    void worldPosition(QPointF* point);
    Q_INVOKABLE QPointF worldPosition();
    void worldScale(qreal * const xScale, qreal * const yScale) const;
    void worldRotation(qreal* const angle);
    Q_INVOKABLE qreal worldRotation();

    bool reverse() const;

    BoneTransform* pointTo() const;
    qreal worldLength() const;
    qreal length() const;
    IKJoint* joint() const;

signals:
    void matrixChanged(const QMatrix4x4& matrix);
    void parentBoneChanged(BoneTransform* arg);

    void yChanged(qreal arg);
    void xChanged(qreal arg);
    void dxChanged(qreal arg);
    void dyChanged(qreal arg);

    void yScaleChanged(qreal arg);
    void xScaleChanged(qreal arg);
    void dxScaleChanged(qreal arg);
    void dyScaleChanged(qreal arg);

    void angleChanged(qreal arg);
    void dangleChanged(qreal arg);

    void transformChanged();

    void inheritScaleChanged(bool arg);
    void inheritRotationChanged(bool arg);

    void reverseChanged(bool arg);

    void pointToChanged(BoneTransform* arg);
    void lengthChanged(qreal arg);
    void jointChanged(IKJoint* arg);

public slots:

    void setParentBone(BoneTransform* arg);
    void setY(qreal arg);
    void setX(qreal arg);
    void setDx(qreal arg);
    void setDy(qreal arg);
    void setXScale(qreal arg);
    void setYScale(qreal arg);
    void setDxScale(qreal arg);
    void setDyScale(qreal arg);
    void setAngle(qreal arg);
    void setDangle(qreal arg);

    void setInheritScale(bool arg);
    void setInheritRotation(bool arg);

    void reset();

    void setReverse(bool arg);

    void setPointTo(BoneTransform* arg);
    void setLength(qreal arg);

    void setJoint(IKJoint* arg);

private slots:
    void updateMatrix();
    void setTransformChanged();

private:

private:
    bool m_matrixNeedsUpdate;
    QMatrix4x4 m_matrix;
    qreal m_y;
    qreal m_x;
    qreal m_dx;
    qreal m_dy;
    BoneTransform* m_parentBone;
    qreal m_xScale;
    qreal m_yScale;
    qreal m_dxScale;
    qreal m_dyScale;
    qreal m_angle;
    qreal m_dangle;
    qreal m_pointToAngle;
    bool m_inheritScale;
    bool m_inheritRotation;
    bool m_reverse;
    BoneTransform* m_pointTo;
    qreal m_length;
    IKJoint* m_joint;

    friend class IKJoint;
};

#endif // BONETRANSFORM_H
