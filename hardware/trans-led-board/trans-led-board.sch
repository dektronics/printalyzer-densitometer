EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Printalyzer Densitometer - Transmission LED Board"
Date "2021-06-15"
Rev "A"
Comp "LogicProbe.org"
Comment1 "Derek Konigsberg"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:LED_ALT D1
U 1 1 609352D1
P 2575 2150
F 0 "D1" H 2575 2275 50  0000 C CNN
F 1 "LED" H 2575 2025 50  0000 C CNN
F 2 "lib_fp:LED_WL-SWTP-3022" H 2575 2150 50  0001 C CNN
F 3 "https://katalog.we-online.de/led/datasheet/158302250.pdf" H 2575 2150 50  0001 C CNN
F 4 "LED WL-SWTP COOL WHT 5000K 2SMD" H 2575 2150 50  0001 C CNN "Description"
F 5 "Würth Elektronik" H 2575 2150 50  0001 C CNN "Manufacturer"
F 6 "158302250" H 2575 2150 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2575 2150 50  0001 C CNN "Supplier"
F 8 "732-5666-1-ND" H 2575 2150 50  0001 C CNN "Supplier PN"
	1    2575 2150
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 6093685B
P 1975 2050
F 0 "J1" H 1975 2175 50  0000 C CNN
F 1 "Conn_01x02" H 1975 1825 50  0000 C CNN
F 2 "Connector_JST:JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal" H 1975 2050 50  0001 C CNN
F 3 "https://www.jst-mfg.com/product/pdf/eng/eSH.pdf" H 1975 2050 50  0001 C CNN
F 4 "CONN HEADER SMD R/A 2POS 1MM" H 1975 2050 50  0001 C CNN "Description"
F 5 "JST Sales America Inc." H 1975 2050 50  0001 C CNN "Manufacturer"
F 6 "SM02B-SRSS-TB(LF)(SN)" H 1975 2050 50  0001 C CNN "Manufacturer PN"
F 7 "TLC" H 1975 2050 50  0001 C CNN "Supplier"
F 8 "SM02B-SRSS-TB(LF)(SN)" H 1975 2050 50  0001 C CNN "Supplier PN"
	1    1975 2050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2425 2150 2175 2150
$Comp
L Mechanical:MountingHole H1
U 1 1 6093E7FB
P 3050 1900
F 0 "H1" H 3150 1946 50  0000 L CNN
F 1 "MountingHole" H 3150 1855 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 3050 1900 50  0001 C CNN
F 3 "~" H 3050 1900 50  0001 C CNN
	1    3050 1900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 6093F399
P 3050 2150
F 0 "H2" H 3150 2196 50  0000 L CNN
F 1 "MountingHole" H 3150 2105 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 3050 2150 50  0001 C CNN
F 3 "~" H 3050 2150 50  0001 C CNN
	1    3050 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2175 2050 2350 2050
Wire Wire Line
	2350 2050 2350 1900
Wire Wire Line
	2350 1900 2825 1900
Wire Wire Line
	2825 1900 2825 2150
Wire Wire Line
	2825 2150 2725 2150
$EndSCHEMATC
