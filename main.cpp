/*
    spine-qml-converter
    Copyright (C) 2014  Vikke Matikainen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QDir>

using namespace std;

const QString boneStr = "Bone";
const QString slotStr = "Slot";
const QString attachmentStr = "Attachment";
const QString animationStr = "Animation";

QString recursiveBoneTransform(int level);
QString toStateQml(QString skin, QJsonObject skinObject);

bool readDocumentFromFile(const QString& fileName, QJsonDocument& document) {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray data = file.readAll();

    document = QJsonDocument::fromJson(data);

    return true;
}

QString indent(QString data, int amount = 1)
{
    QString tab;
    tab.fill('\t', amount);
    QString ret = data;
    if (ret.length() > 0) {
        int i = 0;
        do {
            ret.insert(i, tab);
            i = ret.indexOf("\n", i) + 1;
        } while (i > 0);
    }

    return ret;
}

bool wrapQml(QString& data)
{
    data = indent(data);
    data.prepend("import QtQuick 2.0\n" \
                 "\n" \
                 "Skeleton {\n" \
                 "\tid: root\n");

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
    ret += "Bone {\n";
    ret += "\tid: " + formatName(object.value("name")) + boneStr + "\n";
    if (object.contains("parent")) {
        ret += "\tbone: " + formatName(object.value("parent")) + boneStr + "\n";
    }
    if (object.contains("x")) {
        ret += "\tsetupTrans.x: " + QString::number(object.value("x").toDouble()) + "\n";
    }
    if (object.contains("y")) {
        // pay attention to inverse here * -1
        ret += "\tsetupTrans.y: " + QString::number(object.value("y").toDouble() * -1) + "\n";
    }
    if (object.contains("scaleX")) {
        ret += "\tsetupScale.xScale: " + QString::number(object.value("scaleX").toDouble()) + "\n";
    }
    if (object.contains("scaleY")) {
        ret += "\tsetupScale.yScale: " + QString::number(object.value("scaleY").toDouble()) + "\n";
    }
    if (object.contains("rotation")) {
        // pay attention to inverse here * -1
        ret += "\tsetupRot.angle: " + QString::number(object.value("rotation").toDouble() * -1) + "\n";
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

    ret += "\ttransform: [ " + recursiveBoneTransform(level)+ " ]\n";

    ret += "}\n";

    return ret;
}

QString toImageQml(QJsonObject object, QString name, QString slot, QString skin) {
    QString ret;
    ret += "SpineImage {\n";
    ret += "\tid: " + formatName(skin + " " + slot + " " + name) + "\n";
    if (object.contains("name")) {
        ret += "\tsource: \"" + object.value("name").toString() + ".png\"" + "\n";
    } else {
        ret += "\tsource: \"" + name + ".png\"" + "\n";
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

QString skinsToQml(QJsonObject object) {
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
                ret += indent(toImageQml(slotObject.value(image).toObject(), image, slot, skin));
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

QString toColorAnimationQml(QString target, QString from, QString to, double duration)
{
    QString ret;
    from = toARGBColor(from);
    to = toARGBColor(to);
    ret += "ColorAnimation { target: " + formatName(target) + slotStr + "; property: \"color\"" + "; from: \"#" + from + "\"; to: \"#" + to + "\"; duration: " +
            QString::number((int)(duration*1000)) + " }\n";

    return ret;
}

QString toAnimationQml(QJsonObject animations, QString animationStr, QJsonArray bones, QJsonArray slotArray)
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

                if (time == 0.0) {
                    ret += toPropertyActionQml(formatName(slot) + slotStr, "color", "\"#" + toARGBColor(color) + "\"");
                    previousColor = color;
                    continue;
                }

                ret += toColorAnimationQml(slot, previousColor, color, time - previousTime);

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

        QJsonValue curve, previousCurve; // in spine they announce the curve in starting keyframe. In qml its different.

        if (!rotateValue.isUndefined()) {
            ret += "\tSequentialAnimation {\n";
            double previousTime = 0;
            double setupAngle = boneBoneObject.value("rotation").toDouble() * -1;
            double previousAngle = setupAngle;
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

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr+".rot", "angle", setupAngle + angle);
                    previousAngle += angle;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr+".rot", "angle", previousAngle,
                                              setupAngle + angle, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousAngle = setupAngle + angle;
            }
            ret += "\t}\n";
        }

        if (!scaleValue.isUndefined()) {
            double previousTime = 0;
            ret += "\tSequentialAnimation {\n";
            double setupScaleX = 1.0;
            if (boneBoneObject.contains("scaleX"))
                setupScaleX = boneBoneObject.value("scaleX").toDouble();
            double previousScaleX = setupScaleX;
            foreach(QJsonValue item, scaleArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double scaleX = object.value("x").toDouble();
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr+".scale", "xScale", setupScaleX * scaleX);
                    previousScaleX = setupScaleX * scaleX;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr+".scale", "xScale", previousScaleX,
                                              setupScaleX * scaleX, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousScaleX = setupScaleX * scaleX;
            }
            ret += "\t}\n";

            previousTime = 0;
            ret += "\tSequentialAnimation {\n";
            double setupScaleY = 1.0;
            if (boneBoneObject.contains("scaleY"))
                setupScaleY = boneBoneObject.value("scaleY").toDouble();
            double previousScaleY = setupScaleY;
            foreach(QJsonValue item, scaleArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double scaleY = object.value("y").toDouble();
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr+".scale", "yScale", setupScaleY * scaleY);
                    previousScaleY = setupScaleY * scaleY;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr+".scale", "yScale", previousScaleY,
                                              setupScaleY * scaleY, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousScaleY = setupScaleY * scaleY;
            }
            ret += "\t}\n";
        }


        if (!translateValue.isUndefined()) {
            double previousTime = 0;
            ret += "\tSequentialAnimation {\n";
            double setupX = boneBoneObject.value("x").toDouble();
            double previousX = setupX;
            foreach(QJsonValue item, translateArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double x = object.value("x").toDouble();
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr+".trans", "x", setupX + x);
                    previousX += x;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr+".trans", "x", previousX,
                                              setupX + x, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousX = setupX + x;
            }
            ret += "\t}\n";

            previousTime = 0;
            ret += "\tSequentialAnimation {\n";
            double setupY = boneBoneObject.value("y").toDouble() * -1;
            double previousY = setupY;
            foreach(QJsonValue item, translateArray) {
                QJsonObject object = item.toObject();
                double time = object.value("time").toDouble();
                double y = object.value("y").toDouble() * -1;
                curve = object.value("curve");

                if(time == 0.0) {
                    ret += toPropertyActionQml(formatName(bone)+boneStr+".trans", "y", setupY + y);
                    previousY += y;
                    continue;
                }
                ret += toPropertyAnimationQml(formatName(bone)+boneStr+".trans", "y", previousY,
                                              setupY + y, time - previousTime, previousCurve);
                previousCurve = curve;
                previousTime = time;
                previousY = setupY + y;
            }
            ret += "\t}\n";
        }
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
    if (argc < 2 || argc > 3) {
        QTextStream(stdout) << "Usage: spine-qml-convert input.json Output.qml\n";
        return 0;
    }

    QStringList arguments;
    for (int i = 1; i < argc; i++) {
        arguments += QString(QByteArray(argv[i]));
    }

    QString output;
    QFile outputFile(arguments[1]);

    if(!outputFile.open(QIODevice::WriteOnly)) {
        return 1;
    }

    QJsonDocument document;

    if (!readDocumentFromFile(arguments[0], document)) {
        return 1;
    }

    QJsonObject object = document.object();

    QJsonArray bones = object.value("bones").toArray();

    foreach(const QJsonValue& value, bones) {
        QJsonObject item = value.toObject();

        output += toBoneQml(item);
    }

    QJsonArray slotArray = object.value("slots").toArray();

    foreach(const QJsonValue& value, slotArray) {
        QJsonObject item = value.toObject();

        int level = findSlotLevel(item, bones);
        output += toSlotQml(item, level);
    }

    output += toAttachmentsQml(object);

    QJsonObject skins = object.value("skins").toObject();
    output += skinsToQml(skins);

    QJsonObject animations = object.value("animations").toObject();

    foreach(QString animation, animations.keys()) {
        output += "property alias " + formatName(animation) + animationStr + ": " + formatName(animation) + "\n";
        output += "Item {\n";
        output += toAnimationQml(animations, animation, bones, slotArray);
        output += "}\n";
    }

    wrapQml(output);

    QTextStream out(&outputFile);
    out << output;

    outputFile.close();

    QTextStream(stdout) << "The deed is done.\n";

    return 0;
}

