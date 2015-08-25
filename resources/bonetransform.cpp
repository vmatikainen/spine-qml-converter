#include <QQuickItem>
#include <QtMath>

#include "bonetransform.h"
#include "ikjoint.h"

#include <qmatrix4x4.h>
#include <qtransform.h>

BoneTransform::BoneTransform(QObject *parent) :
    QQuickTransform(parent)
  , m_parentBone(NULL)
  , m_matrixNeedsUpdate(false)
  , m_x(0.0)
  , m_dx(0.0)
  , m_y(0.0)
  , m_dy(0.0)
  , m_angle(0.0)
  , m_dangle(0.0)
  , m_xScale(1.0)
  , m_dxScale(1.0)
  , m_yScale(1.0)
  , m_dyScale(1.0)
  , m_inheritScale(true)
  , m_inheritRotation(true)
  , m_reverse(false)
  , m_pointTo(NULL)
  , m_pointToAngle(0)
  , m_joint(NULL)
  , m_length(0.0)
{
}

void BoneTransform::applyTo(QMatrix4x4 *matrix) const
{
    const QMatrix4x4& ownMatrix = this->matrix();
//    matrix->translate(m_x, m_y);
    matrix->operator *=(ownMatrix);
//    matrix->translate(x, y);
}

BoneTransform *BoneTransform::parentBone() const
{
    return m_parentBone;
}

double BoneTransform::y() const
{
    return m_y;
}

double BoneTransform::x() const
{
    return m_x;
}

double BoneTransform::dx() const
{
    return m_dx;
}

double BoneTransform::dy() const
{
    return m_dy;
}

const QMatrix4x4 &BoneTransform::matrix() const
{
    if (m_matrixNeedsUpdate) {
        BoneTransform* this_non_const = const_cast<BoneTransform*>(this);
        this_non_const->updateMatrix();
    }

    return m_matrix;
}

qreal BoneTransform::xScale() const
{
    return m_xScale;
}

qreal BoneTransform::yScale() const
{
    return m_yScale;
}

qreal BoneTransform::dxScale() const
{
    return m_dxScale;
}

qreal BoneTransform::dyScale() const
{
    return m_dyScale;
}

qreal BoneTransform::angle() const
{
    return m_angle;
}

qreal BoneTransform::dangle() const
{
    return m_dangle;
}

bool BoneTransform::inheritScale() const
{
    return m_inheritScale;
}

bool BoneTransform::inheritRotation() const
{
    return m_inheritRotation;
}

qreal BoneTransform::worldLength() const
{
    qreal xScale;
    qreal yScale;
    worldScale(&xScale, &yScale);

    return (m_length * xScale);
}

void BoneTransform::worldPosition(QPointF* point)
{
    QMatrix4x4 ownTranslationMatrix;

    // todo note, add clause for dx and dy disable in case of joint
    ownTranslationMatrix.translate(m_x + m_dx, m_y + m_dy);
    if (m_parentBone) {
        ownTranslationMatrix = m_parentBone->matrix() * ownTranslationMatrix;
    }

    QTransform ownTranslationTransform = ownTranslationMatrix.toTransform();

    point->setX(ownTranslationTransform.dx());
    point->setY(ownTranslationTransform.dy());
}

QPointF BoneTransform::worldPosition()
{
    QMatrix4x4 ownTranslationMatrix;

    // todo note, add clause for dx and dy disable in case of joint
    ownTranslationMatrix.translate(m_x + m_dx, m_y + m_dy);
    if (m_parentBone) {
        ownTranslationMatrix = m_parentBone->matrix() * ownTranslationMatrix;
    }

    QTransform ownTranslationTransform = ownTranslationMatrix.toTransform();

    return QPointF(ownTranslationTransform.dx(), ownTranslationTransform.dy());
}


void BoneTransform::worldScale(qreal* const xScale, qreal* const yScale) const
{
    qreal x;
    qreal y;
    x = m_xScale * m_dxScale;
    y = m_yScale * m_dyScale;
    if (!m_inheritScale || !m_parentBone) { // recursion ends
        *xScale = x;
        *yScale = y;
    } else {
        m_parentBone->worldScale(xScale, yScale);
        *xScale *= x;
        *yScale *= y;
    }
}

