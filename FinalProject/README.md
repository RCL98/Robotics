- HARDWARE: 
     1. 2x Stepper Motors taken from old DVD drives: for moving the pen left and right, and the paper forward and backwards
     2. 2x Servo Motors: for pulling the pen up and down with the help of some strings
     3. 2x L293D ICs Motor drivers: for an easier control of the dvd's stepper motors
     4. 2x BreadBoards (3x optionally)
     5. 2x Supply Modules HW-131: for powering the stepper motors, as the usb connected Arduino Uno does not give enough for that task
     6. 2x Power Adapters
     7. 1x 1k ohms Rezsitor
     8. 1x 330 ohms Rezistor
     9. 1x Blue LED: brings attention to the user that the color needs to be changed
     10. 1x Active Piezzo Buzzer: same task as the Blue LED
     11. 1x Arduino Uno: controls all the mechanisms
     
     https://ibb.co/6n7z05f (an image with the hardware)
     
     Video: https://youtu.be/5U-HVZ_UWF0
     
     
 - HOW TO USE:
     - In order for the machine to work you would need an image and a gCode file that will tell the machine how to move in order to plot that image.
     For the image you need to use Inkscape. It's a free and open-source software that you can find [here](https://inkscape.org/release/inkscape-0.92.4/) (version 0.48.5 is recommended). You can find tutprials for printing text and other things with Inkscape [here](https://youtu.be/KD50yDop8kQ) and [here](https://youtu.be/7BFa0k6FJcc).
     - For the gCode files you will need to use an extension found [here](https://github.com/martymcguire/inkscape-unicorn). As color commands are not native to gCode, you will need to replace some of the files of the extension with the ones found in the unicorn folder from this repository. These files have been specifically modified in order to work with this CNC MACHINE(i.e. added color commands).
     - Finally you will need to use the gctrl.pde processing program that will take the gCode files and upload them to the Ardunio where they will be processed. For this you will need to use [Proccesing](https://processing.org/) and the gctrl.pde aplication found [here](https://github.com/damellis/gctrl/blob/master/gctrl.pde). You will need to start Proccesing, and upload the gctrl.pde aplication to it.
 This should bring up the following window: https://cdn.instructables.com/FAB/UNTL/IHP6UI22/FABUNTLIHP6UI22.LARGE.jpg?auto=webp&frame=1&width=528&fit=bounds
