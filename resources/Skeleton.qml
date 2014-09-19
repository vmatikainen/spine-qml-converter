import QtQuick 2.0

Item {
    property ParallelAnimation currentAnimation: null

    function animate(animation) {
        if (currentAnimation !== null) {
            currentAnimation.stop();
            reset();
        }
        animation.start();
        currentAnimation = animation;
    }

    function reset() {
        for (var i = 0; i < children.length; i++) {
            if (children[i].reset !== undefined) {
                children[i].reset();
            }
        }
        for (i = 0; i < resources.length; i++) {
            if (resources[i].reset !== undefined) {
                resources[i].reset();
            }
        }
    }
}
