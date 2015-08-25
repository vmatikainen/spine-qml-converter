#include <QApplication>
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QVariantAnimation>
#include <QQuickWindow>

#include "ikjoint.h"
#include "spinemeshitem.h"
#include "spineskinnedmeshitem.h"
#include "bonetransform.h"

QVariant variantListInterpolator(const QVariantList &start, const QVariantList &end, qreal progress)
{
    Q_ASSERT(start.count() == end.count());

    QVariantList ret;
    for(int i = 0; i < start.count(); i++ ) {
        ret += QVariant(start[i].toDouble() + (end[i].toDouble() - start[i].toDouble()) * progress);
    }
    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterAnimationInterpolator<QVariantList>(variantListInterpolator);
    qmlRegisterType<SpineMeshItem>("Spine", 1, 0, "SpineMeshItem");
    qmlRegisterType<SpineSkinnedMeshItem>("Spine", 1, 0, "SpineSkinnedMeshItem");
    qmlRegisterType<BoneTransform>("Spine", 1, 0, "BoneTransform");
    qmlRegisterType<IKJoint>("Spine", 1, 0, "IKJoint");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}


