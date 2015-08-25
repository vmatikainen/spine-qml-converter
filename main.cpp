#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QDir>

using namespace std;

const QString jointStr = "Joint";
const QString boneStr = "Bone";
const QString slotStr = "Slot";
const QString attachmentStr = "Attachment";
const QString animationStr = "Animation";

QString recursiveBoneTransform(int level);
QString toStateQml(QString skin, QJsonObject skinObject);

QVariantList swapXY(QVariantList list) {
    Q_ASSERT(list.count() % 2 == 0);

    return list;
    QVariantList ret;
    for(int i = 0; i < list.count(); i++) {
        if (i % 2 == 1) {
            //            ret.append(QVariant(list[i].toDouble()*-1.0));
            ret.insert(ret.count()-1, list[i]);
        } else {
            ret.append(list[i]);
        }
    }

    return ret;
}

bool readDocumentFromFile(const QString& fileName, QJsonDocument& document) {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return false;
    }

    QByteArray data = file.readAll();

    document = QJsonDocument::fromJson(data);

    return true;
}

QString indent(QString data, int amount = 1)
{
    QString ret = data;
    if (ret.length() > 0) {
        int i = 0;
        do {
            ret.insert(i, '\t');
            i = ret.indexOf("\n", i) + 1;
        } while (i > 0);
    }

    return ret;
}

bool wrapQml(QString& data, QString imageFolder)
{
    data = indent(data);
    data.prepend("import QtQuick 2.0\n" \
                 "import Spine 1.0\n" \
                 "\n" \
                 "Skeleton {\n" \
                 "\tid: root\n" \
                 "\tproperty string imageFolder: \"" + imageFolder + "\"\n"\
                                                                     "\tproperty string imageFolderAndDash: \"qrc:/\" + imageFolder + \"/\"\n\n"\
                 );

    data.append("}\n");

    return true;
}

QString formatName(QJsonValue value) {
    QStringList words = value.toString().split(" ");
    QString ret = words.at(0);

    for(int i = 1; i < words.count(); i++) {
        QString word = words.at(i);
        ret += word.left(1).toUpper() + word.right(word.length()-1);
    }

    if (ret.at(0).isUpper()) {
        QChar temp = ret.at(0).toLower();
        ret.remove(0,1);
        ret.prepend(temp);
    }

    while (true) {
        int i = ret.indexOf("-");
        if (i == -1)
            break;
        ret.replace(i,1, '_');
    }

    while (true) {
        int i = ret.indexOf("/");
        if (i == -1)
            break;
        ret.replace(i,1, '_');
    }

    while (true) {
        int i = ret.indexOf(".");
        if (i == -1)
            break;
        ret.replace(i,1, '_');
    }

    return ret;
}

QString toResetFunctionLine(QString target, QString property, double value) {
    return target + "." + property + " = " + QString::number(value);
}

QString toResetFunctionLine(QString target, QString property, QString value) {
    return target + "." + property + " = " + value;
}

QString toBoneQml(QJsonObject object) {
    QString ret;
    ret += "property alias a" + formatName(object.value("name")) + boneStr + " : " + formatName(object.value("name")) + boneStr + "\n";
    ret += "BoneTransform {\n";
    ret += "\tid: " + formatName(object.value("name")) + boneStr + "\n";
    if (object.contains("parent")) {
        ret += "\tparentBone: " + formatName(object.value("parent")) + boneStr + "\n";
    }
    if (object.contains("x")) {
        ret += "\tx: " + QString::number(object.value("x").toDouble()) + "\n";
    }
    if (object.contains("y")) {
        // pay attention to inverse here * -1
        ret += "\ty: " + QString::number(object.value("y").toDouble()* -1) + "\n";
    }
    if (object.contains("scaleX")) {
        ret += "\txScale: " + QString::number(object.value("scaleX").toDouble()) + "\n";
    }
    if (object.contains("scaleY")) {
        ret += "\tyScale: " + QString::number(object.value("scaleY").toDouble()) + "\n";
    }
    if (object.contains("rotation")) {
        // pay attention to inverse here * -1
        ret += "\tangle: " + QString::number(object.value("rotation").toDouble() * -1) + "\n";
    }
    if (object.contains("inheritScale")) {
        ret += "\tinheritScale: " + (object.value("inheritScale").toBool() ? QString("true") : QString("false")) + "\n";
    }
    if (object.contains("inheritRotation")) {
        ret += "\tinheritRotation: " + (object.value("inheritRotation").toBool() ? QString("true") : QString("false")) + "\n";
    }
    if (object.contains("length")) {
        ret += "\tlength: " + QString::number(object.value("length").toDouble()) + "\n";
    }

    ret += "}\n";

    return ret;
}

QString recursiveBoneTransform(int level) {
    if (level == 0) {
        return QString();
    } else {
        QString bone;
        for( int i = 0; i < level; i++) {
            bone += "bone.";
        }
        return recursiveBoneTransform(level-1) + QString((level!=1 ? ", " : "") + bone + "scale, " + bone + "rot, " + bone + "trans");
    }
}

