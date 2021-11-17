EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Printalyzer Densitometer - Transmission LED Board"
Date "2021-??-??"
Rev "C*"
Comp "Dektronics, Inc."
Comment1 "Derek Konigsberg <dkonigsberg@dektronics.com>"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:LED_ALT D1
U 1 1 609352D1
P 2900 2200
F 0 "D1" H 2900 2325 50  0000 C CNN
F 1 "LED" H 2900 2075 50  0000 C CNN
F 2 "lib_fp:LED_WL-SWTP-3022-1MP_ThermalVias" H 2900 2200 50  0001 C CNN
F 3 "https://katalog.we-online.de/led/datasheet/158302250.pdf" H 2900 2200 50  0001 C CNN
F 4 "LED WL-SWTP COOL WHT 5000K 2SMD" H 2900 2200 50  0001 C CNN "Description"
F 5 "Würth Elektronik" H 2900 2200 50  0001 C CNN "Manufacturer"
F 6 "158302250" H 2900 2200 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2900 2200 50  0001 C CNN "Supplier"
F 8 "732-5666-1-ND" H 2900 2200 50  0001 C CNN "Supplier PN"
	1    2900 2200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 6093685B
P 2300 2100
F 0 "J1" H 2300 2225 50  0000 C CNN
F 1 "Conn_01x02" H 2300 1875 50  0000 C CNN
F 2 "Connector_JST:JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal" H 2300 2100 50  0001 C CNN
F 3 "https://www.jst-mfg.com/product/pdf/eng/eSH.pdf" H 2300 2100 50  0001 C CNN
F 4 "CONN HEADER SMD R/A 2POS 1MM" H 2300 2100 50  0001 C CNN "Description"
F 5 "JST Sales America Inc." H 2300 2100 50  0001 C CNN "Manufacturer"
F 6 "SM02B-SRSS-TB(LF)(SN)" H 2300 2100 50  0001 C CNN "Manufacturer PN"
F 7 "TLC" H 2300 2100 50  0001 C CNN "Supplier"
F 8 "SM02B-SRSS-TB(LF)(SN)" H 2300 2100 50  0001 C CNN "Supplier PN"
	1    2300 2100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2750 2200 2500 2200
$Comp
L Mechanical:MountingHole H2
U 1 1 6093F399
P 3500 2150
F 0 "H2" H 3600 2196 50  0000 L CNN
F 1 "MountingHole" H 3600 2105 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 3500 2150 50  0001 C CNN
F 3 "~" H 3500 2150 50  0001 C CNN
	1    3500 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 2100 2675 2100
Wire Wire Line
	2675 2100 2675 1950
Wire Wire Line
	2675 1950 3150 1950
Wire Wire Line
	3150 1950 3150 2200
Wire Wire Line
	3150 2200 3050 2200
$Comp
L Mechanical:Fiducial FID2
U 1 1 6195409E
P 4275 2150
F 0 "FID2" H 4360 2196 50  0000 L CNN
F 1 "Fiducial" H 4360 2105 50  0000 L CNN
F 2 "Fiducial:Fiducial_1mm_Mask2mm" H 4275 2150 50  0001 C CNN
F 3 "~" H 4275 2150 50  0001 C CNN
	1    4275 2150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:Fiducial FID3
U 1 1 619541E3
P 4275 2400
F 0 "FID3" H 4360 2446 50  0000 L CNN
F 1 "Fiducial" H 4360 2355 50  0000 L CNN
F 2 "Fiducial:Fiducial_1mm_Mask2mm" H 4275 2400 50  0001 C CNN
F 3 "~" H 4275 2400 50  0001 C CNN
	1    4275 2400
	1    0    0    -1  
$EndComp
Text Notes 3425 1700 0    50   ~ 0
Board Mounting
Text Notes 2075 1700 0    50   ~ 0
Transmission LED
$Comp
L Mechanical:Fiducial FID1
U 1 1 61953C8F
P 4275 1900
F 0 "FID1" H 4360 1946 50  0000 L CNN
F 1 "Fiducial" H 4360 1855 50  0000 L CNN
F 2 "Fiducial:Fiducial_1mm_Mask2mm" H 4275 1900 50  0001 C CNN
F 3 "~" H 4275 1900 50  0001 C CNN
	1    4275 1900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 6093E7FB
P 3500 1900
F 0 "H1" H 3600 1946 50  0000 L CNN
F 1 "MountingHole" H 3600 1855 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 3500 1900 50  0001 C CNN
F 3 "~" H 3500 1900 50  0001 C CNN
	1    3500 1900
	1    0    0    -1  
$EndComp
Wire Notes Line
	3375 1575 3375 2600
Wire Notes Line
	3375 2600 4800 2600
Wire Notes Line
	4800 2600 4800 1575
Wire Notes Line
	4800 1575 3375 1575
Wire Notes Line
	2025 1575 2025 2600
Wire Notes Line
	2025 2600 3275 2600
Wire Notes Line
	3275 2575 3275 1575
Wire Notes Line
	3275 1575 2025 1575
$EndSCHEMATC
