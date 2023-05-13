# Target_Practice

A Ardunio based Wemos D1 Mini controller used to create a fencing target. The target takes input from the A,B and C lines from the fencing body cord and compares it to Digital Input signals from three Targets. The B Line is connected to the Analog Pin.

The [enclosure](https://cad.onshape.com/documents/c0f35fa07750503116a4982c/w/df9ccf835c31fb7563f26bd6/e/0c82ad34fe834bd92dafb4dc) was created using Onshape and houses the Wemos, an OLED display and a momentary contact push button.

The electric schematic is described in [Schematic_Fencing_Target_Individually_Addresseable.pdf](Schematic_Fencing_Target_Individually_Addresseable.pdf).

The target has two modes. One mode signals a light when the target is hit, the other mode is a Reaction Time mode that signals the fencer to hit. In Reaction Time mode the signal is sent three time and the reaction times are averaged and displayed to the fencer in milliseconds.

Epee: Epee is designed so that it will light when the A and B lines close. The light will not illuminate if the controller senses that one of the targets was hit with the epee.

Foil: Foil is accomplished by breaking the contact between the B and C lines. There is a sleep mode which will turn the LED lights off after 5 seconds. Plugging in the foil and then hitting a target again will wake up the LED lights.

Saber: Saber will light when a target is hit, closing the C line to one of the targets. The reaction time for Saber will light up 2 of the 3 regions on the addressable LED Strip and the fencer should strike the unlit portion. The light color will change regardless of the region struck, but the correct region hit will yield a green light while incorrect will be blue.

For the Target I used an old lamé. I sewed two strips of cloth, one horizontal and one vertical onto the back of the lamé so that it was divided into three parts. I then cut the lamé down the center of the pieces of cloth to create three electrically isolated regions. I also need two pieces of cloth along the shoulders.