int findSlotLevel(QJsonObject slot, QJsonArray bones) {
    QJsonValue bone = slot.value("bone");
    int count = 0;
    bool end = false;
    while(end == false) {
        foreach(const QJsonValue& value, bones) {
            if (bone.isUndefined()) {
                end = true;
                break;
            }else if (value.toObject().value("name") == bone) {
                count++;
                bone = value.toObject().value("parent");
                break;
            }
        }
    }

    return count;
}

QString toARGBColor(QString rgbaColor) {
    QString ret = rgbaColor.left(6);
    ret.prepend(rgbaColor.right(2));
    return ret;
}


// todo note carve out level, its deprecated
QString toSlotQml(QJsonObject object, int level) {
    QString ret;
    ret += "Slot {\n";
    ret += "\tid: " + formatName(object.value("name")) + slotStr + "\n";
    ret += "\tparent: root\n";
    ret += "\tbone: " + formatName(object.value("bone")) + boneStr + "\n";
    if (object.contains("attachment")) {
        ret += "\tsetupAttachment: " + formatName(object.value("attachment")) + attachmentStr + "\n";
    }
    if (object.contains("color")) {
        ret += "\tsetupColor: \"#" + toARGBColor(object.value("color").toString()) + "\"\n";
    }

    //    ret += "\ttransform: [ " + recursiveBoneTransform(level)+ " ]\n";
    //    ret += "\ttransform: [ " + formatName(object.value("bone")) + boneStr + " ]\n";

    ret += "}\n";

    return ret;
}

QString fromArrayToString(QVariantList array) {
    QString ret;

    bool first = true;
    foreach(QVariant item, array) {
        if (!first) {
            ret += ", ";
        } else {
            first = false;
        }
        ret += QString::number(item.toDouble());
    }

    return ret;
}

QString fromArrayToPointString(QVariantList array) {
    QString ret;

    bool point = true;
    foreach(QVariant item, array) {
        if (point) {
            ret += "Qt.point( ";
        }
        ret += QString::number(item.toDouble());

        if (point) {
            ret += ", ";
        } else {
            ret += "), ";
        }

        point = !point;
    }

    return ret;
}

QVariantList invertEverySecondDouble(QVariantList data) {
    QVariantList ret;
    bool invert = false;
    foreach(QVariant v, data) {
        if (invert) {
            ret += v.toDouble() * -1;
            invert = false;
        } else {
            ret += v;
            invert = true;
        }
    }

    return ret;
}

QString toMeshQml(QJsonObject object, QString name, QString slot, QString skin) {
    QString ret;
    ret += "SpineMeshItem {\n";
    ret += "\tid: " + formatName(skin + " " + slot + " " + name) + "\n";
    if (object.contains("name")) {
        ret += "\tsource: imageFolderAndDash + \"" + object.value("name").toString() + ".png\"" + "\n";
    } else {
        ret += "\tsource: imageFolderAndDash + \"" + name + ".png\"" + "\n";
    }
    // temp hack
    if (skin == "default skin") {
        ret += "\tparent: " + formatName(name) + attachmentStr + "\n";
    }

    if (object.contains("x")) {
        ret += "\ttrans.x: " + QString::number(object.value("x").toDouble()) + "\n";
    }
    if (object.contains("y")) {
        // pay attention to inverse here * -1
        ret += "\ttrans.y: " + QString::number(object.value("y").toDouble() * -1)  + "\n";
    }
    if (object.contains("scaleX")) {
        ret += "\tscale.xScale: " + QString::number(object.value("scaleX").toDouble()) + "\n";
    }
    if (object.contains("scaleY")) {
        ret += "\tscale.yScale: " + QString::number(object.value("scaleY").toDouble()) + "\n";
    }
    if (object.contains("rotation")) {
        // pay attention to inverse here * -1
        ret += "\trot.angle: " + QString::number(object.value("rotation").toDouble() * -1) + "\n";
    }
    if (object.contains("width")) {
        ret += "\twidth: " + QString::number(object.value("width").toDouble()) + "\n";
    }
    if (object.contains("height")) {
        ret += "\theight: " + QString::number(object.value("height").toDouble()) + "\n";
    }

    if (object.contains("uvs")) {
        ret += "\tuvs: [ " + fromArrayToString(object.value("uvs").toArray().toVariantList()) + " ] \n";
    }
    if (object.contains("triangles")) {
        ret += "\ttriangles: [ " + fromArrayToString(object.value("triangles").toArray().toVariantList()) + " ] \n";
    }
    if (object.contains("vertices")) {
        ret += "\tvertices: [ " + fromArrayToString(object.value("vertices").toArray().toVariantList()) + " ] \n";
    }
    if (object.contains("edges")) {
        ret += "\tedges: [ " + fromArrayToString(object.value("edges").toArray().toVariantList()) + " ] \n";
    }


    ret += "}\n";

    return ret;
}

