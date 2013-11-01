# Due Usbprog

Use flashrom tool and Arduino Due to program flash. Based on [serprog-duino](https://gitorious.org/gnutoo-personal-arduino-projects/serprog-duino/source/8856a3ad962b16383ace64d6d977bae34c56af0b:), refer to [Serprog/Arduino flasher](http://www.flashrom.org/Serprog/Arduino_flasher)

## How To

+ Open sketch `due_usbprog`.
+ Upload it to DUE
+ Use `flashrom` to program flash. [flashrom](http://flashrom)

## Connections

	Arduino DUE						Flash
		A5							  CS
	   MOSI						 	 MOSI
	   MISO                          MISO
	    SCK 						  SCK
	 	GND 						  GND
	 	3V 							  3V

## Commands

### Write

	flashrom -p serprog:dev=/dev/ttyACM0:2000000 -w w_fw.bin

### Read
	
	flashrom -p serprog:dev=/dev/ttyACM0:2000000 -r r_fw.bin

