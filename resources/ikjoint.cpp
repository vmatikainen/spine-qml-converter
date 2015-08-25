#include <QtMath>

#include "ikjoint.h"

// IKJoint analytical solution by Ryan Juckett and modified and fit to C++


/******************************************************************************
  Copyright (c) 2008-2009 Ryan Juckett
  http://www.ryanjuckett.com/

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
     distribution.
******************************************************************************/

void IKJoint::solveAngles()
{
    qreal xScale, yScale;
    m_first->worldScale(&xScale, &yScale);
    qreal secondX = m_second->x() * xScale;
    qreal secondY = m_second->y() * yScale;
    double length1 = qSqrt(secondX * secondX + secondY * secondY);      // Length of bone 1. Assumed to be >= zero
    double length2  = m_second->worldLength();      // Length of bone 2. Assumed to be >= zero
    QPointF targetPos = m_target->worldPosition();
    QPointF originPos = m_first->worldPosition();

    double targetX = targetPos.x() - originPos.x();      // Target x position for the bones to reach
    double targetY = targetPos.y() - originPos.y();       // Target y position for the bones to reach

    Q_ASSERT(length1 >= 0.0);
    Q_ASSERT(length2 >= 0.0);

    const double epsilon = 0.0001; // used to prevent division by small numbers

//    bool foundValidSolution = true;

    double targetDistSqr = (targetX*targetX + targetY*targetY);

    //===
    // Compute a new value for angle2 along with its cosine
    double sinAngle2;
    double cosAngle2;

    double cosAngle2_denom = 2*length1*length2;
    if( cosAngle2_denom > epsilon )
    {
        cosAngle2 =   (targetDistSqr - length1*length1 - length2*length2)
                / (cosAngle2_denom);

        // if our result is not in the legal cosine range, we can not find a
        // legal solution for the target
//        if( (cosAngle2 < -1.0) || (cosAngle2 > 1.0) )
//            foundValidSolution = false;

        // clamp our value into range so we can calculate the best
        // solution when there are no valid ones
        cosAngle2 = qMax((double)-1, qMin( (double)1, cosAngle2 ) );

        // compute a new value for angle2
        m_angle2 = qAcos( cosAngle2 );

        // adjust for the desired bend direction
        if( !m_bendPositive )
            m_angle2 = -m_angle2;

        // compute the sine of our angle
        sinAngle2 = qSin( m_angle2 );
    }
    else
    {
        // At least one of the bones had a zero length. This means our
        // solvable domain is a circle around the origin with a radius
        // equal to the sum of our bone lengths.
//        double totalLenSqr = (length1 + length2) * (length1 + length2);
//        if(    targetDistSqr < (totalLenSqr-epsilon)
//               || targetDistSqr > (totalLenSqr+epsilon) )
//        {
//            foundValidSolution = false;
//        }

        // Only the value of angle1 matters at this point. We can just
        // set angle2 to zero.
        m_angle2    = 0.0;
        cosAngle2 = 1.0;
        sinAngle2 = 0.0;
    }

    //===
    // Compute the value of angle1 based on the sine and cosine of angle2
    double triAdjacent = length1 + length2*cosAngle2;
    double triOpposite = length2*sinAngle2;

    double tanY = targetY*triAdjacent - targetX*triOpposite;
    double tanX = targetX*triAdjacent + targetY*triOpposite;

    // Note that it is safe to call Atan2(0,0) which will happen if targetX and
    // targetY are zero
    m_angle1 = qAtan2( tanY, tanX );

    m_angle1 = qRadiansToDegrees(m_angle1);
    m_angle2 = qRadiansToDegrees(m_angle2) + m_angle1;

    //        return foundValidSolution;
}




IKJoint::IKJoint(QObject *parent) :
    QObject(parent)
  , m_first(0)
  , m_second(0)
  , m_target(0)
  , m_bendPositive(true)
  , m_mix(1.0)
  , m_needsTransformUpdate(false)
  , m_angle1(0)
  , m_angle2(0)
{
}


IKJoint::~IKJoint()
{
    qDebug() << Q_FUNC_INFO;
}

BoneTransform *IKJoint::first() const
{
    return m_first;
}

BoneTransform *IKJoint::second() const
{
    return m_second;
}

BoneTransform *IKJoint::target() const
{
    return m_target;
}

bool IKJoint::bendPositive() const
{
    return m_bendPositive;
}

qreal IKJoint::mix() const
{
    return m_mix;
}

qreal IKJoint::getAngle(BoneTransform *bone)
{
    if (m_needsTransformUpdate) {
        solveAngles();
        m_needsTransformUpdate = false;
    }

    if (bone == m_first) {
        return m_angle1;
    } else if (bone == m_second) {
        return m_angle2;
    } else {
        return 0;
    }
}

void IKJoint::setFirst(BoneTransform *arg)
{
    if (m_first == arg)
        return;

    m_first = arg;
    takeControlOfBones();
    emit firstChanged(arg);
}

void IKJoint::setSecond(BoneTransform *arg)
{
    if (m_second == arg)
        return;

    m_second = arg;
    takeControlOfBones();
    emit secondChanged(arg);
}

void IKJoint::setTarget(BoneTransform *arg)
{
    if (m_target == arg)
        return;

    m_target = arg;
    takeControlOfBones();
    emit targetChanged(arg);
}

void IKJoint::setBendPositive(bool arg)
{
    if (m_bendPositive == arg)
        return;

    m_bendPositive = arg;
    emit bendPositiveChanged(arg);
}

void IKJoint::setMix(qreal arg)
{
    if (m_mix == arg)
        return;

    m_mix = arg;
    emit mixChanged(arg);
}

void IKJoint::setUpdateTransforms()
{
    m_needsTransformUpdate = true;
    m_first->setTransformChanged();
    m_second->setTransformChanged();
}

void IKJoint::takeControlOfBones()
{
    if(m_first != NULL && m_second != NULL && m_target != NULL)     {
        qDebug() << Q_FUNC_INFO;
        connect(m_first, &BoneTransform::transformChanged, this, &IKJoint::setUpdateTransforms);
        connect(m_target, &BoneTransform::transformChanged, this, &IKJoint::setUpdateTransforms);

        // later for the longer chains
        m_first->setJoint(this);
        m_second->setJoint(this);

        setUpdateTransforms();

        //        qDebug() << m_first << m_first->parentBone() << m_second << m_second->parentBone() << m_target << m_target->parentBone() << m_first->x();
        //        // lock em real good!
        //        BoneTransform* b = m_second;
        //        while (b != m_first) {
        //            qDebug() << Q_FUNC_INFO << b << b->parentBone();
        //            b->setJoint(this);
        //            b = b->parentBone();
        //        }
        //        b->setJoint(this);
    }
}