QString toSkinnedMeshQml(QJsonObject object, QString name, QString slot, QString skin, QJsonObject root) {
    QString ret;
    ret += "SpineSkinnedMeshItem {\n";
    ret += "\tid: " + formatName(skin + " " + slot + " " + name) + "\n";
    if (object.contains("name")) {
        ret += "\tsource: imageFolderAndDash + \"" + object.value("name").toString() + ".png\"" + "\n";
    } else {
        ret += "\tsource: imageFolderAndDash + \"" + name + ".png\"" + "\n";
    }
    // todo note temp hack
    if (skin == "default skin") {
        ret += "\tparent: " + formatName(name) + attachmentStr + "\n";
    }

    if (object.contains("x")) {
        ret += "\ttrans.x: " + QString::number(object.value("x").toDouble()) + "\n";
    }
    if (object.contains("y")) {
        // pay attention to inverse here * -1
        ret += "\ttrans.y: " + QString::number(object.value("y").toDouble() * -1)  + "\n";
    }
    if (object.contains("scaleX")) {
        ret += "\tscale.xScale: " + QString::number(object.value("scaleX").toDouble()) + "\n";
    }
    if (object.contains("scaleY")) {
        ret += "\tscale.yScale: " + QString::number(object.value("scaleY").toDouble()) + "\n";
    }
    if (object.contains("rotation")) {
        // pay attention to inverse here * -1
        ret += "\trot.angle: " + QString::number(object.value("rotation").toDouble() * -1) + "\n";
    }
    if (object.contains("width")) {
        ret += "\twidth: " + QString::number(object.value("width").toDouble()) + "\n";
    }
    if (object.contains("height")) {
        ret += "\theight: " + QString::number(object.value("height").toDouble()) + "\n";
    }

    if (object.contains("uvs")) {
        ret += "\tuvs: [ " + fromArrayToString(object.value("uvs").toArray().toVariantList()) + " ] \n";
    }
    if (object.contains("triangles")) {
        ret += "\ttriangles: [ " + fromArrayToString(object.value("triangles").toArray().toVariantList()) + " ] \n";
    }
    if (object.contains("vertices")) {
        QVariantList vertices = object.value("vertices").toArray().toVariantList();
        QVariantList boneCounts;
        QStringList bones;
        QVariantList weights;
        QVariantList boneVertices;
        for (int i = 0; i < vertices.count();) {
            int bonesCount = vertices[i++].toInt();
            boneCounts.append(QVariant(bonesCount));
            for(int bi = 0; bi < bonesCount; bi++) {
                int boneIndex = vertices[i++].toInt();
                bones.append(formatName(root.value("bones").toArray().at(boneIndex).toObject().value("name")) + boneStr);
                boneVertices.append(vertices[i++]);
                boneVertices.append(vertices[i++]);
                weights.append(vertices[i++]);
            }
        }

        ret += "\tboneCounts: [ " + fromArrayToString(boneCounts) + " ] \n";
        ret += "\tbones: [ ";
        foreach(QString s, bones) {
            ret += s + ", ";
        }
        ret.remove(ret.length() - 2, 2);
        ret += " ]\n";
        ret += "\tboneVertices: [ " + fromArrayToString(invertEverySecondDouble(boneVertices)) + " ] \n";
        ret += "\tweights: [ " + fromArrayToString(weights) + " ] \n";
    }
    if (object.contains("edges")) {
        ret += "\tedges: [ " + fromArrayToString(object.value("edges").toArray().toVariantList()) + " ] \n";
    }


    ret += "}\n";

    return ret;
}

QString toBoundingBoxQml(QJsonObject object, QString name, QString slot, QString skin, QJsonObject root) {
    QString ret;
    ret += "Item {\n";
    ret += "\tid: " + formatName(skin + " " + slot + " " + name) + "\n";
    // todo note temp hack
    if (skin == "default skin") {
        ret += "\tparent: " + formatName(name) + attachmentStr + "\n";
    }

    if (object.contains("vertices")) {
        QVariantList vertices = object.value("vertices").toArray().toVariantList();
        ret += "\tvertices: [ " + fromArrayToPointString(invertEverySecondDouble(vertices)) + " ] \n";
    }
    ret += "}\n";

    return ret;
}

