import QtQuick 2.0

QtObject {
    id: bone
    property QtObject bone: null

    property Rotation setupRot: Rotation {}
    property Translate setupTrans: Translate {}
    property Scale setupScale: Scale {}

    property Rotation rot: Rotation {}
    property Translate trans: Translate {}
    property Scale scale: Scale {}

    Component.onCompleted: {
        reset();
    }

    function reset() {
        rot.angle = setupRot.angle;
        trans.x = setupTrans.x;
        trans.y = setupTrans.y;
        scale.xScale = setupScale.xScale;
        scale.yScale = setupScale.yScale;
    }
}
