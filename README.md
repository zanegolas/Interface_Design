# MPE LiDAR Midi Interface

[![Interface Final Teaser Thumbnail](https://user-images.githubusercontent.com/70541863/236627964-322ba4fd-9b0f-4fa4-8eb0-dfdfb2d365fa.png)](http://www.youtube.com/watch?feature=player_embedded&v=Cmb0zN6hiA8)

### Purpose
This device tracks objects within a given range and translates their position over time into MIDI data for use with any device that accepts USB MIDI. It is easily reproducible with 3D printing and commonly available parts. The device is designed to run without any reprogramming by the end user, but the open-source nature allows for deep customization if you so choose.

### Applications
- Parameter modulation based on how members of a band move around on stage
- Allowing dancers to play chords and notes based on their choreography without needing to wear sensors
- Control of the sound in an art installation based on audience movement
- Audience feedback that can control an aspect of the overall performance sound


# Fabrication

### Required Parts
* [Teensy 4.1](https://www.pjrc.com/store/teensy41_pins.html) 
  - Ordering the TEENSY41_PINS version is easiest, but you can solder your own pins if you prefer
* [Parts From Amazon](https://www.amazon.com/hz/wishlist/ls/1BWAAT5W76UC8?ref_=wl_share)
* [BOM From Digikey](https://bomtool.oemsecrets.com/bom/w43v497o3n)
  - You can switch out the types of connectors and wires here 
  
I will list the McMaster-Carr parts out in a table by individual pieces, as they may be easy to substitute with something you have on hand, and ordering packs of 50-100 just for this project could quickly double the budget.

| Quantity | Part #     | Description                                                                           |
|:--------:|:----------:|:------------------------------------------------------------------------------------- |
| 4        | 98952A106  | Aluminum Male-Female Threaded Hex Standoff, 4.5mm Hex, 8mm Long, M3 x 0.50 Mm Thread  |
| 4        | 91290A113  | Alloy Steel Socket Head Screw, Black-Oxide, M3 x 0.5 Mm Thread, 8 Mm Long             |
| 6        | 91290A111  | Alloy Steel Socket Head Screw, Black-Oxide, M3 x 0.5 Mm Thread, 6 Mm Long   |
| 3        | 91290A572  | Alloy Steel Socket Head Screw, Black-Oxide, M3 x 0.5 Mm Thread, 15 Mm Long  |
| 1        | 90611A112  | Screw-Mount Nut, Steel, 1/4"-20 Thread, 0.313" Diameter x 1/4" High Barrel    |
| 13       | 97258A101  | 18-8 Stainless Steel Thin Square Nut, M3 x 0.5 Mm Thread   |
| 7        | 91290A102  | Alloy Steel Socket Head Screw, Black-Oxide, M2.5 x 0.45 Mm Thread, 8 Mm Long   |
| 7        | 90710A025  | 18-8 Stainless Steel Thin Hex Nut, M2.5 x 0.45 Mm Thread, Din 439B, Iso 4035   |


### 3D Printing
The enclosure is printed in PETG for better durability and resilience to heat. I printed using a Prusa Mini+ and Prusament. The GCode for that combination is included in this repository, along with .3mf files, which contain prusa slicer projects that you can customize to your needs. Please be aware that in order to achieve a clean look, the print is set to pause whenever a square nut needs to be inserted in the body. If you tweak the settings, those pause points will likely be inaccurate, so you will need to reset them. I'll post further documentation on that process in the 3D_Print folder. For now, you will just find two sets of files, as the print is split between top and bottom.

<img width="500" alt="Top Print Preview" src="https://user-images.githubusercontent.com/70541863/236166066-f1a8b43d-44d0-4eee-aa8e-1dc49ecb9305.png"> 

<img width="500" alt="Bottom Print Preview" src="https://user-images.githubusercontent.com/70541863/236166549-203476f3-fbc0-467d-be78-e5861fd41722.png">

It is also important to note that there is a long, thin bridge where the top of the LCD screen sits, and the best way I found to get it to print was using the organic supports, which are now available in the stable build of PrusaSlicer.


### Custom PCB

<img width="500" alt="PCB" src="https://user-images.githubusercontent.com/70541863/236168416-d5efb0a1-d0ce-41b9-8587-a8afba6bdd5f.png">

This is more of a breakout board than anything and is at the top of my priority list for a revision as I'd like to make the internal USB connection cleaner and add the ability to use some better libraries to drive the screen. It is fairly cheap to manufacture through a company like [JLCPCB](https://jlcpcb.com/) and I have included the Gerber files in the PCB folder. Simply upload them to the website and pick what color you want the board to be.


### Soldering

I will expand on this section in the coming weeks, but the main soldering can be broken down into 4 groups: 
  1. Cut micro USB cable soldered to USB C breakout board
  2. Soldering all the pins to the custom PCB
  3. Desoldering the header pins that came with the LCD and soldering the Molex connector on (this can be skipped if you aren't concerned about cables coming loose inside)
  4. Cutting the default RPLiDAR cable and resoldering it to the right wire gauge/connector for your board

### Assembly

This area needs detailed photos of the assembly steps, so until I have those, here is an exploded view from Fusion360.

https://user-images.githubusercontent.com/70541863/236175005-40a7cf59-78cd-4779-9f89-b16eafd7d8d7.mp4


# Software

### Building and Installing

This project uses Platform.IO and CLion, though VSCode shouldn't be an issue. The most important thing is just to make sure platform.io is installed and configured correctly. Then, you should be able to clone the repository, and your IDE will do the rest of the work. I will also put compiled versions in the release area in case you just want to use teensy loader to flash the device without changing the code.

### Code Organization

- __ZGLidar__ : Handles management of the physical lidar device and pulls data off the serial buffer.
- __ZGObjectTracker__ : This analyzes point cloud data from the lidar and attempts to match objects' positions over time. Holds all tracked objects and tells them when they are no longer relevant
- __ZGObject__ : Represents a tracked point and manages all midi updates and signals throughout its lifetime. It only requires updated coordinates to derive further parameters that it needs to send
- __ZGDisplay__ : This manages the real-time data display and touchscreen menu
- __ZGConversionHelpers__ : Inline functions that are useful in multiple objects

### Touch Screen Options

- __Gear Icon__ : Brings up the main menu 
  - __SCAN__ : Contains settings related to LiDAR data processing 
    - _Range_ - Sets the maximum detection distance for object tracking
    - _Algorithm_ - Switches between Distance (Low Latency/Low Accuracy) and DBSCAN (High Accuracy/10-20ms added latency)
  - __MIDI__ : Contains settings that modify the Midi being sent as a result of processing the data 
    - _Root Note_ - Sets the note that will be assigned to the 0-degree position
    - _Scale Type_ - Changes the number of notes in a 360-degree pattern and the offset for each degree to quantize to a scale mode
  - __DISPLAY__ : Contains settings that modify the information displayed during real-time updates
    - _Main View_ - Allows the user to select between a plot of tracked points and a debug screen outputting latency measurements
  - __ABOUT__ : Contains info about the project and current version

# Next Steps
- [ ] Full code documentation
- [ ] Custom PCB revision
- [ ] Detailed Assembly Guide
- [ ] Updated GUI framework
- [ ] More touch-friendly GUI design
- [ ] More user control over MIDI
- [ ] Ability to set scan exclusion zones for use in smaller spaces