QString toImageQml(QJsonObject object, QString name, QString slot, QString skin, QJsonObject root, QMap<QString, QString>* imageAliases) {
    if (object.contains("type") && object.value("type").toString() == "mesh") {
        return toMeshQml(object, name, slot, skin);
    } else if (object.contains("type") && object.value("type").toString() == "skinnedmesh") {
        return toSkinnedMeshQml(object, name, slot, skin, root);
    } else if (object.contains("type") && object.value("type").toString() == "boundingbox") {
        return toBoundingBoxQml(object, name, slot, skin, root);
    }
    QString ret;
    ret += "SpineImage {\n";
    ret += "\tid: " + formatName(skin + " " + slot + " " + name) + "\n";
    imageAliases->operator []("a" + formatName(skin + " " + name)) = formatName(skin + " " + slot + " " + name);
    if (object.contains("name")) {
        ret += "\tsource: imageFolderAndDash + \"" + object.value("name").toString() + ".png\"" + "\n";
    } else {
        ret += "\tsource: imageFolderAndDash + \"" + name + ".png\"" + "\n";
    }
    // temp hack
    if (skin == "default skin") {
        ret += "\tparent: " + formatName(name) + attachmentStr + "\n";
    }

    if (object.contains("x")) {
        ret += "\ttrans.x: " + QString::number(object.value("x").toDouble()) + "\n";
    }
    if (object.contains("y")) {
        // pay attention to inverse here * -1
        ret += "\ttrans.y: " + QString::number(object.value("y").toDouble() * -1)  + "\n";
    }
    if (object.contains("scaleX")) {
        ret += "\tscale.xScale: " + QString::number(object.value("scaleX").toDouble()) + "\n";
    }
    if (object.contains("scaleY")) {
        ret += "\tscale.yScale: " + QString::number(object.value("scaleY").toDouble()) + "\n";
    }
    if (object.contains("rotation")) {
        // pay attention to inverse here * -1
        ret += "\trot.angle: " + QString::number(object.value("rotation").toDouble() * -1) + "\n";
    }
    if (object.contains("width")) {
        ret += "\twidth: " + QString::number(object.value("width").toDouble()) + "\n";
    }
    if (object.contains("height")) {
        ret += "\theight: " + QString::number(object.value("height").toDouble()) + "\n";
    }

    ret += "}\n";

    return ret;
}

QString skinsToQml(QJsonObject object, QJsonObject root, QMap<QString, QString>* imageAliases) {
    QString ret;
    ret += "property Item skins: Item {\n" \
           "visible: false\n\n";

    if (object.keys().count() > 0) {
        ret += "states: [\n";
        bool first = true;
        foreach(QString skin, object.keys()) {
            if (!first) {
                ret.insert(ret.length()-1,",");
            }
            first = false;
            QJsonObject skinObject = object.value(skin).toObject();

            ret += toStateQml(skin, skinObject);
        }
        ret += "]\n";
    }
    foreach(QString skin, object.keys()) {
        QJsonObject skinObject = object.value(skin).toObject();
        if (skin == "default") {
            skin = "default skin";
        }
        ret += "Item {\n" \
               "\tid: " + formatName(skin) + "\n\n";
        foreach(QString slot, skinObject.keys()) {
            QJsonObject slotObject = skinObject.value(slot).toObject();
            foreach(QString image, slotObject.keys()) {
                ret += indent(toImageQml(slotObject.value(image).toObject(), image, slot, skin, root, imageAliases));
            }
        }
        ret += "}\n";
    }
    ret += "}\n";

    return ret;
}

QString toPropertyActionQml(QString target, QString property, double value)
{
    QString ret;
    ret += "PropertyAction { target: " + target + "; property: \"" + property + "\"; value: " + QString::number(value) + " }\n";
    return ret;
}

QString toPropertyActionQml(QString target, QString property, QString value)
{
    QString ret;
    ret += "PropertyAction { target: " + target + "; property: \"" + property + "\"; value: " + value + " }\n";
    return ret;
}

QString toPropertyActionQml(QString target, QString property, QVariantList value)
{
    QString ret;
    ret += "PropertyAction { target: " + target + "; property: \"" + property + "\"; value: [ " + fromArrayToString(value) + " ] }\n";
    return ret;
}

QString toPropertyAnimationQml(QString target, QString property, double from, double to, double duration, QJsonValue curve)
{
    QString ret;
    int intDuration = (int) (duration * 1000.0);

    if (curve.isString() && curve.toString() == "stepped") {
        ret += "PauseAnimation { duration: " + QString::number(intDuration) + " }\n";
        ret += toPropertyActionQml(target, property, to);
    } else {
        ret += "PropertyAnimation { target: " + target + "; property: \"" + property +
                "\"; from: " + QString::number(from) + "; to: " + QString::number(to) +
                "; duration: " + QString::number(intDuration);
        if (curve.isArray()) {
            QJsonArray curveArray = curve.toArray();
            ret += "; easing.bezierCurve: [ ";
            foreach(QJsonValue item, curveArray) {
                ret += QString::number(item.toDouble()) + ", ";
            }
            ret += "1,1 ]";
        }

        ret += " }\n";
    }
    return ret;
}

QString toPropertyAnimationQml(QString target, QString property, QVariantList from, QVariantList to, double duration, QJsonValue curve)
{
    QString ret;
    int intDuration = (int) (duration * 1000.0);

    if (curve.isString() && curve.toString() == "stepped") {
        ret += "PauseAnimation { duration: " + QString::number(intDuration) + " }\n";
        ret += toPropertyActionQml(target, property, to);
    } else {
        ret += "PropertyAnimation { target: " + target + "; property: \"" + property +
                "\"; from: [ " + fromArrayToString(from) + " ] ; to: [ " + fromArrayToString(to) +
                " ] ; duration: " + QString::number(intDuration);
        if (curve.isArray()) {
            QJsonArray curveArray = curve.toArray();
            ret += "; easing.bezierCurve: [ ";
            foreach(QJsonValue item, curveArray) {
                ret += QString::number(item.toDouble()) + ", ";
            }
            ret += "1,1 ]";
        }

        ret += " }\n";
    }
    return ret;
}

