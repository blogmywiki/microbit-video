# microbit-video
C++ programs for generating video from a V1 micro:bit.

This is a project created by Kevin Moonlight. You can view his video here https://www.youtube.com/watch?v=uh61FeqduBk and read his code here: https://os.mbed.com/users/yyzkevin/code/microbit-hello-world/

What I've done is add a filled box and compile it into a HEX file so you can easily try it out yourself.

![screenshot](https://raw.githubusercontent.com/blogmywiki/microbit-video/main/images/screenshot.jpg)


## What it does
It generated a demo of some boxes and text as a black and white NTSC video image you can view on a TV.

## What you need to try it out
- A V1 micro:bit
- A 680 ohm resistor
- Some crocodile clip leads
- A phono / RCA plug with bare wires
- A TV that can show a composite NTSC video signal. A cheap TV I bought in the UK was fine.

![wiring photo](https://raw.githubusercontent.com/blogmywiki/microbit-video/main/images/wiring-photo.jpg)

## How to build it
Flash the HEX file to a V1 micro:bit.
Connect GND pin on the micro:bit to the shield pin on the phono/RCA video plug.
Connect pin 1 on the micro:bit to the centre pin on the video plug.
Connect pin 0 to a 680 ohm resistor, and then to the centre pin.
Connect the plug to the composite video input of a TV and select the relevant AV input. You should see 
two boxes, one empty, one filled, some text and a counter.

![wiring diagram](https://raw.githubusercontent.com/blogmywiki/microbit-video/main/images/wiring-diagram-white.png)

## How to modify and recompile the code
Not that easy. What I did was go to https://ide.mbed.com/, log in, I dragged Kevin's original 3 files into a new project:
- main.cpp
- font.h
- microbit.lib

I tinkered with the code in the Mbed IDE, clicked compile to get a HEX file I could drag and drop on to a V1 micro:bit's MICROBIT drive.

## Can I write a program to do this in Python or MakeCode?
Short answer: no.
Slightly longer answer: not easily, because generating video signals requires very precise timings which you can't access in MakeCode. 
In Python you'd need to use machine code I think. Maybe someone could make this into an extension? That would be cool.

## What use is this?
Not a huge amount on its own, but I think getting video from a micro:bit is pretty cool. 
You can read about how to turn a V1 micro:bit and a PS/2 keyboard into a programmable computer here: http://www.suppertime.co.uk/blogmywiki/2021/05/video-from-a-microbit/

## Next steps
- Video thermometer
- Screen that prints any radio messages it receives
- Version for V2 micro:bit
- Custom font if I can design or find a better 5x7 font
