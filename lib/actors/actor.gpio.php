<?php

/*
 *
 * Copyright 2024 Dieter Naujoks <devops@service.istmein.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

/*

       read <pin>
              Read the digital value of the given pin and print 0 or  1  to
              represent the respective logic levels.

       write <pin> <value>
              Write  the  given  value (0 or 1) to the pin. You need to set
              the pin to output mode first.

       toggle <pin>
              Changes the state of a GPIO pin; 0 to 1, or 1 to 0.
              Note unlike the blink command, the pin must be in output mode
              first.

       gpio mode 4 output # Set pin 4 to output

       gpio -g mode 23 output # Set GPIO pin 23 to output (same as WiringPi
       pin 4)

       gpio mode 1 pwm # Set pin 1 to PWM mode

       gpio pwm 1 512 # Set pin 1 to PWM value 512 - half brightness

       gpio export 17 out # Set GPIO Pin 17 to output

       gpio export 0 in # Set GPIO Pin 0 (SDA0) to input.

       gpio -g read 0 # Read GPIO Pin 0 (SDA0)

*/

/*
gpioinfo:

gpiochip0 - 54 lines:
        line   0:       "ID_SDA"                input
        line   1:       "ID_SCL"                input
        line   2:       "SDA1"                  input
        line   3:       "SCL1"                  input
        line   4:       "GPIO_GCLK"             output drive=open-drain consumer="onewire@4"
        line   5:       "GPIO5"                 input
        line   6:       "GPIO6"                 input
        line   7:       "SPI_CE1_N"             input
        line   8:       "SPI_CE0_N"             input
        line   9:       "SPI_MISO"              input
        line  10:       "SPI_MOSI"              input
        line  11:       "SPI_SCLK"              input
        line  12:       "GPIO12"                input
        line  13:       "GPIO13"                input
        line  14:       "TXD0"                  input
        line  15:       "RXD0"                  input
        line  16:       "GPIO16"                input
        line  17:       "GPIO17"                input
        line  18:       "GPIO18"                input
        line  19:       "GPIO19"                input
        line  20:       "GPIO20"                input
        line  21:       "GPIO21"                input
        line  22:       "GPIO22"                input consumer="sysfs"
        line  23:       "GPIO23"                input
        line  24:       "GPIO24"                input
        line  25:       "GPIO25"                input consumer="sysfs"
        line  26:       "GPIO26"                input
        line  27:       "GPIO27"                input consumer="sysfs"
        line  28:       "SDA0"                  input
        line  29:       "SCL0"                  input
        line  30:       "NC"                    input
        line  31:       "LAN_RUN"               output
        line  32:       "CAM_GPIO1"             output
        line  33:       "NC"                    input
        line  34:       "NC"                    input
        line  35:       "PWR_LOW_N"             input consumer="PWR"
        line  36:       "NC"                    input
        line  37:       "NC"                    input
        line  38:       "USB_LIMIT"             output
        line  39:       "NC"                    input
        line  40:       "PWM0_OUT"              input
        line  41:       "CAM_GPIO0"             output consumer="cam1_regulator"
        line  42:       "SMPS_SCL"              output
        line  43:       "SMPS_SDA"              input
        line  44:       "ETH_CLK"               input
        line  45:       "PWM1_OUT"              input
        line  46:       "HDMI_HPD_N"            input
        line  47:       "STATUS_LED"            output consumer="ACT"
        line  48:       "SD_CLK_R"              input
        line  49:       "SD_CMD_R"              input
        line  50:       "SD_DATA0_R"            input
        line  51:       "SD_DATA1_R"            input
        line  52:       "SD_DATA2_R"            input
        line  53:       "SD_DATA3_R"            input


*/


class gpio extends actor
{

	function __construct($_port, $_addr, $_res)
	{
		parent::__construct($_port, $_addr, $_res);
	}

	function __destruct()
	{
		parent::__destruct();
	}

	public function init($gpio)
	{
	}

	public function action($gpio)
	{
	}

}


?>