QVariantList add(QVariantList left, QVariantList right, int offset)
{
    QVariantList ret;
    int right_i = 0;
    for(int i = 0; i < left.count(); i++) {
        if (i >= offset) {
            if (right_i < right.count()) {
                ret += QVariant(left[i].toDouble() + right[right_i].toDouble());
                right_i++;
            } else {
                ret += QVariant(left[i].toDouble());
            }
        } else {
            ret += QVariant(left[i].toDouble());
        }

    }

    return ret;
}


QString toColorAnimationQml(QString target, QString from, QString to, double duration, QJsonValue curve)
{
    QString ret;
    int intDuration = (int) (duration * 1000.0);

    from = toARGBColor(from);
    to = toARGBColor(to);

    if (curve.isString() && curve.toString() == "stepped") {
        ret += "PauseAnimation { duration: " + QString::number(intDuration) + " }\n";
        ret += toPropertyActionQml(target, "color", to);
    } else {
        ret += "ColorAnimation { target: " + formatName(target) + slotStr + "; property: \"color\"" + "; from: \"#" + from + "\"; to: \"#" + to + "\"; duration: " +
                QString::number(intDuration);

        if (curve.isArray()) {
            QJsonArray curveArray = curve.toArray();
            ret += "; easing.bezierCurve: [ ";
            foreach(QJsonValue item, curveArray) {
                ret += QString::number(item.toDouble()) + ", ";
            }
            ret += "1,1 ]";
        }
        ret += " }\n";
    }

    return ret;
}

