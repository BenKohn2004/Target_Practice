# Target_Practice

An Ardunio based Wemos D1 Mini controller used to create a fencing target. The target takes input from the A,B and C lines from the fencing body cord and compares it to Digital Input signals from three Targets. The B Line is connected to the Analog Pin.

The [enclosure](https://cad.onshape.com/documents/c0f35fa07750503116a4982c/w/df9ccf835c31fb7563f26bd6/e/0c82ad34fe834bd92dafb4dc) was created using Onshape and houses the [Wemos](https://www.amazon.com/Organizer-ESP8266-Internet-Development-Compatible/dp/B081PX9YFV/ref=sr_1_3?keywords=wemos+d1+mini&sr=8-3), an [OLED](https://www.amazon.com/Songhe-0-96-inch-I2C-Raspberry/dp/B085WCRS7C/ref=sr_1_5?keywords=oled+ssd1306&sr=8-5) display, a momentary contact push button [WS2812](https://www.amazon.com/BTF-LIGHTING-Flexible-Individually-Addressable-Non-waterproof/dp/B01CDTEJBG/ref=sxin_16_pa_sp_search_thematic_sspa?content-id=amzn1.sym.9aaeff48-3310-46df-94ec-ea59eda4c869%3Aamzn1.sym.9aaeff48-3310-46df-94ec-ea59eda4c869&cv_ct_cx=ws2812b%2Bled%2Bstrip&keywords=ws2812b%2Bled%2Bstrip&pd_rd_i=B01CDTEJBG&pd_rd_r=243e2b19-9ba6-4468-9cfc-878a01be8559&pd_rd_w=z607i&pd_rd_wg=7a3mR&pf_rd_p=9aaeff48-3310-46df-94ec-ea59eda4c869&pf_rd_r=Y0J400D35XPYA4JBNP74&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sr=1-1-2b34d040-5c83-4b7f-ba01-15975dfb8828-spons&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUEyWkhIMEEySDNRRVY2JmVuY3J5cHRlZElkPUEwNzQ2NzA0TFQ2WjFEUk5MV0dUJmVuY3J5cHRlZEFkSWQ9QTAwMDA5NThWS0s4UUpSQTRMUU4md2lkZ2V0TmFtZT1zcF9zZWFyY2hfdGhlbWF0aWMmYWN0aW9uPWNsaWNrUmVkaXJlY3QmZG9Ob3RMb2dDbGljaz10cnVl&th=1) Individually Addressable LEDs.

The electric schematic is described in [Schematic_Fencing_Target_Individually_Addresseable.pdf](Schematic_Fencing_Target_Individually_Addresseable.pdf). It is powered from a 5V power supply. In this case I used the 5V from the WEMOS usb connection, but it is designed to use a dedicated 5V power source.

The target has two modes. One mode signals a light when the target is hit, the other mode is a Reaction Time mode that signals the fencer to hit. In Reaction Time mode the signal is sent three time and the reaction times are averaged and displayed to the fencer in milliseconds.

Epee: Epee is designed so that it will light when the A and B lines close. The light will not illuminate if the controller senses that one of the targets was hit with the epee.

Foil: Foil is accomplished by breaking the contact between the B and C lines. There is a sleep mode which will turn the LED lights off after 5 seconds. Plugging in the foil and then hitting a target again will wake up the LED lights.

Saber: Saber will light when a target is hit, closing the C line to one of the targets. The reaction time for Saber will light up 2 of the 3 regions on the addressable LED Strip and the fencer should strike the unlit portion. The light color will change regardless of the region struck, but the correct region hit will yield a green light while incorrect will be blue.

For the Target I used an old lamé. I sewed two strips of cloth, one horizontal and one vertical onto the back of the lamé so that it was divided into three parts. I then cut the lamé down the center of the pieces of cloth to create three electrically isolated regions. I also need two pieces of cloth along the shoulders.