void BoneTransform::worldRotation(qreal* const angle)
{
    qreal a;
    a = m_angle + m_dangle;
    if (!m_inheritRotation || !m_parentBone) { // recursion ends
        *angle = a;
    } else if (m_pointTo != NULL) { // also recursion ends
        *angle = m_pointToAngle;
    } else if (m_joint != NULL) {
//        m_parentBone->worldRotation(angle);
//        *angle = 0;
        *angle = m_joint->getAngle(this);
    }
    else {
        m_parentBone->worldRotation(angle);
        *angle += a;
    }
}

qreal BoneTransform::worldRotation()
{
    qreal *temp = new qreal;
    worldRotation(temp);
    return *temp;
}

bool BoneTransform::reverse() const
{
    return m_reverse;
}

BoneTransform *BoneTransform::pointTo() const
{
    return m_pointTo;
}

qreal BoneTransform::length() const
{
    return m_length;
}

IKJoint *BoneTransform::joint() const
{
    return m_joint;
}

void BoneTransform::setParentBone(BoneTransform *arg)
{
    if (m_parentBone == arg)
        return;

    if (m_parentBone != NULL) {
        disconnect(m_parentBone, &BoneTransform::transformChanged, this, &BoneTransform::setTransformChanged);
    }

    connect(arg, &BoneTransform::transformChanged, this, &BoneTransform::setTransformChanged);

    m_parentBone = arg;
    setTransformChanged();
    emit parentBoneChanged(arg);
}

void BoneTransform::setY(double arg)
{
    if (m_y == arg)
        return;

    m_y = arg;
    setTransformChanged();
    emit yChanged(arg);
}

void BoneTransform::setX(double arg)
{
    if (m_x == arg)
        return;

    m_x = arg;
    setTransformChanged();
    emit xChanged(arg);
}

void BoneTransform::setDx(double arg)
{
    if (m_dx == arg)
        return;

    m_dx = arg;
    setTransformChanged();
    emit dxChanged(arg);
}

void BoneTransform::setDy(double arg)
{
    if (m_dy == arg)
        return;

    m_dy = arg;
    setTransformChanged();
    emit dyChanged(arg);
}

void BoneTransform::setXScale(qreal arg)
{
    if (m_xScale == arg)
        return;

    m_xScale = arg;
    setTransformChanged();
    emit xScaleChanged(arg);
}

void BoneTransform::setYScale(qreal arg)
{
    if (m_yScale == arg)
        return;

    m_yScale = arg;
    setTransformChanged();
    emit yChanged(arg);
}

void BoneTransform::setDxScale(qreal arg)
{
    if (m_dxScale == arg)
        return;

    m_dxScale = arg;
    setTransformChanged();
    emit dxScaleChanged(arg);
}

void BoneTransform::setDyScale(qreal arg)
{
    if (m_dyScale == arg)
        return;

    m_dyScale = arg;
    setTransformChanged();
    emit dyScaleChanged(arg);
}

void BoneTransform::setAngle(qreal arg)
{
    if (m_angle == arg)
        return;

    m_angle = arg;
    setTransformChanged();
    emit angleChanged(arg);
}

void BoneTransform::setDangle(qreal arg)
{
    if (m_dangle == arg)
        return;

    m_dangle = arg;
    setTransformChanged();
    emit dangleChanged(arg);
}

void BoneTransform::reset()
{
    setDx(0);
    setDy(0);
    setDangle(0);
    setDxScale(0);
    setDyScale(0);
}

void BoneTransform::setReverse(bool arg)
{
    if (m_reverse == arg)
        return;

    m_reverse = arg;
    setTransformChanged();
    emit reverseChanged(arg);
}

void BoneTransform::setPointTo(BoneTransform *arg)
{
    qDebug() << Q_FUNC_INFO << arg;
    if (m_pointTo == arg)
        return;

    if (m_pointTo != NULL) {
        disconnect(m_pointTo, &BoneTransform::transformChanged, this, &BoneTransform::setTransformChanged);
    }

    connect(arg, &BoneTransform::transformChanged, this, &BoneTransform::setTransformChanged);

    m_pointTo = arg;
    setTransformChanged();
    emit pointToChanged(arg);
}

void BoneTransform::setLength(qreal arg)
{
    if (m_length == arg)
        return;

    m_length = arg;
    setTransformChanged();
    emit lengthChanged(arg);
}

void BoneTransform::setJoint(IKJoint *arg)
{
    if (m_joint == arg)
        return;

    m_joint = arg;
    setTransformChanged();
    emit jointChanged(arg);
}

