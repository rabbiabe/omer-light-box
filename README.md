# Edge-Lit Omer Counter
![Photo of finished project](/images/00-finished-project.jpg)
## objective
I designed this project to create an attractive reminder device for *sefirat ha-omer*. It does not have all the features that can be found in smartphone apps; my goals were to create an attractive physical device that would have an automatic timeout so it could provide a reminder on Shabbat/Yom Tov without requiring the user to interact directly with the device. The aesthetic concept took inspiration from [this video](https://www.youtube.com/watch?v=wuDAnHHIx6M&pp=ygUTZWxlbWVudDE0IGxlZCBzaWducw%3D%3D).

#### Possible features someone else might want to add:
- Use Pico W to interface with HebCal (or other online API?) to automatically sync current time, Hebrew date, and nightfall
- Audio reminder 
- Text of blessing
- user-configurable display timeouts (currently hard-coded at 5s for quick view and 5h for alarm-triggered display)

## Materials
These are the materials I used for this project, with links to specific items where relevant. However, I did my best to design the project in ways that would allow flexibility in physical setup and design.

### Enclosure
- [Wooden box with clear lid](https://www.amazon.com/gp/product/B08DG97HP2/ref=ppx_od_dt_b_asin_title_s00?ie=UTF8&psc=1)
- [4" square plexiglass sheets](https://www.amazon.com/gp/product/B0BK5697J9/ref=ppx_od_dt_b_asin_title_s00?ie=UTF8&psc=1) – these are thin so I used 3 in a stack to get the right thickness relative to the LEDs
- 2xAAA battery holder + batteries as backup
- Some kind of 5VDC supply 

### Circuit components
- Raspberry Pi Pico
- Rotary encoder with switch
- WS2812B addressable LED strip (the box linked above will exactly fit a strip of 24 LEDs)
- 128x64 OLED display with I2C
- Trimpot (I used 10k but the actual value is not critical)
- LED for low battery indicator
- Diodes, resistors, capacitors, protoboard, wires , etc. (see schematic)

### Tools
- Scriber tool ([like this](https://www.amazon.com/gp/product/B081967TD3/ref=ppx_od_dt_b_asin_title_s00?ie=UTF8&psc=1)) or another way to etch the plexiglass. Dremel will work but may be overkill, especially on thinner sheets.
- Xacto or other project knife
- Metal straight edge
- Hot glue gun
- Drill (if you add an external DC connection)

## Assembly
Here are the basic steps – I will add photos of the steps as I have a chance to take them.

## Code
the project code is broken into a few different modules for clarity and organization.
- `omer-counter:` main(), global variables and definitions, and interrupt/timer callbacks
- `omer-obj:` OmerCounter class and some data structures for handling Hebrew dates and the omer count. 
- `display:` Functions and variables to control the OLED and LED string. 
### Dependencies
This project requires [PicoLED](https://github.com/ForsakenNGS/PicoLED) to control the LED strip and [SSD1306_OLED_PICO](https://github.com/gavinlyonsrepo/SSD1306_OLED_PICO) for the screen output. The source files for these libraries should be placed in their respective subfolders of lib/.

