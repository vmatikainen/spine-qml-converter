import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: slot
    property Bone bone: null

    property Item setupAttachment: null
    property Item attachment: null

    property color setupColor: "#FFFFFFFF"
    property color color: null

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