QString toAnimationQml(QJsonObject animations, QString animationStr, QJsonArray bones, QJsonArray slotArray, QJsonObject root)
{
    QString ret;
    QJsonObject animation = animations.value(animationStr).toObject();

    ret += "ParallelAnimation {\n";
    ret += "\tid: " + formatName(animationStr) + "\n";

    QJsonObject slotAnimation = animation.value("slots").toObject();
    foreach(QString slot, slotAnimation.keys()) {
        QJsonObject slotObject = slotAnimation.value(slot).toObject();
        QJsonArray attachmentChanges = slotObject.value("attachment").toArray();
        QJsonArray colorChanges = slotObject.value("color").toArray();

        if (attachmentChanges.count() > 0) {
            double previousTime = 0;
            ret += "\tSequentialAnimation {\n";
            foreach(QJsonValue item, attachmentChanges) {
                QJsonObject itemObject = item.toObject();
                double time = itemObject.value("time").toDouble();
                QJsonValue attachmentName = itemObject.value("name");
                QString attachment = "null";

                if (time > 0) {
                    ret += "PauseAnimation { duration: " + QString::number((int)((time-previousTime)*1000)) + " }\n";
                }

                if (!attachmentName.isNull()) {
                    attachment = formatName(attachmentName.toString()) + attachmentStr ;
                }

                ret += "ScriptAction { script: "+ formatName(slot) + slotStr + ".attach(" +
                        attachment +  "); }\n";

                previousTime = time;
            }
            ret += "}\n";
        }

        if (colorChanges.count() > 0) {
            double previousTime = 0;
            QJsonValue curve, previousCurve; // in spine they announce the curve in starting keyframe. In qml its different.
            ret += "\tSequentialAnimation {\n";
            QString setupColor = "ffffffff";
            QString previousColor = setupColor;
            foreach(QJsonValue item, colorChanges) {
                QJsonObject itemObject = item.toObject();
                double time = itemObject.value("time").toDouble();
                QString color = itemObject.value("color").toString();
                foreach(QJsonValue value, slotArray) {
                    QJsonObject object = value.toObject();
                    if (object.value("name").toString() == slot) {
                        if (object.contains("color"))
                            setupColor = object.value("color").toString();
                        break;
                    }
                }
                curve = itemObject.value("curve");

                if (time == 0.0) {
                    ret += toPropertyActionQml(formatName(slot) + slotStr, "color", "\"#" + toARGBColor(color) + "\"");
                    previousColor = color;
                    previousCurve = curve;
                    continue;
                }

                ret += toColorAnimationQml(slot, previousColor, color, time - previousTime, previousCurve);

                previousCurve = curve;
                previousColor = color;
                previousTime = time;
            }
            ret += "}\n";
        }

    }
    QJsonObject boneAnimation = animation.value("bones").toObject();
    foreach(QString bone, boneAnimation.keys()) {
        // this is retarded, I need coffee or sleep, I will not remember this
        QJsonObject boneBoneObject;
        foreach(QJsonValue item, bones) {
            if (item.toObject().value("name").toString() == bone) {
                boneBoneObject = item.toObject();
                break;
            }
        }

        QJsonObject boneObject = boneAnimation.value(bone).toObject();
        QJsonValue rotateValue = boneObject.value("rotate");
        QJsonArray rotateArray = rotateValue.toArray();
        QJsonValue scaleValue = boneObject.value("scale");
        QJsonArray scaleArray = scaleValue.toArray();
        QJsonValue translateValue = boneObject.value("translate");
        QJsonArray translateArray = translateValue.toArray();


        if (!rotateValue.isUndefined()) {
            ret += "\tSequentialAnimation {\n";
            double previousTime = 0;
            QJsonValue curve, previousCurve; // in spine they announce the curve in starting keyframe. In qml its different.
            double previousAngle = 0.0;
            foreach(QJsonValue item, rotateArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                // note the inverse
                double angle = object.value("angle").toDouble() * -1;
                if (angle < -180)
                    angle += 360;
                if (angle > 180)
                    angle -= 360;
                curve = object.value("curve");

                if (angle + previousAngle < angle - previousAngle) {

                }

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr, "dangle", angle);
                    previousAngle = angle;
                    previousCurve = curve;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr, "dangle", previousAngle,
                                              angle, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousAngle = angle;
            }
            ret += "\t}\n";
        }

        if (!scaleValue.isUndefined()) {
            double previousTime = 0;
            QJsonValue curve, previousCurve; // in spine they announce the curve in starting keyframe. In qml its different.
            ret += "\tSequentialAnimation {\n";
            double previousScaleX = 1.0;
            foreach(QJsonValue item, scaleArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double scaleX = object.value("x").toDouble();
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr, "dxScale", scaleX);
                    previousScaleX = scaleX;
                    previousCurve = curve;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr, "dxScale", previousScaleX,
                                              scaleX, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousScaleX = scaleX;
            }
            ret += "\t}\n";

            previousTime = 0;
            curve = QJsonValue();
            previousCurve = QJsonValue(); // in spine they announce the curve in starting keyframe. In qml its different.
            ret += "\tSequentialAnimation {\n";
            double previousScaleY = 1.0;
            foreach(QJsonValue item, scaleArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double scaleY = object.value("y").toDouble();
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr, "dyScale", scaleY);
                    previousScaleY = scaleY;
                    previousCurve = curve;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr, "dyScale", previousScaleY,
                                              scaleY, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousScaleY = scaleY;
            }
            ret += "\t}\n";
        }


        if (!translateValue.isUndefined()) {
            double previousTime = 0;
            QJsonValue curve, previousCurve; // in spine they announce the curve in starting keyframe. In qml its different.
            ret += "\tSequentialAnimation {\n";
            double previousX = 0.0;
            foreach(QJsonValue item, translateArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double x = object.value("x").toDouble();
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr, "dx", x);
                    previousX = x;
                    previousCurve = curve;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr, "dx", previousX,
                                              x, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousX = x;
            }
            ret += "\t}\n";

            previousTime = 0;
            curve = QJsonValue();
            previousCurve = QJsonValue(); // in spine they announce the curve in starting keyframe. In qml its different.

            ret += "\tSequentialAnimation {\n";
            double previousY = 0.0;
            foreach(QJsonValue item, translateArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                // note the inverse
                double y = object.value("y").toDouble() * -1;
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr, "dy", y);
                    previousY = y;
                    previousCurve = curve;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr, "dy", previousY,
                                              y, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousY = y;
            }
            ret += "\t}\n";
        }
    }

    if (animation.contains("ffd")) {
        qDebug() << "one ffd";
        QJsonObject ffdAnimation = animation.value("ffd").toObject();

        // todo note, there are others so this is not enuff!
//        QJsonObject defaultSkinObject = ffdAnimation.value("default").toObject();

        foreach(QString skin, ffdAnimation.keys()) {
            QJsonObject skinObject = ffdAnimation.value(skin).toObject();
            foreach(QString slot, skinObject.keys()) {
                qDebug() << "one slot";
                QJsonObject slotObject = skinObject.value(slot).toObject();

                // todo note, there maybe more doctor who knows?
                QString attachment = slotObject.keys().first();
                QJsonArray animationArray = slotObject.value(attachment).toArray();

                //            QJsonObject boneObject = boneAnimation.value(bone).toObject();
                //            QJsonValue rotateValue = boneObject.value("rotate");
                //            QJsonArray rotateArray = rotateValue.toArray();
                //            QJsonValue scaleValue = boneObject.value("scale");
                //            QJsonArray scaleArray = scaleValue.toArray();
                //            QJsonValue translateValue = boneObject.value("translate");
                //            QJsonArray translateArray = translateValue.toArray();

                QJsonValue curve, previousCurve; // in spine they announce the curve in starting keyframe. In qml its different.

                double previousTime = 0;
                ret += "\tSequentialAnimation {\n";
                QVariantList setupVertices = root.value("skins").toObject().value(skin).toObject().value(slot).toObject().value(attachment).toObject().value("vertices").toArray().toVariantList();
                QVariantList previousVertices = setupVertices;
                foreach(QJsonValue item, animationArray) {
                    QJsonObject object = item.toObject();
                    double time = object.value("time").toDouble();
                    QVariantList vertices = swapXY(object.value("vertices").toArray().toVariantList());
                    int offset = object.contains("offset") ? object.value("offset").toInt() : 0;
                    curve = object.value("curve");



                    if(time == 0.0) {
                        ret += toPropertyActionQml(formatName(skin + " " + slot + " " + attachment), "vertices", add(setupVertices, vertices, offset));
                        previousVertices = add(previousVertices, vertices, offset);
                        previousCurve = curve;
                        continue;
                    }
                    ret += toPropertyAnimationQml(formatName(skin + " " + slot + " " + attachment), "vertices", previousVertices,
                                                  add(setupVertices, vertices, offset), time - previousTime, previousCurve);
                    previousCurve = curve;
                    previousTime = time;
                    previousVertices = add(setupVertices, vertices, offset);
                }
                ret += "\t}\n";
            }
        }
    }

    if (animation.contains("events")) {
        QJsonArray eventsArray = animation.value("events").toArray();

        double previousTime = 0;

        ret += "\tSequentialAnimation {\n";

        foreach(QJsonValue event, eventsArray) {
            QJsonObject eventObject = event.toObject();

            double time = eventObject.value("time").toDouble();
            QString name = eventObject.value("name").toString();

            if (time - previousTime > 0) {
                int duration = (time - previousTime) * 1000;
                ret += "PauseAnimation { duration: " + QString::number(duration) + " }\n";
            }

            QString params = "(";

            if (eventObject.contains("float")) {
                params += QString::number(eventObject.value("float").toDouble());
            }

            if (eventObject.contains("int")) {
                if (eventObject.contains("float")) {
                    params += ", ";
                }
                params += QString::number(eventObject.value("int").toInt());
            }

            if (eventObject.contains("string")) {
                if (eventObject.contains("float") || eventObject.contains("int")) {
                    params += ", ";
                }
                params += "\"" + eventObject.value("string").toString() + "\"";
            }

            params += ")";

            ret += "ScriptAction { script: " + name + params + "; }\n";
        }

        ret += "\t}\n";

    }


    ret += "}\n";

    return ret;
}

