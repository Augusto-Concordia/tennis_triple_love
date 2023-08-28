![1693239220_tennis_triple_love](https://github.com/Augusto-Concordia/tennis_triple_love/assets/33916325/4f805789-f6f2-49a0-ad27-92ab4023ffb6)


# tennis_triple_love
Assignment #2 - COMP 371

## Authors

- Gabrielle Guidote - 40175182
- Jack Spiratos - 40210312
- Augusto Mota Pinheiro - 40208080

## Features
* Many keybinds to control the camera and the model
* Animated and fun-shaded tennis racket with ball
* Lit model using the simple Phong Lighting model

## Getting Started
### From a zipped folder (TAs ⚠️)
All depended upon libraries should already be included in the vendor folder.

1. Generate project files with CMake
2. Set the working directory to the root of the project
3. Run the `tennis_triple_love` project!

### From the git repository
Similar to the above instructions, with the key difference being to clone the repository with `--recurse-submodules` to include the depended upon libraries.

1. Generate project files with CMake
2. Set the working directory to the root of the project
3. Run the `tennis_triple_love` project!

## Keybinds
* `Home` & `Keypad 5`: Resets the camera's position & rotation
* `Tab`: Resets the current model's position & rotation

<br/>

* `U`: Scale current model up
* `J`: Scale current model down

<br/>

_The following controls' direction were decided relative to the default camera's position, because it was assumed that the user wouldn't orbit the camera much._
* `W`: Move model forward (along the X axis)
* `A`: Move model left (along the Y axis)
* `S`: Move model backward (along the X axis)
* `D`: Move model right (along the Y axis)

<br/>

* `q`: Rotate model forward (around the X axis)
* `e`: Rotate model backward (around the X axis)
* `w`: Rotate model left (around the Y axis)
* `s`: Rotate model right (around the Y axis)
* `a`: Rotate model clockwise (around the Z axis)
* `d`: Rotate model counter-clockwise (around the Z axis)

<br/>

* `Up Arrow + Shift`: Rotate forward current model's upper arm 
* `Down Arrow + Shift`: Rotate backward current model's upper arm
* `Right Arrow + Shift`: Rotate outward current model's upper arm
* `Left Arrow + Shift`: Rotate inward current model's upper arm

<br/>

_The following changes the world orientation (orbits the camera)._
* `Up Arrow`: Orbit up
* `Right Arrow`: Orbit right
* `Down Arrow`: Orbit down
* `Left Arrow`: Orbit left

<br/>

* `Keypad 7`: Move camera upward (along the Z direction)
* `Keypad 9`: Move camera downward
* `Keypad 8`: Move camera forward (along where it's looking)
* `Keypad 4`: Move camera left
* `Keypad 2`: Move camera backward (along where it's looking)
* `Keypad 6`: Move camera right

<br/>

* `Right Click` & `Horizontal Mouse`: Pan the camera (rotates it horizontally)
* `Middle Click` & `Vertical Mouse`: TIlt the camera (rotates it vertically)
* `Left Click` & `Mouse`: Moves the camera in and out of the scene (along where it's looking)

<br/>

* `P`: Renders the model as points
* `L`: Renders the model as lines
* `T`: Renders the model as filled triangles (i.e. polygons)

<br/>

* `X`: Toggles textures on/off
* `B`: Toggles shadow mapping on/off
* `Z`: Pauses light movement
