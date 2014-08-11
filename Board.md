| Function      | Pin | FT | Alt     |
| --------------|-----|----|---------|
|Keypad         |PA0  |no  |         |
|Keypad         |PA1  |no  |         |
|Keypad         |PA2  |no  |         |
|Keypad         |PA3  |no  |         |
|Keypad         |PA4  |no  |         |
|Keypad         |PA5  |no  |         |
|Keypad         |PA6  |no  |         |
|Keypad         |PA7  |no  |         |
|Switch         |PC10 |yes |         |
|Switch         |PC11 |yes |         |
|Switch         |PC12 |yes |         |
|Encoder Btn    |PB5  |no  |         |
|Encoder        |PB6  |yes |TIM4_CH1 |
|Encoder        |PB7  |yes |TIM4_CH2 |
|LCD (RS)       |PB15 |yes |         |
|LCD (R/W)      |PB8  |yes |         |
|LCD (E)        |PB9  |yes |         |
|LCD (DB4)      |PA8  |yes |         |
|LCD (DB5)      |PA9  |yes |         |
|LCD (DB6)      |PA10 |yes |         |
|LCD (DB7)      |PA11 |yes |         |
|Driver (RESET) |PB10 |yes |         |
|Driver (ENABLE)|PB11 |yes |         |
|Driver (DIR)   |PB12 |yes |         |
|Driver (STEP)  |PB13 |yes |TIM1_CH1N|
|Spindle (quad) |PC6  |yes |TIM3_CH1 |
|Spindle (quad) |PC7  |yes |TIM3_CH2 |
|Spindle (index)|PB14 |yes |TIM15_CH1|
|Button 1       |PC0  |no  |         |
|Button 2       |PC1  |no  |         |
|Button 3       |PC2  |no  |         |
|Button 4       |PC3  |no  |         |


Wires color coding
------------------

Driver

|Wire color|Function|Pin |
|----------|--------|----|
|Red       |5V      |    |
|Black     |GND     |    |
|Gray      |STEP    |PB13|
|Orange    |DIR     |PB12|
|Green     |ENABLE  |PB11|
|Brown     |RESET   |PB10|

Spindle

|Wire color|Function|Pin |
|----------|--------|----|
|Red       |5V      |    |
|Black     |GND     |    |
|Blue      |QUAD    |PC6 |
|Green     |QUAD    |PC7 |
|White     |INDEX   |PB14|

Switch

|Wire color|Function|Pin |
|----------|--------|----|
|Blue      |1st     |PC10|
|Green     |2st     |PC11|
|Brown     |3rd     |PC12|
|Black     |GND,4th |    |

Encoder

|Wire color|Function|Pin |
|----------|--------|----|
|Red       |3.3V    |    |
|Black     |GND     |    |
|White     |Button  |PB5 |
|Brown     |QUAD    |PB6 |
|Gray      |QUAD    |PB7 |

Extra Buttons

|Wire color|Function|Pin |
|----------|--------|----|
|Black     |GND     |    |
|White     |Button 1|PC0 |
|Green     |Button 2|PC1 |
|Brown     |Button 3|PC2 |
|Gray      |Button 4|PC3 |


TIM6: utility timer
TIM7: debounce timer

Colors:
Red
Black
Violet
Orange
White
Brown
Gray
Green
Blue
Yellow

