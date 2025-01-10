# BezierAnimation

Project for Analiza Numeryczna at UWr. Simple program for animating with Bezier curves.

To build the project you need cmake and the g++ new enough to compile c++ 23.
(Also linux is recomended, didn't test it yet on windows:P)

---

# Additional info

## Keybindings

q - previous frame
e - next frame
up and down arrows - change animation playback speed
m - move state
g - add curve state
h - add point state     
n - normal state
s - save to file
a - play animation     
f - new frame

---

## How to run

To compile:

```> mkdir build```
```> cd build```
```> cmake ..```
```> make ```
 
--

To run: (This way, from terminal, is recomended. Another way may not work:P)
```> ./build/bin/BezierAnimation [optional: path to file you want to open]```

Running example animation:
```> ./build/bin/BezierAnimation pokaz.anim```
and press start playing button

sd
