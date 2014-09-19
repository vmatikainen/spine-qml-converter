import QtQuick 2.0

Item {
    property Item setupParent: null
    parent: null //setupParent

    Component.onCompleted: {
        reset();
    }

    function reset() {
        parent = setupParent;
    }
}
