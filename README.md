# microbit-video
C++ programs for generating video from a V1 micro:bit.

This is a project created by Kevin Moonlight. You can view a video here https://www.youtube.com/watch?v=uh61FeqduBk and read his code here: https://os.mbed.com/users/yyzkevin/code/microbit-hello-world/

What I've done is compile this into a HEX file so you can easily try it out yourself.

![screenshot](https://raw.githubusercontent.com/blogmywiki/microbit-video/main/images/screenshot.jpg)


## What it does
It generated a demo of some boxes and text as a black and white NTSC video image you can view on a TV.

## What you need to try it out
- A V1 micro:bit
- A 680 ohm resistor
- Some crocodile clip leads
- A phono / RCA plug with bare wires
- A TV that can show a composite NTSC video signal. A cheap TV I bought in the UK was fine.

## How to build it
Flash the hex file to a V1 micro:bit.
Connect GND pin on the micro:bit to the shield pin on the phono/RCA plug.
Connect pin 1 on the micro:bit to the centre pin on plug.
Connect pin 0 to a 680 ohm resistor, and then to the centre pin.
Connect the plug to the composite video input of a TV and select the relevant AV input. You should see 
two boxes, one empty, one filled, some text and a counter.


## How to modify and recompile the code
Urgh.


## Next steps
- Video thermometer
- Screen that prints any radio messages it receives
