spine-qml-converter
===================
Converts Spine by Esoteric Software export json format into native QML items and animations.

The idea is to convert animations and UI's from Spine to QML which in turn can be modified
by hand to integrate fluidly to your Qt/QML games and apps.

Usage
=========
spine-qml-converter input.json Output.qml

Then add the converted qml, the files in resources/ and any required images to your qml project.

Skeleton format
=========
The animations are exposed properties named as "walkAnimation", "shootAnimation" etc.
They can be controlled as native animations, but function animate(animation) is preferred, as
it resets the pose before starting the animation. 
If using skins, you must set skeleton.skin.state = "GOBLIN".
The root bone will be in the x and y position of the item.
reset function can be used to set the skeleton back to setup pose.

Notes
=========
Does not (yet) support IK or FFD. Texture atlases are not used.
Tested with Spine example json export, so might have a number of issues left. 

Version
=========
0.1 
 * Proof of concept implementation
 * The QML format and that data it exposes is in early format

Environment
=========
Has been tested in Windows 7 and Ubuntu 14.04 with Qt 5.1 and 5.3.

Known issues
=========
The intendation in the output is not correct (so ctrl+a + ctrl+i is your friend).