QString toIKJointQml(QJsonObject joint) {
    QString ret;

    ret += "IKJoint {\n";

    ret += "\tid: " + formatName(joint.value("name").toString() + jointStr) + "\n";
    ret += "\ttarget: " + formatName(joint.value("target").toString() + boneStr) + "\n";

    QStringList bones;
    QJsonArray bonesArray = joint.value("bones").toArray();

    foreach(const QJsonValue& v, bonesArray) {
        bones.append(formatName(v.toString()) + boneStr);
    }

    ret += "\tfirst: " + bones.at(0) + "\n";
    if (bones.length() == 2)
        ret += "\tsecond: " + bones.at(1) + "\n";

    if (joint.contains("bendPositive")) {
        ret += "\tbendPositive: " + QString((joint.value("bendPositive").toBool() ? "true" : "false")) + "\n";
    }
    if (joint.contains("mix")) {
        ret += "\tmix: " + QString::number(joint.value("mix").toDouble()) + "\n";
    }
    ret += "}\n";

    return ret;
}

QString toPropertyChangeQml(QString target, QString property, QString value) {
    return QString("PropertyChanges { target: " + target + "; " + property + ": " + value + " }\n");
}

QMap<QString, QString> findAttachmentParentPairs(QJsonObject skeleton) {
    QMap<QString, QString> ret;

    QJsonObject skinsObject = skeleton.value("skins").toObject();
    foreach(QString skin, skinsObject.keys()) {
        QJsonObject skinObject = skinsObject.value(skin).toObject();
        foreach(QString slot, skinObject.keys()) {
            QJsonObject slotObject= skinObject.value(slot).toObject();
            foreach(QString attachment, slotObject.keys()) {
                ret[attachment] = QString();
            }
        }
    }

    QJsonArray slotsArray = skeleton.value("slots").toArray();

    foreach(QJsonValue item, slotsArray) {
        QJsonObject itemObject = item.toObject();
        if (!itemObject.contains("attachment"))
            continue;


        QString attachment = itemObject.value("attachment").toString();
        QString slot = itemObject.value("name").toString();
        ret[attachment] = slot;
    }

    return ret;
}

QString parseAttachmentToQml(QString attachment, QString slot) {
    QString ret;
    bool hasParent = slot != QString();
    ret += "Attachment { id: " + formatName(attachment) + attachmentStr +
            (hasParent ? "; setupParent: " + formatName(slot) + slotStr : "") + " }\n";

    return ret;
}

