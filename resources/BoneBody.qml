import QtQuick 2.2
import QtQuick.Controls 1.1
import Box2D 2.0
import Spine 1.0

import "shared"

Item {
    id: item

    property alias body: internalBody
    property alias bodyType: internalBody.bodyType
    property BoneTransform bone: null
    property bool boneDirty: false

    Rectangle {
        color: "green"
        width: 50
        height: 50
    }

    onXChanged: console.log("OMG")

    function updatePosition() {
        var point = bone.worldPosition()
        console.log("update: " + item.x + " " + item.y + " " + point);
        item.x = point.x
        item.y = point.y
//        console.log("positionChanged " + item.x + " " + item.y + " " + point)
        item.rotation = bone.worldRotation()
    }

    Connections {
        target: bone
        onTransformChanged: {
            if (bodyType === Body.Kinematic || boneDirty === true)  {
                console.log("jou " )
                updatePosition() ;
            }
        }
    }

    onBoneChanged: { boneDirty = true }


    Body {
        id: internalBody
        bodyType: Body.Kinematic
        world: physicsWorld
        target: item

        signal beginContact(Fixture other)
        signal endContact(Fixture other)

        onPositionChanged: {
            if (bodyType === Body.Dynamic) {
                boneDirty = false;
//                console.log("positionChanged " + item.x + " " + item.y)
                var world = bone.parentBone.worldPosition()
                bone.x = (item.x - world.x )*5
                bone.y = (item.y - world.y )*5
            }
        }

//        onRotationChanged {

//        }

        Chain {
            id: polygon

//            vertices: [
//                Qt.point(-10.5, 0),
//                Qt.point(0, 100),
//                Qt.point(110, 100),
//                Qt.point(100, 0)
//            ]
            vertices: [ Qt.point( 25.8469*0.2, -13.0201*0.2), Qt.point( -30.6559*0.2, 68.1495*0.2), Qt.point( 42.0997*0.2, 167.552*0.2), Qt.point( 76.0243*0.2, 215.5*0.2), Qt.point( 120.324*0.2, 242.7*0.2), Qt.point( 174.72*0.2, 247.47*0.2), Qt.point( 229.957*0.2, 226.163*0.2), Qt.point( 353.9*0.2, 36.0658*0.2), Qt.point( 451.565*0.2, -122.11*0.2)/*, Qt.point( 348.133*0.2, 30.2814*0.2), Qt.point( 221.839*0.2, 214.818*0.2), Qt.point( 167.392*0.2, 231.447*0.2), Qt.point( 126.863*0.2, 231.182*0.2), Qt.point( 83.01*0.2, 204.482*0.2), Qt.point( 36.0952*0.2, 136.728*0.2), Qt.point( -16.4216*0.2, 67.7476)*/  ]


            density: 1
            friction: 0.3
            restitution: 0

            onBeginContact: body.beginContact(other)
            onEndContact: body.endContact(other)
        }

//        Circle {
//            density: 1
//            friction: 0.3
//            restitution: 0

//            id: circle

//            radius: 10

//            onBeginContact: body.beginContact(other)
//            onEndContact: body.endContact(other)
//        }
    }
//    CircleBody {
//        target: item

//        id: body

//        radius: 10

//        density: 1
//        friction: 0.3
//        restitution: 0

//        bodyType: Body.Kinematic
//        world: physicsWorld
//    }
}
