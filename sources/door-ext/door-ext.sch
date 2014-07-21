EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 24 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Module Extérieur"
Date "21 juil. 2014"
Rev "REV 1"
Comp "Nerdvellous"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATTINY84A-P IC1
U 1 1 53CD4D74
P 6000 3450
F 0 "IC1" H 5150 4200 40  0000 C CNN
F 1 "ATTINY84A-P" H 6700 2700 40  0000 C CNN
F 2 "DIP14" H 6000 3250 35  0000 C CIN
F 3 "" H 6000 3450 60  0000 C CNN
	1    6000 3450
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 53CD4E4A
P 4850 2800
F 0 "#PWR?" H 4850 2890 20  0001 C CNN
F 1 "+5V" H 4850 2890 30  0000 C CNN
F 2 "" H 4850 2800 60  0000 C CNN
F 3 "" H 4850 2800 60  0000 C CNN
	1    4850 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 2800 4850 2850
Wire Wire Line
	4850 2850 4950 2850
$Comp
L GND #PWR?
U 1 1 53CD4EDC
P 4900 4150
F 0 "#PWR?" H 4900 4150 30  0001 C CNN
F 1 "GND" H 4900 4080 30  0001 C CNN
F 2 "" H 4900 4150 60  0000 C CNN
F 3 "" H 4900 4150 60  0000 C CNN
	1    4900 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 4050 4900 4050
Wire Wire Line
	4900 4050 4900 4150
$EndSCHEMATC