void BoneTransform::setInheritScale(bool arg)
{
    if (m_inheritScale == arg)
        return;

    m_inheritScale = arg;
    setTransformChanged();
    emit inheritScaleChanged(arg);
}

void BoneTransform::setInheritRotation(bool arg)
{
    if (m_inheritRotation == arg)
        return;

    m_inheritRotation = arg;
    setTransformChanged();
    emit inheritRotationChanged(arg);
}


void BoneTransform::updateMatrix()
{
    // todo note, this was just set here as a safeguard as spamming this could kill all performance
    Q_ASSERT(m_matrixNeedsUpdate);
    Q_ASSERT(!(m_pointTo != NULL && m_joint != NULL)); // not both can be not null :D
    QMatrix4x4 ownMatrix;

    qreal ownAngle;

    if (m_pointTo != NULL) {
        QTransform pointToWorldTransform = m_pointTo->matrix().toTransform();
        QMatrix4x4 ownWorldMatrix;
        ownWorldMatrix.translate(m_x + m_dx, m_y + m_dy);
        ownWorldMatrix = m_parentBone->matrix() * ownWorldMatrix;
        QTransform ownWorldTransform = ownWorldMatrix.toTransform();

        m_pointToAngle = qRadiansToDegrees(qAtan2(pointToWorldTransform.dy()- ownWorldTransform.dy(), pointToWorldTransform.dx()- ownWorldTransform.dx()));
        ownAngle = m_pointToAngle; // not needed?
    } else if (m_joint != NULL) {
        ownAngle = m_joint->getAngle(this);
    } else {
        ownAngle = m_angle + m_dangle;
    }

    if (!m_reverse) {
        // todo note optimization point, scaling is used much more rarely, maybe this could be optimized to a case
        ownMatrix.translate(m_x + m_dx, m_y + m_dy);
        ownMatrix.rotate(ownAngle, 0, 0, 1);
//        if (m_pointTo == NULL) {
//            ownMatrix.rotate(m_angle + m_dangle, 0, 0, 1);
//        } else {
//            ownMatrix.rotate(m_pointToAngle, 0, 0, 1);
//        }
        ownMatrix.scale(m_xScale * m_dxScale, m_yScale * m_dyScale);
    //    ownMatrix.scale(m_xScale * m_dxScale, m_yScale * m_dyScale);
    //    ownMatrix.translate(m_x + m_dx, m_y + m_dy);
    //    ownMatrix.rotate(m_angle + m_dangle, 0, 0, 1);
    } else {
        // okay lets talk about this. If all parents are identity matrices, then this is true reverse world transformation
        // otherwise its something I don't know. The inheritscale and inheritrotation are also undefined. This was
        // written to streamline Spine scene camera implementation, as I didn't want to use three addition bones and qml
        // code to reverse the camera transform. Nor I needed a correct inverse tree transform.
        ownMatrix.scale(1 / (m_xScale * m_dxScale), 1 / (m_yScale * m_dyScale));
        ownMatrix.rotate(ownAngle, 0, 0, -1);
//        if (m_pointTo == NULL) {
//            ownMatrix.rotate(m_angle + m_dangle, 0, 0, -1);
//        } else {
//            ownMatrix.rotate(m_pointToAngle, 0, 0, -1); // untested
//        }
        ownMatrix.translate(-1 * (m_x + m_dx), -1 * (m_y + m_dy));
    }

    if (m_parentBone) {
        m_matrix = m_parentBone->matrix() * ownMatrix;
    } else {
        m_matrix = ownMatrix;
    }


    if (!m_inheritScale && m_parentBone) {
        qreal xScale;
        qreal yScale;
        m_parentBone->worldScale(&xScale, &yScale);
        m_matrix.scale(1/xScale, 1/yScale);
    }

    if ((!m_inheritRotation ||  m_pointTo != NULL || m_joint != NULL) && m_parentBone ) { // untested, but we don't care about reverse rotation if we are pointing
//        qDebug() << Q_FUNC_INFO << "yeah";
        qreal angle;
        m_parentBone->worldRotation(&angle);
        m_matrix.rotate(-angle, 0, 0, 1);
    }

    m_matrixNeedsUpdate = false;
}

void BoneTransform::setTransformChanged()
{
    if (!m_matrixNeedsUpdate) {
        m_matrixNeedsUpdate = true;
        update();
    }
    emit transformChanged();
}

//void BoneTransform::updateBindMatrix()
//{
//    m_bindMatrix.setToIdentity();
//    m_bindMatrix.translate(m_x, m_y);
//}