QString toAttachmentsQml(QJsonObject skeleton) {
    QString ret;

    QMap<QString, QString> attachments = findAttachmentParentPairs(skeleton);

    ret += "Item {\n";
    ret += "\tid: attachments\n";
    ret += "\tvisible: false\n\n";

    foreach(QString key, attachments.keys()) {
        ret += parseAttachmentToQml(key, attachments.value(key));
    }

    ret += "}\n";

    return ret;
}


QString toStateQml(QString skin, QJsonObject skinObject) {
    QString ret;
    ret += "State {\n";
    ret += "\tname: \"" + skin.toUpper() + "\"\n";

    foreach(QString slot, skinObject.keys()) {
        QJsonObject slotObject = skinObject.value(slot).toObject();
        foreach(QString attachment, slotObject.keys()) {
            QString target = formatName(formatName(skin + " " + slot + " " + attachment));
            ret += toPropertyChangeQml(target, "parent", formatName(attachment) + attachmentStr);
        }
    }

    ret += "}\n";

    return ret;
}

int main(int argc, char *argv[])
{
    if (argc < 1 || argc > 4) {
        QTextStream(stdout) << "Usage: Spine-convert input.json | Spine-convert input.json Output.qml \n";
        return 0;
    }


    QStringList arguments;
    for (int i = 1; i < argc; i++) {
        arguments += QString(QByteArray(argv[i]));
    }

    foreach(QString s, arguments) {
        qDebug() << Q_FUNC_INFO << s;
    }

    //    QFile log(arguments[2] +        "log.txt");
    //    log.open(QIODevice::WriteOnly);
    //    QTextStream logStream(&log);
    //    foreach(QString s, arguments) {
    //        qDebug() << Q_FUNC_INFO << s;
    //        logStream << s << "\n";
    //    }
    //    log.close();

    QString output;
    QString outputFilename;
    QString imageFolder;
    QFile outputFile;
    qDebug() << Q_FUNC_INFO << arguments;

    if (arguments[0].compare("auto") != 0) {
        imageFolder = arguments[0].left(arguments[0].lastIndexOf('.')) + "_images"; // from input.json -> input

        if (arguments.count() == 2) {
            outputFilename = arguments[1];
        } else {
            qDebug() << Q_FUNC_INFO << "huh";
            outputFilename = arguments[0].left(arguments[0].lastIndexOf('.')) + ".qml";
            outputFilename.data()[0] = outputFilename.data()[0].toUpper();
        }

        qDebug() << outputFilename;
        outputFile.setFileName(outputFilename);

        if(!outputFile.open(QIODevice::WriteOnly)) {
            return 1;
        }
    } else { // a special case for directory monitors, start them with "auto" argument and then
        // filename and dir as second and third argument
        if (arguments.count() == 2) {
            arguments.append("");
        }
        imageFolder = arguments[1].left(arguments[1].lastIndexOf('.')) + "_images"; // from input.json -> input

        outputFilename = arguments[1].left(arguments[1].lastIndexOf('.')) + ".qml";
        outputFilename.data()[0] = outputFilename.data()[0].toUpper();

        outputFile.setFileName(arguments[2] + "\\" +outputFilename);

        if(!outputFile.open(QIODevice::WriteOnly)) {
            return 1;
        }
    }

    QJsonDocument document;

    if (arguments[0].compare("auto") == 0) {
        readDocumentFromFile(arguments[2] + "\\" + arguments[1], document);
    }
    else if (!readDocumentFromFile(arguments[0], document)) {
        return 1;
    }

    QJsonObject root = document.object();

    QJsonArray bones = root.value("bones").toArray();

    foreach(const QJsonValue& value, bones) {
        QJsonObject item = value.toObject();

        output += toBoneQml(item);
    }

    QJsonArray ikArray =  root.value("ik").toArray();
    foreach(const QJsonValue& value, ikArray) {
        QJsonObject item = value.toObject();

        output += toIKJointQml(item);
    }

    QJsonArray slotArray = root.value("slots").toArray();

    foreach(const QJsonValue& value, slotArray) {
        QJsonObject item = value.toObject();

        int level = findSlotLevel(item, bones);
        output += toSlotQml(item, level);
    }

    output += toAttachmentsQml(root);

    QJsonObject skins = root.value("skins").toObject();
    QMap<QString, QString> imageAliases;
    output += skinsToQml(skins, root, &imageAliases);

    foreach(QString k, imageAliases.keys()) {
        output += "property alias " + k + ": " + imageAliases.value(k) + "\n";
    }

    QJsonObject animations = root.value("animations").toObject();

    foreach(QString animation, animations.keys()) {
        output += "property alias " + formatName(animation) + animationStr + ": " + formatName(animation) + "\n";
        output += "Item {\n";
        output += toAnimationQml(animations, animation, bones, slotArray, root);
        output += "}\n";
    }

    wrapQml(output, imageFolder);

    QTextStream out(&outputFile);
    out << output;

    outputFile.close();

    QTextStream(stdout) << "The deed is done.\n";

    return 0;
}

