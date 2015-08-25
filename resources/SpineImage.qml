import QtQuick 2.0
import QtGraphicalEffects 1.0

Image {
    id: image
    property Rotation rot: Rotation {}
    property Translate trans: Translate {}
    property Scale scale: Scale { }
    property Translate center: Translate { x: -width/2; y: -height/2 }
    property color color: parent !== null ? parent.parent !== null ? parent.parent.color !== undefined ? parent.parent.color : "#ffffffff" : "#ffffffff" : "#ffffffff"

    opacity: color !== null ? color.a : 1.0

    transform: [ /*parent.parent.bone,*/ center, scale, rot, trans ]

    layer.effect: SpineColorShader {
//        parent: !Qt.colorEqual(image.color, "#ffffffff") ? image : null
//        anchors.fill: parent
//        source: image
        color: image.color
    }

    layer.enabled: !Qt.colorEqual(image.color, "#ffffffff")

//    onStatusChanged: console.log(source + " " + status)

    // todo note optimization check on android and all, this could be opt out maybe or not
//    antialiasing: true

}
