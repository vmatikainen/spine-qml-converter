import QtQuick 2.0
import QtGraphicalEffects 1.0
import Spine 1.0

Item {
    id: slot
    property BoneTransform bone: null

    property Item setupAttachment: null
    property Item attachment: null //setupAttachment

    property color setupColor: "#FFFFFFFF"
    property color color: null

    transform: bone

    function attach(item) {
        if (attachment === item)
            return;
        if (attachment !== null)
            attachment.parent = null
        if (item !== null)
            item.parent = slot
        this.attachment = item
    }

    Component.onCompleted: {
        reset();
    }

    function reset() {
        attach(setupAttachment);
        color = setupColor;
    }
}
