import QtQuick 2.0

Item {
    property Item setupParent: null
    parent: null 

    Component.onCompleted: {
        reset();
    }

    function reset() {
        parent = setupParent;
    }
}
