<style>
.r2{
	text-align:right;
}
</style>

## Upload the binary to the Waveshare ESP32 S3 3.5" Touch

These instructions will walk you through the steps to install the firmware on the device. You can do this before connecting the GPS.

```
NOTE : 

If everything goes wrong you can factory reset the device by uploading the original firmware at address 0x0.  The firmware can be downloaded from https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-3.5/ESP32-S3-Touch-LCD-3.5-Demo.zip The firmware bin can be found at '..\ESP32-S3-Touch-LCD-3.5-Demo\Firmware'

Also check out the Wiki 
https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-3.5
```



- Download the RTK Server files from [Waveshare ESP32 S3 3.5" Touch](https://1drv.ms/f/c/85ac5a1466e8dffa/Qvrf6GYUWqwggIWYqgEAAAAAAQn6uUMdvsv8OQ). 

## Method 1 (Easier)

1) Open https://espressif.github.io/esp-launchpad/ which does not require anything to be downloaded to your computer.

2) Select `Connect` the choose the device in the popup. If you cant connect, try holding down the boot button on the back of the device while tapping the reset button.

3) Select `DIY`

4) Add the files shown in the files and addresses table below. Note for a fresh install from the factory you only need to add `firmware.bin` at address `0x10000`. IMPORTANT the address default to `0x1000` which is missing one `0`

<table style='margin-left:100px;'>
<tr><th>File</th><th>Address</th><th>Required</th></tr>
<tr><td>bootloader.bin</td><td class='r2'>0x0000</td><td>No</td></tr>
<tr><td>partitions.bin</td><td class='r2'>0x8000</td><td>No</td></tr>
<tr><td>boot_app0.bin</td><td class='r2'>0xE000</td><td>No</td></tr>
<tr><td>firmware.bin<td class='r2'>0x10000</td><td>YES</td></tr>
</table>

5) Select `Program`

6) Once upload completes. Wait 30 seconds thenselect `RESET`.

7) The device will reboot. The first time it runs, it will format the flash memory. This can rake up to 3 minutes. (Please be patient).

8) Once startup is complete the screen should look like the following without the green buttons.

	<img src="Photos/ScreenShots/StartupScreen.JPG?raw=true" width="200"/>

9) Next go to the [WiFi setup - Here](README.md#connect-wifi)
 


## Method 2

1) Get the Espressif Flash Download Tools from https://www.espressif.com/en/support/download/other-tools
	- The tool is called Flash Download Tools
	- If the tool doesn't work out of the box watch https://www.youtube.com/watch?v=BVJlc4hio6A to get the correct drivers
2) Launch flash_download_tool_3.9.7.exe (Version number may be a little different)
3) Select ChipType: ESP32-S3

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display-S3/Flash-ChipSelect.png?raw=true" width="200"/>
4) Add each file to the list and address. Be sure to add a tick next to each file and the correct address

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display-S3/Flash-FileAssignment.png?raw=true" width="300"/>

	| File | Address | 
	| --- | --- | 
 	| bootloader.bin|0x0 | 
 	| partitions.bin|0x8000 | 
 	| boot_app0.bin |0xe000 |
 	| firmware.bin |0x10000|

5) Set SPI speed to 80MHz
6) Double check your work
7) Press `START`
8) If all goes well it should look like

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display-S3/Flash-CompleteResult.png?raw=true" Width="600"/>

```
NOTE : The first time you install it may take up to 90 seconds to startup after reset. Please be patient
```

9) Power cycle or hit reset. It should look like

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display-S3/PRE_WIFI.jpg?raw=true" width="300"/>

10) If it doesn't you can reset the device with the firmware_no_touch.bin downloaded from the TTGO website
	- Don't use the ERASE button unless you want to reload the TTGO firmware