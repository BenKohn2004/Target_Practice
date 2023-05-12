# Target_Practice

A Ardunio based Wemos D1 Mini controller used to create a fencing target. The target takes input from the A,B and C lines from the fencing body cord and compares it to Digital Input signals from three Targets. The B Line is connected to the Analog Pin.

The [enclosure](https://cad.onshape.com/documents/c0f35fa07750503116a4982c/w/df9ccf835c31fb7563f26bd6/e/0c82ad34fe834bd92dafb4dc) was created using Onshape and houses the Wemos, an OLED display and a momentary contact push button.

The electric schematic is described in [Schematic_Fencing_Target_Individually_Addresseable.pdf](Schematic_Fencing_Target_Individually_Addresseable.pdf).

The target has two modes. One mode signals a light when the target is hit, the other mode is a Reaction Time mode that signals the fencer to hit. In Reaction Time mode the signal is sent three time and the reaction times are averaged and displayed to the fencer in milliseconds.
