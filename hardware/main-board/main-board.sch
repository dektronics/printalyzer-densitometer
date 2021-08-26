EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Printalyzer Densitometer - Main Board"
Date "2021-??-??"
Rev "?"
Comp "LogicProbe.org"
Comment1 "Derek Konigsberg"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_ST_STM32L0:STM32L052K8Ux U2
U 1 1 6056A21B
P 4525 2400
F 0 "U2" H 4100 3250 50  0000 C CNN
F 1 "STM32L052K8U6" H 4900 1450 50  0000 C CNN
F 2 "lib_fp:ST_UFQFPN32" H 4025 1500 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00108217.pdf" H 4525 2400 50  0001 C CNN
F 4 "IC MCU 32BIT 64KB FLASH 32UFQFPN" H 4525 2400 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 4525 2400 50  0001 C CNN "Manufacturer"
F 6 "STM32L052K8U6" H 4525 2400 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 4525 2400 50  0001 C CNN "Supplier"
F 8 "497-17494-ND" H 4525 2400 50  0001 C CNN "Supplier PN"
	1    4525 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_ALT D1
U 1 1 6056C6A5
P 2150 3000
F 0 "D1" H 2150 3100 50  0000 C CNN
F 1 "LED" H 2143 3126 50  0001 C CNN
F 2 "lib_fp:LED_WL-SWTP-3022" H 2150 3000 50  0001 C CNN
F 3 "https://katalog.we-online.de/led/datasheet/158302250.pdf" H 2150 3000 50  0001 C CNN
F 4 "LED WL-SWTP COOL WHT 5000K 2SMD" H 2150 3000 50  0001 C CNN "Description"
F 5 "Würth Elektronik" H 2150 3000 50  0001 C CNN "Manufacturer"
F 6 "158302250" H 2150 3000 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2150 3000 50  0001 C CNN "Supplier"
F 8 "732-5666-1-ND" H 2150 3000 50  0001 C CNN "Supplier PN"
	1    2150 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_ALT D2
U 1 1 6056CEB5
P 2150 3250
F 0 "D2" H 2150 3350 50  0000 C CNN
F 1 "LED" H 2143 3376 50  0001 C CNN
F 2 "lib_fp:LED_WL-SWTP-3022" H 2150 3250 50  0001 C CNN
F 3 "https://katalog.we-online.de/led/datasheet/158302250.pdf" H 2150 3250 50  0001 C CNN
F 4 "LED WL-SWTP COOL WHT 5000K 2SMD" H 2150 3250 50  0001 C CNN "Description"
F 5 "Würth Elektronik" H 2150 3250 50  0001 C CNN "Manufacturer"
F 6 "158302250" H 2150 3250 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2150 3250 50  0001 C CNN "Supplier"
F 8 "732-5666-1-ND" H 2150 3250 50  0001 C CNN "Supplier PN"
	1    2150 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_ALT D3
U 1 1 6056D4A2
P 2150 3500
F 0 "D3" H 2150 3600 50  0000 C CNN
F 1 "LED" H 2143 3626 50  0001 C CNN
F 2 "lib_fp:LED_WL-SWTP-3022" H 2150 3500 50  0001 C CNN
F 3 "https://katalog.we-online.de/led/datasheet/158302250.pdf" H 2150 3500 50  0001 C CNN
F 4 "LED WL-SWTP COOL WHT 5000K 2SMD" H 2150 3500 50  0001 C CNN "Description"
F 5 "Würth Elektronik" H 2150 3500 50  0001 C CNN "Manufacturer"
F 6 "158302250" H 2150 3500 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2150 3500 50  0001 C CNN "Supplier"
F 8 "732-5666-1-ND" H 2150 3500 50  0001 C CNN "Supplier PN"
	1    2150 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_ALT D4
U 1 1 6056D9B6
P 2150 3750
F 0 "D4" H 2150 3850 50  0000 C CNN
F 1 "LED" H 2143 3876 50  0001 C CNN
F 2 "lib_fp:LED_WL-SWTP-3022" H 2150 3750 50  0001 C CNN
F 3 "https://katalog.we-online.de/led/datasheet/158302250.pdf" H 2150 3750 50  0001 C CNN
F 4 "LED WL-SWTP COOL WHT 5000K 2SMD" H 2150 3750 50  0001 C CNN "Description"
F 5 "Würth Elektronik" H 2150 3750 50  0001 C CNN "Manufacturer"
F 6 "158302250" H 2150 3750 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2150 3750 50  0001 C CNN "Supplier"
F 8 "732-5666-1-ND" H 2150 3750 50  0001 C CNN "Supplier PN"
	1    2150 3750
	1    0    0    -1  
$EndComp
$Comp
L Power_Protection:USBLC6-2SC6 U4
U 1 1 6056E468
P 2225 7175
F 0 "U4" V 2575 6925 50  0000 C CNN
F 1 "USBLC6-2SC6" V 1875 7575 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 2225 6675 50  0001 C CNN
F 3 "https://www.st.com/resource/en/datasheet/usblc6-2.pdf" H 2425 7525 50  0001 C CNN
F 4 "TVS DIODE 5.25V 17V SOT23-6" H 2225 7175 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 2225 7175 50  0001 C CNN "Manufacturer"
F 6 "USBLC6-2SC6" H 2225 7175 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2225 7175 50  0001 C CNN "Supplier"
F 8 "497-5235-1-ND" H 2225 7175 50  0001 C CNN "Supplier PN"
	1    2225 7175
	0    1    -1   0   
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 6056FFDA
P 8700 1075
F 0 "SW1" H 8700 1250 50  0000 C CNN
F 1 "SW_Push" H 8700 1000 50  0000 C CNN
F 2 "lib_fp:ESwitch_TL1105_6mm_H7.5mm" H 8700 1275 50  0001 C CNN
F 3 "https://www.e-switch.com/system/asset/product_line/data_sheet/144/TL1105.pdf" H 8700 1275 50  0001 C CNN
F 4 "SWITCH TACTILE SPST-NO 0.05A 12V" H 8700 1075 50  0001 C CNN "Description"
F 5 "E-Switch" H 8700 1075 50  0001 C CNN "Manufacturer"
F 6 "TL1105SPF160Q" H 8700 1075 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 8700 1075 50  0001 C CNN "Supplier"
F 8 "EG1861-ND" H 8700 1075 50  0001 C CNN "Supplier PN"
	1    8700 1075
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 60570781
P 8700 1775
F 0 "SW2" H 8700 1950 50  0000 C CNN
F 1 "SW_Push" H 8700 1700 50  0000 C CNN
F 2 "lib_fp:ESwitch_TL1105_6mm_H7.5mm" H 8700 1975 50  0001 C CNN
F 3 "https://www.e-switch.com/system/asset/product_line/data_sheet/144/TL1105.pdf" H 8700 1975 50  0001 C CNN
F 4 "SWITCH TACTILE SPST-NO 0.05A 12V" H 8700 1775 50  0001 C CNN "Description"
F 5 "E-Switch" H 8700 1775 50  0001 C CNN "Manufacturer"
F 6 "TL1105SPF160Q" H 8700 1775 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 8700 1775 50  0001 C CNN "Supplier"
F 8 "EG1861-ND" H 8700 1775 50  0001 C CNN "Supplier PN"
	1    8700 1775
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J1
U 1 1 605714B7
P 825 7175
F 0 "J1" H 882 7642 50  0000 C CNN
F 1 "USB_B_Micro" H 882 7551 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Amphenol_10118194_Horizontal" H 975 7125 50  0001 C CNN
F 3 "http://www.amphenol-icc.com/media/wysiwyg/files/drawing/10118194.pdf" H 975 7125 50  0001 C CNN
F 4 "CONN RCPT USB2.0 MICRO B SMD R/A" H 825 7175 50  0001 C CNN "Description"
F 5 "Amphenol ICC (FCI)" H 825 7175 50  0001 C CNN "Manufacturer"
F 6 "Digi-Key" H 825 7175 50  0001 C CNN "Supplier"
F 7 "10118194-0001LF" H 825 7175 50  0001 C CNN "Manufacturer PN"
F 8 "609-4618-1-ND" H 825 7175 50  0001 C CNN "Supplier PN"
	1    825  7175
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 60572EAB
P 8450 5400
F 0 "H1" H 8550 5446 50  0000 L CNN
F 1 "MountingHole" H 8550 5355 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 8450 5400 50  0001 C CNN
F 3 "~" H 8450 5400 50  0001 C CNN
	1    8450 5400
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 60573343
P 8450 5625
F 0 "H2" H 8550 5671 50  0000 L CNN
F 1 "MountingHole" H 8550 5580 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 8450 5625 50  0001 C CNN
F 3 "~" H 8450 5625 50  0001 C CNN
	1    8450 5625
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 605736BA
P 8450 5850
F 0 "H3" H 8550 5896 50  0000 L CNN
F 1 "MountingHole" H 8550 5805 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 8450 5850 50  0001 C CNN
F 3 "~" H 8450 5850 50  0001 C CNN
	1    8450 5850
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 605739B5
P 8450 6075
F 0 "H4" H 8550 6121 50  0000 L CNN
F 1 "MountingHole" H 8550 6030 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 8450 6075 50  0001 C CNN
F 3 "~" H 8450 6075 50  0001 C CNN
	1    8450 6075
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H5
U 1 1 60573BE2
P 9450 5450
F 0 "H5" H 9550 5550 50  0000 L CNN
F 1 "MountingHole" H 9550 5450 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_Pad_Via" H 9450 5450 50  0001 C CNN
F 3 "~" H 9450 5450 50  0001 C CNN
	1    9450 5450
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H6
U 1 1 60573EB9
P 9450 5900
F 0 "H6" H 9550 6000 50  0000 L CNN
F 1 "MountingHole" H 9550 5900 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_Pad_Via" H 9450 5900 50  0001 C CNN
F 3 "~" H 9450 5900 50  0001 C CNN
	1    9450 5900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 6056B42F
P 1250 2025
F 0 "#PWR0101" H 1250 1775 50  0001 C CNN
F 1 "GND" H 1255 1852 50  0000 C CNN
F 2 "" H 1250 2025 50  0001 C CNN
F 3 "" H 1250 2025 50  0001 C CNN
	1    1250 2025
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0102
U 1 1 6056BFBC
P 1250 900
F 0 "#PWR0102" H 1250 750 50  0001 C CNN
F 1 "+3.3V" H 1265 1073 50  0000 C CNN
F 2 "" H 1250 900 50  0001 C CNN
F 3 "" H 1250 900 50  0001 C CNN
	1    1250 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R1
U 1 1 6056CDF1
P 1250 1000
F 0 "R1" H 1309 1046 50  0000 L CNN
F 1 "22R" H 1309 955 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" H 1250 1000 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 1250 1000 50  0001 C CNN
F 4 "RES SMD 22 OHM 5% 1/16W 0402" H 1250 1000 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 1250 1000 50  0001 C CNN "Manufacturer"
F 6 "CRCW040222R0JNED" H 1250 1000 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 1250 1000 50  0001 C CNN "Supplier"
F 8 "541-22JCT-ND" H 1250 1000 50  0001 C CNN "Supplier PN"
	1    1250 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 6056D7C9
P 1075 1175
F 0 "C1" V 846 1175 50  0000 C CNN
F 1 "1uF" V 937 1175 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" H 1075 1175 50  0001 C CNN
F 3 "http://www.samsungsem.com/kr/support/product-search/mlcc/CL05A105KQ5NNNC.jsp" H 1075 1175 50  0001 C CNN
F 4 "CAP CER 1UF 6.3V X5R 0402" H 1075 1175 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 1075 1175 50  0001 C CNN "Manufacturer"
F 6 "CL05A105KQ5NNNC" H 1075 1175 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 1075 1175 50  0001 C CNN "Supplier"
F 8 "1276-1010-1-ND" H 1075 1175 50  0001 C CNN "Supplier PN"
	1    1075 1175
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 6056F1F0
P 975 1175
F 0 "#PWR0103" H 975 925 50  0001 C CNN
F 1 "GND" H 980 1002 50  0000 C CNN
F 2 "" H 975 1175 50  0001 C CNN
F 3 "" H 975 1175 50  0001 C CNN
	1    975  1175
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 1100 1250 1175
Wire Wire Line
	1175 1175 1250 1175
Connection ~ 1250 1175
Wire Wire Line
	1250 1175 1250 1325
$Comp
L power:GND #PWR0110
U 1 1 6058952F
P 825 7575
F 0 "#PWR0110" H 825 7325 50  0001 C CNN
F 1 "GND" H 830 7402 50  0000 C CNN
F 2 "" H 825 7575 50  0001 C CNN
F 3 "" H 825 7575 50  0001 C CNN
	1    825  7575
	1    0    0    -1  
$EndComp
Wire Wire Line
	1125 7275 1700 7275
Wire Wire Line
	1700 7275 1700 7625
Wire Wire Line
	1700 7625 2125 7625
Wire Wire Line
	2125 7625 2125 7575
Wire Wire Line
	1125 7175 1700 7175
Wire Wire Line
	1700 7175 1700 6725
Wire Wire Line
	1700 6725 2125 6725
Wire Wire Line
	2125 6725 2125 6775
$Comp
L power:GND #PWR0111
U 1 1 605914DA
P 1825 7175
F 0 "#PWR0111" H 1825 6925 50  0001 C CNN
F 1 "GND" H 1830 7002 50  0000 C CNN
F 2 "" H 1825 7175 50  0001 C CNN
F 3 "" H 1825 7175 50  0001 C CNN
	1    1825 7175
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R8
U 1 1 60572A35
P 3325 2825
F 0 "R8" H 3384 2871 50  0000 L CNN
F 1 "2K" H 3384 2780 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 3325 2825 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 3325 2825 50  0001 C CNN
F 4 "RES SMD 2K OHM 5% 1/10W 0603" H 3325 2825 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 3325 2825 50  0001 C CNN "Manufacturer"
F 6 "CRCW06032K00JNEA" H 3325 2825 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3325 2825 50  0001 C CNN "Supplier"
F 8 "541-2.0KGCT-ND" H 3325 2825 50  0001 C CNN "Supplier PN"
	1    3325 2825
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 6057594F
P 3575 2125
F 0 "#PWR0112" H 3575 1875 50  0001 C CNN
F 1 "GND" H 3575 1975 50  0000 C CNN
F 2 "" H 3575 2125 50  0001 C CNN
F 3 "" H 3575 2125 50  0001 C CNN
	1    3575 2125
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 60575ED8
P 4525 3400
F 0 "#PWR0113" H 4525 3150 50  0001 C CNN
F 1 "GND" H 4530 3227 50  0000 C CNN
F 2 "" H 4525 3400 50  0001 C CNN
F 3 "" H 4525 3400 50  0001 C CNN
	1    4525 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3925 1900 3575 1900
Wire Wire Line
	3575 1900 3575 1925
$Comp
L Device:C_Small C5
U 1 1 6057751B
P 4900 1200
F 0 "C5" H 4992 1246 50  0000 L CNN
F 1 "0.1uF" H 4992 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4900 1200 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 4900 1200 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 4900 1200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 4900 1200 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 4900 1200 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 4900 1200 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 4900 1200 50  0001 C CNN "Supplier PN"
	1    4900 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 60578203
P 3625 1200
F 0 "C3" H 3717 1246 50  0000 L CNN
F 1 "0.1uF" H 3717 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3625 1200 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 3625 1200 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 3625 1200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 3625 1200 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 3625 1200 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3625 1200 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 3625 1200 50  0001 C CNN "Supplier PN"
	1    3625 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 605786B2
P 3225 1200
F 0 "C2" H 3317 1246 50  0000 L CNN
F 1 "10uF" H 3317 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3225 1200 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 3225 1200 50  0001 C CNN
F 4 "CAP CER 10UF 25V X5R 0805" H 3225 1200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 3225 1200 50  0001 C CNN "Manufacturer"
F 6 "CL21A106KAYNNNG" H 3225 1200 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3225 1200 50  0001 C CNN "Supplier"
F 8 "1276-6454-1-ND" H 3225 1200 50  0001 C CNN "Supplier PN"
	1    3225 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 6057AEB2
P 3225 1350
F 0 "#PWR0114" H 3225 1100 50  0001 C CNN
F 1 "GND" H 3230 1177 50  0000 C CNN
F 2 "" H 3225 1350 50  0001 C CNN
F 3 "" H 3225 1350 50  0001 C CNN
	1    3225 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0115
U 1 1 6057BC1B
P 3225 1050
F 0 "#PWR0115" H 3225 900 50  0001 C CNN
F 1 "+3.3V" H 3240 1223 50  0000 C CNN
F 2 "" H 3225 1050 50  0001 C CNN
F 3 "" H 3225 1050 50  0001 C CNN
	1    3225 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3225 1050 3225 1100
Wire Wire Line
	3225 1300 3225 1350
Wire Wire Line
	3625 1100 3225 1100
Connection ~ 3225 1100
Wire Wire Line
	3625 1300 3225 1300
Connection ~ 3225 1300
Wire Wire Line
	4000 1300 3625 1300
Connection ~ 3625 1300
Connection ~ 3625 1100
Wire Wire Line
	4525 1100 4425 1100
$Comp
L Device:C_Small C8
U 1 1 605806B5
P 3225 1850
F 0 "C8" H 3317 1896 50  0000 L CNN
F 1 "0.1uF" H 3317 1805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3225 1850 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 3225 1850 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 3225 1850 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 3225 1850 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 3225 1850 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3225 1850 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 3225 1850 50  0001 C CNN "Supplier PN"
	1    3225 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C6
U 1 1 60580956
P 5300 1200
F 0 "C6" H 5392 1246 50  0000 L CNN
F 1 "1uF" H 5392 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5300 1200 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 5300 1200 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 5300 1200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 5300 1200 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 5300 1200 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 5300 1200 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 5300 1200 50  0001 C CNN "Supplier PN"
	1    5300 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 6058365E
P 4900 1350
F 0 "#PWR0116" H 4900 1100 50  0001 C CNN
F 1 "GND" H 4905 1177 50  0000 C CNN
F 2 "" H 4900 1350 50  0001 C CNN
F 3 "" H 4900 1350 50  0001 C CNN
	1    4900 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0117
U 1 1 60583ECA
P 5425 875
F 0 "#PWR0117" H 5425 725 50  0001 C CNN
F 1 "+3.3V" H 5440 1048 50  0000 C CNN
F 2 "" H 5425 875 50  0001 C CNN
F 3 "" H 5425 875 50  0001 C CNN
	1    5425 875 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4525 1100 4525 1500
Wire Wire Line
	4425 1500 4425 1100
Connection ~ 4425 1100
Wire Wire Line
	4625 1500 4625 1100
Wire Wire Line
	4625 1100 4900 1100
Wire Wire Line
	4900 1100 5300 1100
Connection ~ 4900 1100
Wire Wire Line
	4900 1350 4900 1300
Wire Wire Line
	4900 1300 5300 1300
Wire Wire Line
	4900 1100 4900 950 
Wire Wire Line
	5200 950  5425 950 
Wire Wire Line
	5425 950  5425 875 
$Comp
L power:GND #PWR0118
U 1 1 60593A28
P 3225 1950
F 0 "#PWR0118" H 3225 1700 50  0001 C CNN
F 1 "GND" H 3230 1777 50  0000 C CNN
F 2 "" H 3225 1950 50  0001 C CNN
F 3 "" H 3225 1950 50  0001 C CNN
	1    3225 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3925 1700 3225 1700
Wire Wire Line
	3225 1700 3225 1750
Text Label 2975 1700 0    50   ~ 0
RESET
Wire Wire Line
	2975 1700 3225 1700
Connection ~ 3225 1700
Connection ~ 4900 1300
Text Label 5475 2900 2    50   ~ 0
USB_D+
Text Label 5475 2800 2    50   ~ 0
USB_D-
Wire Wire Line
	5025 2800 5475 2800
Wire Wire Line
	3625 1100 4000 1100
Wire Wire Line
	5025 2900 5475 2900
Text Label 5475 3100 2    50   ~ 0
SWCLK
Wire Wire Line
	5025 3100 5475 3100
Wire Wire Line
	5475 3000 5025 3000
Text Label 3525 3100 0    50   ~ 0
I2C1_SDA
Text Label 3525 3000 0    50   ~ 0
I2C1_SCL
Text Label 5475 2700 2    50   ~ 0
USART1_RX
Text Label 5475 2600 2    50   ~ 0
USART1_TX
Wire Wire Line
	5475 2700 5025 2700
Wire Wire Line
	5475 2600 5025 2600
$Comp
L Device:R_Small R7
U 1 1 605A5840
P 3075 2825
F 0 "R7" H 3134 2871 50  0000 L CNN
F 1 "2K" H 3134 2780 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 3075 2825 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 3075 2825 50  0001 C CNN
F 4 "RES SMD 2K OHM 5% 1/10W 0603" H 3075 2825 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 3075 2825 50  0001 C CNN "Manufacturer"
F 6 "CRCW06032K00JNEA" H 3075 2825 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3075 2825 50  0001 C CNN "Supplier"
F 8 "541-2.0KGCT-ND" H 3075 2825 50  0001 C CNN "Supplier PN"
	1    3075 2825
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0120
U 1 1 605A5D9E
P 3075 2675
F 0 "#PWR0120" H 3075 2525 50  0001 C CNN
F 1 "+3.3V" H 3090 2848 50  0000 C CNN
F 2 "" H 3075 2675 50  0001 C CNN
F 3 "" H 3075 2675 50  0001 C CNN
	1    3075 2675
	1    0    0    -1  
$EndComp
Wire Wire Line
	3325 3000 3325 2925
Wire Wire Line
	3325 3000 3925 3000
Wire Wire Line
	3075 3100 3075 2925
Wire Wire Line
	3075 3100 3925 3100
Wire Wire Line
	3075 2725 3075 2675
Wire Wire Line
	3325 2725 3325 2675
Wire Wire Line
	3325 2675 3075 2675
Connection ~ 3075 2675
$Comp
L Device:R_Small R6
U 1 1 605AAD1E
P 3575 2025
F 0 "R6" H 3634 2071 50  0000 L CNN
F 1 "10K" H 3634 1980 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 3575 2025 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 3575 2025 50  0001 C CNN
F 4 "RES SMD 10K OHM 5% 1/10W 0603" H 3575 2025 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 3575 2025 50  0001 C CNN "Manufacturer"
F 6 "CRCW060310K0JNEA" H 3575 2025 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3575 2025 50  0001 C CNN "Supplier"
F 8 "541-10KGCT-ND" H 3575 2025 50  0001 C CNN "Supplier PN"
	1    3575 2025
	1    0    0    -1  
$EndComp
Text Label 5475 3000 2    50   ~ 0
SWDIO
Text Label 2175 1675 2    50   ~ 0
I2C1_SCL
Text Label 2175 1825 2    50   ~ 0
I2C1_SDA
Wire Wire Line
	1750 1825 2175 1825
Wire Wire Line
	1750 1675 2175 1675
$Comp
L Project:AP2502 U3
U 1 1 60581108
P 1325 3150
F 0 "U3" H 1325 3567 50  0000 C CNN
F 1 "AP2502" H 1325 3476 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 1375 2850 50  0001 L CNN
F 3 "https://www.diodes.com/assets/Datasheets/AP2502.pdf" H 1225 2750 50  0001 C CNN
F 4 "IC LED DRVR LIN DIM 20MA SOT23-6" H 1325 3150 50  0001 C CNN "Description"
F 5 "Diodes Incorporated" H 1325 3150 50  0001 C CNN "Manufacturer"
F 6 "AP2502KTR-G1" H 1325 3150 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 1325 3150 50  0001 C CNN "Supplier"
F 8 "AP2502KTR-G1DICT-ND" H 1325 3150 50  0001 C CNN "Supplier PN"
	1    1325 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 3000 1725 3000
Wire Wire Line
	2000 3250 1900 3250
Wire Wire Line
	1900 3250 1900 3100
Wire Wire Line
	1900 3100 1725 3100
Wire Wire Line
	2000 3500 1850 3500
Wire Wire Line
	1850 3500 1850 3200
Wire Wire Line
	1850 3200 1725 3200
Wire Wire Line
	2000 3750 1800 3750
Wire Wire Line
	1800 3750 1800 3300
Wire Wire Line
	1800 3300 1725 3300
$Comp
L Device:C_Small C10
U 1 1 60593A12
P 2525 3125
F 0 "C10" H 2617 3171 50  0000 L CNN
F 1 "0.1uF" H 2617 3080 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2525 3125 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 2525 3125 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 2525 3125 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 2525 3125 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 2525 3125 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2525 3125 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 2525 3125 50  0001 C CNN "Supplier PN"
	1    2525 3125
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 3750 2300 3500
Wire Wire Line
	2300 3500 2300 3250
Connection ~ 2300 3500
Wire Wire Line
	2300 3250 2300 3000
Connection ~ 2300 3250
Wire Wire Line
	2525 2975 2525 3000
Wire Wire Line
	2300 3000 2525 3000
Connection ~ 2300 3000
Connection ~ 2525 3000
Wire Wire Line
	2525 3000 2525 3025
$Comp
L power:GND #PWR0125
U 1 1 605A29E2
P 2525 3225
F 0 "#PWR0125" H 2525 2975 50  0001 C CNN
F 1 "GND" H 2530 3052 50  0000 C CNN
F 2 "" H 2525 3225 50  0001 C CNN
F 3 "" H 2525 3225 50  0001 C CNN
	1    2525 3225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0126
U 1 1 605A2DF9
P 1325 3500
F 0 "#PWR0126" H 1325 3250 50  0001 C CNN
F 1 "GND" H 1330 3327 50  0000 C CNN
F 2 "" H 1325 3500 50  0001 C CNN
F 3 "" H 1325 3500 50  0001 C CNN
	1    1325 3500
	1    0    0    -1  
$EndComp
Text Label 3300 7325 2    50   ~ 0
USB_D-
Text Label 3300 7125 2    50   ~ 0
USB_D+
Wire Wire Line
	2325 7575 2325 7625
Wire Wire Line
	2325 7625 2925 7625
Wire Wire Line
	2325 6775 2325 6725
Wire Wire Line
	2325 6725 2925 6725
Text Label 5475 1900 2    50   ~ 0
RLED_EN
Text Label 575  3000 0    50   ~ 0
RLED_EN
Wire Wire Line
	5025 1900 5475 1900
Wire Wire Line
	575  3000 925  3000
$Comp
L Project:ESDALC6V1-1U2 D5
U 1 1 605CB7E3
P 9325 1275
F 0 "D5" V 9279 1353 50  0000 L CNN
F 1 "ESDALC6V1-1U2" V 9370 1353 50  0000 L CNN
F 2 "lib_fp:ST0201" H 9325 1075 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/90/b1/aa/a3/9f/73/4d/1e/CD00212838.pdf/files/CD00212838.pdf/jcr:content/translations/en.CD00212838.pdf" H 9325 1325 50  0001 C CNN
F 4 "TVS DIODE 3V ST0201" H 9325 1275 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 9325 1275 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1-1U2" H 9325 1275 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9325 1275 50  0001 C CNN "Supplier"
F 8 "497-8752-1-ND" H 9325 1275 50  0001 C CNN "Supplier PN"
	1    9325 1275
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R3
U 1 1 605D1A9B
P 9000 1275
F 0 "R3" H 9059 1321 50  0000 L CNN
F 1 "100K" H 9059 1230 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9000 1275 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9000 1275 50  0001 C CNN
F 4 "RES SMD 100K OHM 5% 1/10W 0603" H 9000 1275 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9000 1275 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603100KJNEA" H 9000 1275 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9000 1275 50  0001 C CNN "Supplier"
F 8 "541-100KGCT-ND" H 9000 1275 50  0001 C CNN "Supplier PN"
	1    9000 1275
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R2
U 1 1 605D25A4
P 9750 1075
F 0 "R2" H 9809 1121 50  0000 L CNN
F 1 "330R" H 9809 1030 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9750 1075 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9750 1075 50  0001 C CNN
F 4 "RES SMD 330 OHM 5% 1/10W 0603" H 9750 1075 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9750 1075 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603330RJNEA" H 9750 1075 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9750 1075 50  0001 C CNN "Supplier"
F 8 "541-330GCT-ND" H 9750 1075 50  0001 C CNN "Supplier PN"
	1    9750 1075
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C7
U 1 1 605D366F
P 10125 1275
F 0 "C7" H 10217 1321 50  0000 L CNN
F 1 "0.1uF" H 10217 1230 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10125 1275 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 10125 1275 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 10125 1275 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 10125 1275 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 10125 1275 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 10125 1275 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 10125 1275 50  0001 C CNN "Supplier PN"
	1    10125 1275
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 1075 9000 1075
Connection ~ 9325 1075
Wire Wire Line
	9325 1075 9650 1075
Wire Wire Line
	9000 1175 9000 1075
Connection ~ 9000 1075
Wire Wire Line
	9000 1075 9325 1075
$Comp
L power:GND #PWR0128
U 1 1 605DA6BA
P 9000 1475
F 0 "#PWR0128" H 9000 1225 50  0001 C CNN
F 1 "GND" H 9005 1302 50  0000 C CNN
F 2 "" H 9000 1475 50  0001 C CNN
F 3 "" H 9000 1475 50  0001 C CNN
	1    9000 1475
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 1375 9000 1475
Wire Wire Line
	10125 1375 10125 1475
Wire Wire Line
	10125 1475 9325 1475
Wire Wire Line
	9325 1475 9000 1475
Connection ~ 9325 1475
Wire Wire Line
	10125 1075 10125 1175
$Comp
L power:+3.3V #PWR0129
U 1 1 605E701C
P 8425 950
F 0 "#PWR0129" H 8425 800 50  0001 C CNN
F 1 "+3.3V" H 8440 1123 50  0000 C CNN
F 2 "" H 8425 950 50  0001 C CNN
F 3 "" H 8425 950 50  0001 C CNN
	1    8425 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8425 950  8425 1075
Wire Wire Line
	8425 1075 8500 1075
Text Label 10400 1075 2    50   ~ 0
BTN1
Wire Wire Line
	10125 1075 10400 1075
Text Label 10400 1775 2    50   ~ 0
BTN2
Text Label 3525 2900 0    50   ~ 0
BTN1
Wire Wire Line
	3525 2900 3925 2900
Text Label 3525 2800 0    50   ~ 0
BTN2
Wire Wire Line
	3525 2800 3925 2800
$Comp
L Device:R_Small R5
U 1 1 6060584E
P 9000 1975
F 0 "R5" H 9059 2021 50  0000 L CNN
F 1 "100K" H 9059 1930 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9000 1975 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9000 1975 50  0001 C CNN
F 4 "RES SMD 100K OHM 5% 1/10W 0603" H 9000 1975 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9000 1975 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603100KJNEA" H 9000 1975 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9000 1975 50  0001 C CNN "Supplier"
F 8 "541-100KGCT-ND" H 9000 1975 50  0001 C CNN "Supplier PN"
	1    9000 1975
	1    0    0    -1  
$EndComp
$Comp
L Project:ESDALC6V1-1U2 D6
U 1 1 60605E37
P 9325 1975
F 0 "D6" V 9279 2053 50  0000 L CNN
F 1 "ESDALC6V1-1U2" V 9370 2053 50  0000 L CNN
F 2 "lib_fp:ST0201" H 9325 1775 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/90/b1/aa/a3/9f/73/4d/1e/CD00212838.pdf/files/CD00212838.pdf/jcr:content/translations/en.CD00212838.pdf" H 9325 2025 50  0001 C CNN
F 4 "TVS DIODE 3V ST0201" H 9325 1975 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 9325 1975 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1-1U2" H 9325 1975 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9325 1975 50  0001 C CNN "Supplier"
F 8 "497-8752-1-ND" H 9325 1975 50  0001 C CNN "Supplier PN"
	1    9325 1975
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C9
U 1 1 60606949
P 10125 1975
F 0 "C9" H 10217 2021 50  0000 L CNN
F 1 "0.1uF" H 10217 1930 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10125 1975 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 10125 1975 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 10125 1975 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 10125 1975 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 10125 1975 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 10125 1975 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 10125 1975 50  0001 C CNN "Supplier PN"
	1    10125 1975
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R4
U 1 1 60606EB7
P 9750 1775
F 0 "R4" H 9809 1821 50  0000 L CNN
F 1 "330R" H 9809 1730 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9750 1775 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9750 1775 50  0001 C CNN
F 4 "RES SMD 330 OHM 5% 1/10W 0603" H 9750 1775 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9750 1775 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603330RJNEA" H 9750 1775 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9750 1775 50  0001 C CNN "Supplier"
F 8 "541-330GCT-ND" H 9750 1775 50  0001 C CNN "Supplier PN"
	1    9750 1775
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR0130
U 1 1 6061D995
P 8425 1650
F 0 "#PWR0130" H 8425 1500 50  0001 C CNN
F 1 "+3.3V" H 8440 1823 50  0000 C CNN
F 2 "" H 8425 1650 50  0001 C CNN
F 3 "" H 8425 1650 50  0001 C CNN
	1    8425 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8425 1650 8425 1775
Wire Wire Line
	8425 1775 8500 1775
Wire Wire Line
	8900 1775 9000 1775
Connection ~ 9325 1775
Wire Wire Line
	9325 1775 9650 1775
Wire Wire Line
	9850 1775 10125 1775
Wire Wire Line
	10125 1875 10125 1775
Connection ~ 10125 1775
Wire Wire Line
	10125 1775 10400 1775
Wire Wire Line
	9000 1875 9000 1775
Connection ~ 9000 1775
Wire Wire Line
	9000 1775 9325 1775
$Comp
L power:GND #PWR0131
U 1 1 60641ACC
P 9000 2175
F 0 "#PWR0131" H 9000 1925 50  0001 C CNN
F 1 "GND" H 9005 2002 50  0000 C CNN
F 2 "" H 9000 2175 50  0001 C CNN
F 3 "" H 9000 2175 50  0001 C CNN
	1    9000 2175
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 2075 9000 2175
Wire Wire Line
	9000 2175 9325 2175
Wire Wire Line
	9325 2175 10125 2175
Wire Wire Line
	10125 2175 10125 2075
Connection ~ 9325 2175
NoConn ~ 725  7575
NoConn ~ 1750 1525
NoConn ~ 3925 2100
NoConn ~ 3925 2200
NoConn ~ 3925 3200
NoConn ~ 3925 2500
NoConn ~ 3925 2600
NoConn ~ 5025 1700
NoConn ~ 5025 1800
NoConn ~ 1125 7375
Wire Wire Line
	1525 6975 1525 6625
Wire Wire Line
	1525 6625 2675 6625
Wire Wire Line
	2675 6625 2675 7175
Wire Wire Line
	2675 7175 2625 7175
Connection ~ 2675 6625
$Comp
L Device:C_Small C4
U 1 1 6060A6DB
P 4000 1200
F 0 "C4" H 4092 1246 50  0000 L CNN
F 1 "0.1uF" H 4092 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4000 1200 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 4000 1200 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 4000 1200 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 4000 1200 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 4000 1200 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 4000 1200 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 4000 1200 50  0001 C CNN "Supplier PN"
	1    4000 1200
	1    0    0    -1  
$EndComp
Connection ~ 4000 1100
Wire Wire Line
	4000 1100 4425 1100
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 6060C0D7
P 4625 1100
F 0 "#FLG0101" H 4625 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 4625 1273 50  0001 C CNN
F 2 "" H 4625 1100 50  0001 C CNN
F 3 "~" H 4625 1100 50  0001 C CNN
	1    4625 1100
	1    0    0    -1  
$EndComp
Connection ~ 4625 1100
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 6060CBDC
P 1250 1175
F 0 "#FLG0102" H 1250 1250 50  0001 C CNN
F 1 "PWR_FLAG" V 1250 1303 50  0001 L CNN
F 2 "" H 1250 1175 50  0001 C CNN
F 3 "~" H 1250 1175 50  0001 C CNN
	1    1250 1175
	0    1    1    0   
$EndComp
Wire Wire Line
	2925 6725 2925 7125
Wire Wire Line
	2925 7125 3300 7125
Wire Wire Line
	2925 7625 2925 7325
Wire Wire Line
	2925 7325 3300 7325
Text Notes 600  675  0    50   ~ 0
Light Sensor
Text Notes 600  2650 0    50   ~ 0
Illumination LEDs
Wire Notes Line
	550  550  550  2450
Wire Notes Line
	550  2450 2225 2450
Wire Notes Line
	2225 2450 2225 550 
Wire Notes Line
	2225 550  550  550 
Wire Notes Line
	550  2525 2875 2525
Wire Notes Line
	2875 2525 2875 5025
Wire Notes Line
	2875 5025 550  5025
Wire Notes Line
	550  5025 550  2525
Text Notes 8325 675  0    50   ~ 0
User Buttons
Wire Notes Line
	8275 550  8275 3825
Wire Notes Line
	8275 3825 10450 3825
Wire Notes Line
	10450 3825 10450 550 
Wire Notes Line
	10450 550  8275 550 
Text Notes 3025 700  0    50   ~ 0
Microcontroller
Wire Notes Line
	2950 550  2950 3625
Wire Notes Line
	5575 3625 5575 550 
Wire Notes Line
	5575 550  2950 550 
Wire Notes Line
	2950 3625 5575 3625
Text Label 1125 7175 0    50   ~ 0
USB_CONN_D+
Text Label 1125 7275 0    50   ~ 0
USB_CONN_D-
$Comp
L power:+5V #PWR013
U 1 1 60629D8A
P 2525 2975
F 0 "#PWR013" H 2525 2825 50  0001 C CNN
F 1 "+5V" H 2540 3148 50  0000 C CNN
F 2 "" H 2525 2975 50  0001 C CNN
F 3 "" H 2525 2975 50  0001 C CNN
	1    2525 2975
	1    0    0    -1  
$EndComp
$Comp
L Project:TSL2591 U1
U 1 1 60B806BE
P 1250 1625
F 0 "U1" H 925 1625 50  0000 R CNN
F 1 "TSL2591" H 925 1525 50  0000 R CNN
F 2 "lib_fp:AMS_TSL2591" H 1250 1125 50  0001 C CNN
F 3 "http://ams.com/documents/20143/36005/TSL2591_DS000338_5-00.pdf" H 2000 1925 50  0001 C CNN
F 4 "SENSOR OPT AMBIENT 6DFN" H 1250 1625 50  0001 C CNN "Description"
F 5 "ams" H 1250 1625 50  0001 C CNN "Manufacturer"
F 6 "TSL25911FN" H 1250 1625 50  0001 C CNN "Manufacturer PN"
F 7 "Mouser" H 1250 1625 50  0001 C CNN "Supplier"
F 8 "985-TSL25911FN" H 1250 1625 50  0001 C CNN "Supplier PN"
	1    1250 1625
	1    0    0    -1  
$EndComp
$Comp
L Project:AP2502 U6
U 1 1 60BB1317
P 1325 4400
F 0 "U6" H 1325 4817 50  0000 C CNN
F 1 "AP2502" H 1325 4726 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 1375 4100 50  0001 L CNN
F 3 "https://www.diodes.com/assets/Datasheets/AP2502.pdf" H 1225 4000 50  0001 C CNN
F 4 "IC LED DRVR LIN DIM 20MA SOT23-6" H 1325 4400 50  0001 C CNN "Description"
F 5 "Diodes Incorporated" H 1325 4400 50  0001 C CNN "Manufacturer"
F 6 "AP2502KTR-G1" H 1325 4400 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 1325 4400 50  0001 C CNN "Supplier"
F 8 "AP2502KTR-G1DICT-ND" H 1325 4400 50  0001 C CNN "Supplier PN"
	1    1325 4400
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 60BB2239
P 2175 4200
F 0 "J4" H 2255 4192 50  0000 L CNN
F 1 "Conn_01x02" H 1950 4000 50  0001 L CNN
F 2 "Connector_JST:JST_SH_BM02B-SRSS-TB_1x02-1MP_P1.00mm_Vertical" H 2175 4200 50  0001 C CNN
F 3 "http://www.jst-mfg.com/product/pdf/eng/ePH.pdf" H 2175 4200 50  0001 C CNN
F 4 "Trans LED" H 2175 4000 50  0000 C CNN "User Label"
F 5 "CONN HEADER SMD 2POS 1MM" H 2175 4200 50  0001 C CNN "Description"
F 6 "JST Sales America Inc." H 2175 4200 50  0001 C CNN "Manufacturer"
F 7 "BM02B-SRSS-TB(LF)(SN)" H 2175 4200 50  0001 C CNN "Manufacturer PN"
F 8 "Digi-Key" H 2175 4200 50  0001 C CNN "Supplier"
F 9 "455-1788-1-ND" H 2175 4200 50  0001 C CNN "Supplier PN"
	1    2175 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C22
U 1 1 60BB3A94
P 2525 4225
F 0 "C22" H 2617 4271 50  0000 L CNN
F 1 "0.1uF" H 2617 4180 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2525 4225 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 2525 4225 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 2525 4225 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 2525 4225 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 2525 4225 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2525 4225 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 2525 4225 50  0001 C CNN "Supplier PN"
	1    2525 4225
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 60BB4FA6
P 1325 4750
F 0 "#PWR020" H 1325 4500 50  0001 C CNN
F 1 "GND" H 1330 4577 50  0000 C CNN
F 2 "" H 1325 4750 50  0001 C CNN
F 3 "" H 1325 4750 50  0001 C CNN
	1    1325 4750
	1    0    0    -1  
$EndComp
NoConn ~ 1725 4450
NoConn ~ 1725 4550
$Comp
L power:GND #PWR019
U 1 1 60BD1EDA
P 2525 4325
F 0 "#PWR019" H 2525 4075 50  0001 C CNN
F 1 "GND" H 2530 4152 50  0000 C CNN
F 2 "" H 2525 4325 50  0001 C CNN
F 3 "" H 2525 4325 50  0001 C CNN
	1    2525 4325
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR017
U 1 1 60BD22F4
P 2525 4075
F 0 "#PWR017" H 2525 3925 50  0001 C CNN
F 1 "+5V" H 2540 4248 50  0000 C CNN
F 2 "" H 2525 4075 50  0001 C CNN
F 3 "" H 2525 4075 50  0001 C CNN
	1    2525 4075
	1    0    0    -1  
$EndComp
Wire Wire Line
	2525 4075 2525 4100
Wire Wire Line
	2525 4100 1975 4100
Wire Wire Line
	1975 4100 1975 4200
Connection ~ 2525 4100
Wire Wire Line
	2525 4100 2525 4125
Text Label 575  4250 0    50   ~ 0
TLED_EN
Wire Wire Line
	575  4250 925  4250
Wire Wire Line
	1725 4250 1925 4250
$Comp
L Switch:SW_Push SW3
U 1 1 60C5021A
P 8700 2475
F 0 "SW3" H 8700 2650 50  0000 C CNN
F 1 "SW_Push" H 8700 2400 50  0000 C CNN
F 2 "lib_fp:ESwitch_TL1105_6mm_H7.5mm" H 8700 2675 50  0001 C CNN
F 3 "https://www.e-switch.com/system/asset/product_line/data_sheet/144/TL1105.pdf" H 8700 2675 50  0001 C CNN
F 4 "SWITCH TACTILE SPST-NO 0.05A 12V" H 8700 2475 50  0001 C CNN "Description"
F 5 "E-Switch" H 8700 2475 50  0001 C CNN "Manufacturer"
F 6 "TL1105SPF160Q" H 8700 2475 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 8700 2475 50  0001 C CNN "Supplier"
F 8 "EG1861-ND" H 8700 2475 50  0001 C CNN "Supplier PN"
	1    8700 2475
	1    0    0    -1  
$EndComp
Text Label 10400 2475 2    50   ~ 0
BTN3
$Comp
L Device:R_Small R11
U 1 1 60C508C6
P 9000 2675
F 0 "R11" H 9059 2721 50  0000 L CNN
F 1 "100K" H 9059 2630 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9000 2675 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9000 2675 50  0001 C CNN
F 4 "RES SMD 100K OHM 5% 1/10W 0603" H 9000 2675 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9000 2675 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603100KJNEA" H 9000 2675 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9000 2675 50  0001 C CNN "Supplier"
F 8 "541-100KGCT-ND" H 9000 2675 50  0001 C CNN "Supplier PN"
	1    9000 2675
	1    0    0    -1  
$EndComp
$Comp
L Project:ESDALC6V1-1U2 D9
U 1 1 60C508D5
P 9325 2675
F 0 "D9" V 9279 2753 50  0000 L CNN
F 1 "ESDALC6V1-1U2" V 9370 2753 50  0000 L CNN
F 2 "lib_fp:ST0201" H 9325 2475 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/90/b1/aa/a3/9f/73/4d/1e/CD00212838.pdf/files/CD00212838.pdf/jcr:content/translations/en.CD00212838.pdf" H 9325 2725 50  0001 C CNN
F 4 "TVS DIODE 3V ST0201" H 9325 2675 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 9325 2675 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1-1U2" H 9325 2675 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9325 2675 50  0001 C CNN "Supplier"
F 8 "497-8752-1-ND" H 9325 2675 50  0001 C CNN "Supplier PN"
	1    9325 2675
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C18
U 1 1 60C508E4
P 10125 2675
F 0 "C18" H 10217 2721 50  0000 L CNN
F 1 "0.1uF" H 10217 2630 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10125 2675 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 10125 2675 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 10125 2675 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 10125 2675 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 10125 2675 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 10125 2675 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 10125 2675 50  0001 C CNN "Supplier PN"
	1    10125 2675
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R9
U 1 1 60C508F3
P 9750 2475
F 0 "R9" H 9809 2521 50  0000 L CNN
F 1 "330R" H 9809 2430 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9750 2475 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9750 2475 50  0001 C CNN
F 4 "RES SMD 330 OHM 5% 1/10W 0603" H 9750 2475 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9750 2475 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603330RJNEA" H 9750 2475 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9750 2475 50  0001 C CNN "Supplier"
F 8 "541-330GCT-ND" H 9750 2475 50  0001 C CNN "Supplier PN"
	1    9750 2475
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR06
U 1 1 60C508FD
P 8425 2350
F 0 "#PWR06" H 8425 2200 50  0001 C CNN
F 1 "+3.3V" H 8440 2523 50  0000 C CNN
F 2 "" H 8425 2350 50  0001 C CNN
F 3 "" H 8425 2350 50  0001 C CNN
	1    8425 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8425 2350 8425 2475
Wire Wire Line
	8425 2475 8500 2475
Wire Wire Line
	8900 2475 9000 2475
Connection ~ 9325 2475
Wire Wire Line
	9325 2475 9650 2475
Wire Wire Line
	9850 2475 10125 2475
Wire Wire Line
	10125 2575 10125 2475
Connection ~ 10125 2475
Wire Wire Line
	10125 2475 10400 2475
Wire Wire Line
	9000 2575 9000 2475
Connection ~ 9000 2475
Wire Wire Line
	9000 2475 9325 2475
$Comp
L power:GND #PWR012
U 1 1 60C50913
P 9000 2875
F 0 "#PWR012" H 9000 2625 50  0001 C CNN
F 1 "GND" H 9005 2702 50  0000 C CNN
F 2 "" H 9000 2875 50  0001 C CNN
F 3 "" H 9000 2875 50  0001 C CNN
	1    9000 2875
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 2775 9000 2875
Wire Wire Line
	9000 2875 9325 2875
Wire Wire Line
	9325 2875 10125 2875
Wire Wire Line
	10125 2875 10125 2775
Connection ~ 9325 2875
$Comp
L Switch:SW_Push SW4
U 1 1 60C6EB5A
P 8700 3175
F 0 "SW4" H 8700 3350 50  0000 C CNN
F 1 "SW_Push" H 8700 3100 50  0000 C CNN
F 2 "lib_fp:ESwitch_TL1105_6mm_H7.5mm" H 8700 3375 50  0001 C CNN
F 3 "https://www.e-switch.com/system/asset/product_line/data_sheet/144/TL1105.pdf" H 8700 3375 50  0001 C CNN
F 4 "SWITCH TACTILE SPST-NO 0.05A 12V" H 8700 3175 50  0001 C CNN "Description"
F 5 "E-Switch" H 8700 3175 50  0001 C CNN "Manufacturer"
F 6 "TL1105SPF160Q" H 8700 3175 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 8700 3175 50  0001 C CNN "Supplier"
F 8 "EG1861-ND" H 8700 3175 50  0001 C CNN "Supplier PN"
	1    8700 3175
	1    0    0    -1  
$EndComp
Text Label 10400 3175 2    50   ~ 0
BTN4
$Comp
L Device:R_Small R13
U 1 1 60C6F282
P 9000 3375
F 0 "R13" H 9059 3421 50  0000 L CNN
F 1 "100K" H 9059 3330 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9000 3375 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9000 3375 50  0001 C CNN
F 4 "RES SMD 100K OHM 5% 1/10W 0603" H 9000 3375 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9000 3375 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603100KJNEA" H 9000 3375 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9000 3375 50  0001 C CNN "Supplier"
F 8 "541-100KGCT-ND" H 9000 3375 50  0001 C CNN "Supplier PN"
	1    9000 3375
	1    0    0    -1  
$EndComp
$Comp
L Project:ESDALC6V1-1U2 D10
U 1 1 60C6F291
P 9325 3375
F 0 "D10" V 9279 3453 50  0000 L CNN
F 1 "ESDALC6V1-1U2" V 9370 3453 50  0000 L CNN
F 2 "lib_fp:ST0201" H 9325 3175 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/90/b1/aa/a3/9f/73/4d/1e/CD00212838.pdf/files/CD00212838.pdf/jcr:content/translations/en.CD00212838.pdf" H 9325 3425 50  0001 C CNN
F 4 "TVS DIODE 3V ST0201" H 9325 3375 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 9325 3375 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1-1U2" H 9325 3375 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9325 3375 50  0001 C CNN "Supplier"
F 8 "497-8752-1-ND" H 9325 3375 50  0001 C CNN "Supplier PN"
	1    9325 3375
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C21
U 1 1 60C6F2A0
P 10125 3375
F 0 "C21" H 10217 3421 50  0000 L CNN
F 1 "0.1uF" H 10217 3330 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10125 3375 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 10125 3375 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 10125 3375 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 10125 3375 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 10125 3375 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 10125 3375 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 10125 3375 50  0001 C CNN "Supplier PN"
	1    10125 3375
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R12
U 1 1 60C6F2AF
P 9750 3175
F 0 "R12" H 9809 3221 50  0000 L CNN
F 1 "330R" H 9809 3130 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9750 3175 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9750 3175 50  0001 C CNN
F 4 "RES SMD 330 OHM 5% 1/10W 0603" H 9750 3175 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9750 3175 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603330RJNEA" H 9750 3175 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9750 3175 50  0001 C CNN "Supplier"
F 8 "541-330GCT-ND" H 9750 3175 50  0001 C CNN "Supplier PN"
	1    9750 3175
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR014
U 1 1 60C6F2B9
P 8425 3050
F 0 "#PWR014" H 8425 2900 50  0001 C CNN
F 1 "+3.3V" H 8440 3223 50  0000 C CNN
F 2 "" H 8425 3050 50  0001 C CNN
F 3 "" H 8425 3050 50  0001 C CNN
	1    8425 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8425 3050 8425 3175
Wire Wire Line
	8425 3175 8500 3175
Wire Wire Line
	8900 3175 9000 3175
Connection ~ 9325 3175
Wire Wire Line
	9325 3175 9650 3175
Wire Wire Line
	9850 3175 10125 3175
Wire Wire Line
	10125 3275 10125 3175
Connection ~ 10125 3175
Wire Wire Line
	10125 3175 10400 3175
Wire Wire Line
	9000 3275 9000 3175
Connection ~ 9000 3175
Wire Wire Line
	9000 3175 9325 3175
$Comp
L power:GND #PWR016
U 1 1 60C6F2CF
P 9000 3575
F 0 "#PWR016" H 9000 3325 50  0001 C CNN
F 1 "GND" H 9005 3402 50  0000 C CNN
F 2 "" H 9000 3575 50  0001 C CNN
F 3 "" H 9000 3575 50  0001 C CNN
	1    9000 3575
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 3475 9000 3575
Wire Wire Line
	9000 3575 9325 3575
Wire Wire Line
	9325 3575 10125 3575
Wire Wire Line
	10125 3575 10125 3475
Connection ~ 9325 3575
Connection ~ 9000 1475
Connection ~ 9000 2175
Connection ~ 9000 2875
Connection ~ 9000 3575
$Comp
L Switch:SW_Push SW5
U 1 1 60CA5C6C
P 8700 4400
F 0 "SW5" H 8700 4575 50  0000 C CNN
F 1 "SW_Detect" H 8700 4325 50  0000 C CNN
F 2 "lib_fp:CK_KSU213WLFG" H 8700 4600 50  0001 C CNN
F 3 "https://www.ckswitches.com/media/1312/ksu.pdf" H 8700 4600 50  0001 C CNN
F 4 "SWITCH DETECTOR SPST-NO 10MA 50V" H 8700 4400 50  0001 C CNN "Description"
F 5 "C&K" H 8700 4400 50  0001 C CNN "Manufacturer"
F 6 "KSU213WLFG" H 8700 4400 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 8700 4400 50  0001 C CNN "Supplier"
F 8 "401-1598-1-ND" H 8700 4400 50  0001 C CNN "Supplier PN"
	1    8700 4400
	1    0    0    -1  
$EndComp
Text Label 10400 4400 2    50   ~ 0
BTN5
$Comp
L Device:R_Small R15
U 1 1 60CA6410
P 9000 4600
F 0 "R15" H 9059 4646 50  0000 L CNN
F 1 "100K" H 9059 4555 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9000 4600 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9000 4600 50  0001 C CNN
F 4 "RES SMD 100K OHM 5% 1/10W 0603" H 9000 4600 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9000 4600 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603100KJNEA" H 9000 4600 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9000 4600 50  0001 C CNN "Supplier"
F 8 "541-100KGCT-ND" H 9000 4600 50  0001 C CNN "Supplier PN"
	1    9000 4600
	1    0    0    -1  
$EndComp
$Comp
L Project:ESDALC6V1-1U2 D11
U 1 1 60CA641F
P 9325 4600
F 0 "D11" V 9279 4678 50  0000 L CNN
F 1 "ESDALC6V1-1U2" V 9370 4678 50  0000 L CNN
F 2 "lib_fp:ST0201" H 9325 4400 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/90/b1/aa/a3/9f/73/4d/1e/CD00212838.pdf/files/CD00212838.pdf/jcr:content/translations/en.CD00212838.pdf" H 9325 4650 50  0001 C CNN
F 4 "TVS DIODE 3V ST0201" H 9325 4600 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 9325 4600 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1-1U2" H 9325 4600 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9325 4600 50  0001 C CNN "Supplier"
F 8 "497-8752-1-ND" H 9325 4600 50  0001 C CNN "Supplier PN"
	1    9325 4600
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C23
U 1 1 60CA642E
P 10125 4600
F 0 "C23" H 10217 4646 50  0000 L CNN
F 1 "0.1uF" H 10217 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10125 4600 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 10125 4600 50  0001 C CNN
F 4 "CAP CER 0.1UF 25V X7R 0603" H 10125 4600 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 10125 4600 50  0001 C CNN "Manufacturer"
F 6 "CL10B104KA8NNNC" H 10125 4600 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 10125 4600 50  0001 C CNN "Supplier"
F 8 "1276-1006-1-ND" H 10125 4600 50  0001 C CNN "Supplier PN"
	1    10125 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R14
U 1 1 60CA643D
P 9750 4400
F 0 "R14" H 9809 4446 50  0000 L CNN
F 1 "330R" H 9809 4355 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9750 4400 50  0001 C CNN
F 3 "https://www.vishay.com/docs/20035/dcrcwe3.pdf" H 9750 4400 50  0001 C CNN
F 4 "RES SMD 330 OHM 5% 1/10W 0603" H 9750 4400 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 9750 4400 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603330RJNEA" H 9750 4400 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 9750 4400 50  0001 C CNN "Supplier"
F 8 "541-330GCT-ND" H 9750 4400 50  0001 C CNN "Supplier PN"
	1    9750 4400
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR018
U 1 1 60CA6447
P 8425 4275
F 0 "#PWR018" H 8425 4125 50  0001 C CNN
F 1 "+3.3V" H 8440 4448 50  0000 C CNN
F 2 "" H 8425 4275 50  0001 C CNN
F 3 "" H 8425 4275 50  0001 C CNN
	1    8425 4275
	1    0    0    -1  
$EndComp
Wire Wire Line
	8425 4275 8425 4400
Wire Wire Line
	8425 4400 8500 4400
Wire Wire Line
	8900 4400 9000 4400
Connection ~ 9325 4400
Wire Wire Line
	9325 4400 9650 4400
Wire Wire Line
	9850 4400 10125 4400
Wire Wire Line
	10125 4500 10125 4400
Connection ~ 10125 4400
Wire Wire Line
	10125 4400 10400 4400
Wire Wire Line
	9000 4500 9000 4400
Connection ~ 9000 4400
Wire Wire Line
	9000 4400 9325 4400
$Comp
L power:GND #PWR021
U 1 1 60CA645D
P 9000 4800
F 0 "#PWR021" H 9000 4550 50  0001 C CNN
F 1 "GND" H 9005 4627 50  0000 C CNN
F 2 "" H 9000 4800 50  0001 C CNN
F 3 "" H 9000 4800 50  0001 C CNN
	1    9000 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 4700 9000 4800
Wire Wire Line
	9000 4800 9325 4800
Wire Wire Line
	9325 4800 10125 4800
Wire Wire Line
	10125 4800 10125 4700
Connection ~ 9325 4800
Connection ~ 9000 4800
Text Notes 8325 4025 0    50   ~ 0
Hinge Button
Wire Notes Line
	8275 3900 10450 3900
Wire Notes Line
	10450 3900 10450 5050
Wire Notes Line
	10450 5050 8275 5050
Wire Notes Line
	8275 5050 8275 3900
$Comp
L Device:C_Small C14
U 1 1 60DB9B4D
P 6050 1075
F 0 "C14" H 6142 1121 50  0000 L CNN
F 1 "1uF" H 6142 1030 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6050 1075 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 6050 1075 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 6050 1075 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 6050 1075 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 6050 1075 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 6050 1075 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 6050 1075 50  0001 C CNN "Supplier PN"
	1    6050 1075
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C15
U 1 1 60DBA65D
P 6050 1475
F 0 "C15" H 6142 1521 50  0000 L CNN
F 1 "1uF" H 6142 1430 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6050 1475 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 6050 1475 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 6050 1475 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 6050 1475 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 6050 1475 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 6050 1475 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 6050 1475 50  0001 C CNN "Supplier PN"
	1    6050 1475
	1    0    0    -1  
$EndComp
$Comp
L Project:ER-OLED013 J3
U 1 1 60DDA2CA
P 6800 2025
F 0 "J3" H 6750 3250 60  0000 L CNN
F 1 "Display Connector" H 6400 800 60  0000 L CNN
F 2 "lib_fp:ER-CON30HT-1" H 6978 1919 60  0001 L CNN
F 3 "https://www.buydisplay.com/download/connector/ER-CON30HT-1.pdf" H 7250 3725 60  0001 C CNN
F 4 "CONN FFC TOP 30POS 0.50MM R/A" H 6800 2025 50  0001 C CNN "Description"
F 5 "EastRising" H 6800 2025 50  0001 C CNN "Manufacturer"
F 6 "ER-CON30HT-1" H 6800 2025 50  0001 C CNN "Manufacturer PN"
F 7 "BuyDisplay" H 6800 2025 50  0001 C CNN "Supplier"
	1    6800 2025
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C16
U 1 1 60E2BE66
P 5875 1875
F 0 "C16" H 5967 1921 50  0000 L CNN
F 1 "1uF" H 5967 1830 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5875 1875 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 5875 1875 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 5875 1875 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 5875 1875 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 5875 1875 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 5875 1875 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 5875 1875 50  0001 C CNN "Supplier PN"
	1    5875 1875
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C17
U 1 1 60E2CCE8
P 5800 2500
F 0 "C17" H 5892 2546 50  0000 L CNN
F 1 "1uF" H 5892 2455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5800 2500 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 5800 2500 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 5800 2500 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 5800 2500 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 5800 2500 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 5800 2500 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 5800 2500 50  0001 C CNN "Supplier PN"
	1    5800 2500
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 60E74EA5
P 5875 1775
F 0 "#PWR02" H 5875 1625 50  0001 C CNN
F 1 "+3.3V" H 5890 1948 50  0000 C CNN
F 2 "" H 5875 1775 50  0001 C CNN
F 3 "" H 5875 1775 50  0001 C CNN
	1    5875 1775
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 60E7F7B0
P 5875 1975
F 0 "#PWR03" H 5875 1725 50  0001 C CNN
F 1 "GND" H 5875 1825 50  0000 C CNN
F 2 "" H 5875 1975 50  0001 C CNN
F 3 "" H 5875 1975 50  0001 C CNN
	1    5875 1975
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR07
U 1 1 60E9D486
P 5800 2400
F 0 "#PWR07" H 5800 2250 50  0001 C CNN
F 1 "+3.3V" H 5815 2573 50  0000 C CNN
F 2 "" H 5800 2400 50  0001 C CNN
F 3 "" H 5800 2400 50  0001 C CNN
	1    5800 2400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 60E9D77F
P 5800 2600
F 0 "#PWR09" H 5800 2350 50  0001 C CNN
F 1 "GND" H 5805 2427 50  0000 C CNN
F 2 "" H 5800 2600 50  0001 C CNN
F 3 "" H 5800 2600 50  0001 C CNN
	1    5800 2600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 60E9D9D0
P 6125 1975
F 0 "#PWR04" H 6125 1725 50  0001 C CNN
F 1 "GND" H 6125 1825 50  0000 C CNN
F 2 "" H 6125 1975 50  0001 C CNN
F 3 "" H 6125 1975 50  0001 C CNN
	1    6125 1975
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 60ED3F2D
P 6250 2850
F 0 "#PWR011" H 6250 2600 50  0001 C CNN
F 1 "GND" H 6255 2677 50  0000 C CNN
F 2 "" H 6250 2850 50  0001 C CNN
F 3 "" H 6250 2850 50  0001 C CNN
	1    6250 2850
	1    0    0    -1  
$EndComp
Text Label 7675 1075 2    50   ~ 0
DISP_~RES
$Comp
L Device:R_Small R10
U 1 1 61055858
P 7875 2575
F 0 "R10" H 7934 2621 50  0000 L CNN
F 1 "390K" H 7934 2530 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7875 2575 50  0001 C CNN
F 3 "https://www.vishay.com/docs/28773/crcwce3.pdf" H 7875 2575 50  0001 C CNN
F 4 "RES SMD 390K OHM 1% 1/10W 0603" H 7875 2575 50  0001 C CNN "Description"
F 5 "Vishay Dale" H 7875 2575 50  0001 C CNN "Manufacturer"
F 6 "CRCW0603390KFKEAC" H 7875 2575 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 7875 2575 50  0001 C CNN "Supplier"
F 8 "541-5379-1-ND" H 7875 2575 50  0001 C CNN "Supplier PN"
	1    7875 2575
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C20
U 1 1 610812B9
P 7700 2975
F 0 "C20" H 7792 3021 50  0000 L CNN
F 1 "4.7uF" H 7792 2930 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7700 2975 50  0001 C CNN
F 3 "https://product.tdk.com/info/en/catalog/datasheets/mlcc_commercial_general_en.pdf" H 7700 2975 50  0001 C CNN
F 4 "CAP CER 4.7UF 16V X5R 0603" H 7700 2975 50  0001 C CNN "Description"
F 5 "TDK Corporation" H 7700 2975 50  0001 C CNN "Manufacturer"
F 6 "C1608X5R1C475K080AC" H 7700 2975 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 7700 2975 50  0001 C CNN "Supplier"
F 8 "445-7478-1-ND" H 7700 2975 50  0001 C CNN "Supplier PN"
	1    7700 2975
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C19
U 1 1 610822AF
P 7300 2975
F 0 "C19" H 7392 3021 50  0000 L CNN
F 1 "2.2uF" H 7392 2930 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7300 2975 50  0001 C CNN
F 3 "http://www.samsungsem.com/kr/support/product-search/mlcc/CL10B225KP8NNNC.jsp" H 7300 2975 50  0001 C CNN
F 4 "CAP CER 2.2UF 10V X7R 0603" H 7300 2975 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 7300 2975 50  0001 C CNN "Manufacturer"
F 6 "CL10B225KP8NNNC" H 7300 2975 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 7300 2975 50  0001 C CNN "Supplier"
F 8 "1276-1134-1-ND" H 7300 2975 50  0001 C CNN "Supplier PN"
	1    7300 2975
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 610BCEEC
P 7700 3150
F 0 "#PWR015" H 7700 2900 50  0001 C CNN
F 1 "GND" H 7705 2977 50  0000 C CNN
F 2 "" H 7700 3150 50  0001 C CNN
F 3 "" H 7700 3150 50  0001 C CNN
	1    7700 3150
	1    0    0    -1  
$EndComp
Text Notes 5675 675  0    50   ~ 0
Display Interface
Wire Notes Line
	8200 550  8200 3625
Wire Notes Line
	8200 3625 5625 3625
Wire Notes Line
	5625 3625 5625 550 
Wire Wire Line
	6350 975  6050 975 
Wire Wire Line
	6350 1175 6050 1175
Wire Wire Line
	6350 1375 6050 1375
Wire Wire Line
	6350 1575 6050 1575
Wire Wire Line
	6350 1975 6125 1975
Wire Wire Line
	6350 2375 6250 2375
Wire Wire Line
	6250 2375 6250 2575
Wire Wire Line
	6350 2775 6250 2775
Connection ~ 6250 2775
Wire Wire Line
	6250 2775 6250 2850
$Comp
L power:GND #PWR01
U 1 1 6127DDB0
P 7800 1400
F 0 "#PWR01" H 7800 1150 50  0001 C CNN
F 1 "GND" H 7805 1227 50  0000 C CNN
F 2 "" H 7800 1400 50  0001 C CNN
F 3 "" H 7800 1400 50  0001 C CNN
	1    7800 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 1075 7675 1075
Wire Wire Line
	7675 1575 7250 1575
$Comp
L power:GND #PWR05
U 1 1 6130ED69
P 7600 2275
F 0 "#PWR05" H 7600 2025 50  0001 C CNN
F 1 "GND" H 7600 2125 50  0000 C CNN
F 2 "" H 7600 2275 50  0001 C CNN
F 3 "" H 7600 2275 50  0001 C CNN
	1    7600 2275
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 1875 7400 1875
Wire Wire Line
	7400 1875 7400 1975
Wire Wire Line
	7250 1975 7400 1975
Connection ~ 7400 1975
Wire Wire Line
	7250 2075 7400 2075
Wire Wire Line
	7400 1975 7400 2075
Connection ~ 7400 2075
Wire Wire Line
	7400 2075 7400 2175
Wire Wire Line
	7250 2175 7400 2175
Connection ~ 7400 2175
Wire Wire Line
	7400 2175 7400 2275
Wire Wire Line
	7250 2275 7400 2275
Connection ~ 7400 2275
Wire Wire Line
	7700 2675 7700 2875
Wire Wire Line
	7250 2875 7300 2875
Wire Wire Line
	7250 3075 7300 3075
Wire Wire Line
	7700 2675 7250 2675
Wire Wire Line
	6350 1775 5875 1775
Connection ~ 5875 1775
Wire Wire Line
	7700 3075 7700 3150
Wire Wire Line
	5800 2400 5975 2400
Wire Wire Line
	5975 2400 5975 2175
Wire Wire Line
	5975 2175 6350 2175
Connection ~ 5800 2400
Wire Wire Line
	7875 2475 7250 2475
$Comp
L power:GND #PWR010
U 1 1 616AD7F4
P 7875 2675
F 0 "#PWR010" H 7875 2425 50  0001 C CNN
F 1 "GND" H 7875 2525 50  0000 C CNN
F 2 "" H 7875 2675 50  0001 C CNN
F 3 "" H 7875 2675 50  0001 C CNN
	1    7875 2675
	1    0    0    -1  
$EndComp
Connection ~ 10125 1075
Wire Wire Line
	9850 1075 10125 1075
Wire Notes Line
	5625 550  8200 550 
$Comp
L Device:Net-Tie_2 NT1
U 1 1 60BA0BEB
P 1825 4350
F 0 "NT1" H 1825 4300 50  0000 C CNN
F 1 "Net-Tie_2" H 1825 4440 50  0001 C CNN
F 2 "lib_fp:NetTie-2_SMD_Pad_20mil" H 1825 4350 50  0001 C CNN
F 3 "~" H 1825 4350 50  0001 C CNN
	1    1825 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1925 4250 1925 4300
Wire Wire Line
	1975 4300 1925 4300
Connection ~ 1925 4300
Wire Wire Line
	1925 4300 1925 4350
$Comp
L Device:Net-Tie_2 NT2
U 1 1 60C06AD9
P 7500 2275
F 0 "NT2" H 7500 2325 50  0000 C CNN
F 1 "Net-Tie_2" H 7500 2365 50  0001 C CNN
F 2 "lib_fp:NetTie-2_SMD_Pad_10mil" H 7500 2275 50  0001 C CNN
F 3 "~" H 7500 2275 50  0001 C CNN
	1    7500 2275
	1    0    0    -1  
$EndComp
Text Label 3525 2400 0    50   ~ 0
DISP_~RES
Text Label 5475 2000 2    50   ~ 0
TLED_EN
Text Label 3525 2700 0    50   ~ 0
BTN3
Text Label 5475 3200 2    50   ~ 0
BTN4
Text Label 5475 2500 2    50   ~ 0
BTN5
Wire Wire Line
	5025 2000 5475 2000
Wire Wire Line
	3525 2700 3925 2700
Wire Wire Line
	5025 3200 5475 3200
Wire Wire Line
	5025 2500 5475 2500
Wire Wire Line
	7800 1275 7800 1375
Text Label 7675 975  2    50   Italic 0
DISP_~CS
Text Label 7675 1175 2    50   Italic 0
DISP_D~C
Wire Wire Line
	7250 975  7675 975 
Wire Wire Line
	7250 1175 7675 1175
Wire Wire Line
	7250 1275 7800 1275
Wire Wire Line
	7250 1375 7800 1375
Connection ~ 7800 1375
Wire Wire Line
	7800 1375 7800 1400
Wire Wire Line
	7250 1675 7675 1675
Text Label 7675 1575 2    50   Italic 0
DISP_SCK
Text Label 7675 1675 2    50   Italic 0
DISP_MOSI
NoConn ~ 7250 1775
Wire Wire Line
	6350 2575 6250 2575
Connection ~ 6250 2575
Wire Wire Line
	6250 2575 6250 2775
Text Label 5475 2100 2    50   Italic 0
DISP_~CS
Text Label 5475 2300 2    50   Italic 0
DISP_D~C
Text Label 5475 2200 2    50   Italic 0
DISP_SCK
Text Label 5475 2400 2    50   Italic 0
DISP_MOSI
Wire Wire Line
	5475 2100 5025 2100
Wire Wire Line
	5475 2200 5025 2200
Wire Wire Line
	5025 2300 5475 2300
Wire Wire Line
	5475 2400 5025 2400
Wire Wire Line
	3525 2400 3925 2400
$Comp
L power:GND #PWR0119
U 1 1 60BE6FE8
P 7300 3150
F 0 "#PWR0119" H 7300 2900 50  0001 C CNN
F 1 "GND" H 7305 2977 50  0000 C CNN
F 2 "" H 7300 3150 50  0001 C CNN
F 3 "" H 7300 3150 50  0001 C CNN
	1    7300 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 3075 7300 3150
Connection ~ 7300 3075
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 60BF6E7F
P 7300 2875
F 0 "#FLG0103" H 7300 2950 50  0001 C CNN
F 1 "PWR_FLAG" H 7300 3048 50  0001 C CNN
F 2 "" H 7300 2875 50  0001 C CNN
F 3 "~" H 7300 2875 50  0001 C CNN
	1    7300 2875
	1    0    0    -1  
$EndComp
Connection ~ 7300 2875
$Comp
L Device:Ferrite_Bead FB2
U 1 1 60C00550
P 5050 950
F 0 "FB2" V 4776 950 50  0000 C CNN
F 1 "600R @ 100MHz" V 4867 950 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" V 4980 950 50  0001 C CNN
F 3 "https://product.tdk.com/en/system/files?file=dam/doc/product/emc/emc/beads/catalog/beads_commercial_signal_mmz1608_en.pdf" H 5050 950 50  0001 C CNN
F 4 "FERRITE BEAD 600 OHM 0603 1LN" H 5050 950 50  0001 C CNN "Description"
F 5 "TDK Corporation" H 5050 950 50  0001 C CNN "Manufacturer"
F 6 "MMZ1608B601CTAH0" H 5050 950 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 5050 950 50  0001 C CNN "Supplier"
F 8 "445-2166-1-ND" H 5050 950 50  0001 C CNN "Supplier PN"
	1    5050 950 
	0    1    1    0   
$EndComp
Wire Notes Line
	2600 5100 550  5100
Wire Notes Line
	2600 6200 2600 5100
Wire Notes Line
	550  6200 2600 6200
Wire Notes Line
	550  5100 550  6200
Text Notes 600  5225 0    50   ~ 0
Power Regulator
Wire Wire Line
	1825 5550 2000 5550
Wire Wire Line
	2000 5550 2300 5550
Connection ~ 2000 5550
$Comp
L Connector:TestPoint TP1
U 1 1 60670A35
P 2000 5550
F 0 "TP1" H 2058 5622 50  0000 L CNN
F 1 "3.3V" H 2058 5577 50  0001 L CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 2200 5550 50  0001 C CNN
F 3 "~" H 2200 5550 50  0001 C CNN
F 4 "3.3V" H 2000 5550 50  0001 C CNN "User Label"
	1    2000 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0123
U 1 1 605C3F6B
P 850 5750
F 0 "#PWR0123" H 850 5500 50  0001 C CNN
F 1 "GND" H 855 5577 50  0000 C CNN
F 2 "" H 850 5750 50  0001 C CNN
F 3 "" H 850 5750 50  0001 C CNN
	1    850  5750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0108
U 1 1 6057C4A8
P 850 5500
F 0 "#PWR0108" H 850 5350 50  0001 C CNN
F 1 "+5V" H 865 5673 50  0000 C CNN
F 2 "" H 850 5500 50  0001 C CNN
F 3 "" H 850 5500 50  0001 C CNN
	1    850  5500
	1    0    0    -1  
$EndComp
Connection ~ 2300 5550
Wire Wire Line
	2300 5500 2300 5550
$Comp
L power:+3.3V #PWR0107
U 1 1 6057B039
P 2300 5500
F 0 "#PWR0107" H 2300 5350 50  0001 C CNN
F 1 "+3.3V" H 2315 5673 50  0000 C CNN
F 2 "" H 2300 5500 50  0001 C CNN
F 3 "" H 2300 5500 50  0001 C CNN
	1    2300 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 6057AC86
P 2300 5750
F 0 "#PWR0106" H 2300 5500 50  0001 C CNN
F 1 "GND" H 2305 5577 50  0000 C CNN
F 2 "" H 2300 5750 50  0001 C CNN
F 3 "" H 2300 5750 50  0001 C CNN
	1    2300 5750
	1    0    0    -1  
$EndComp
Connection ~ 1025 5550
Wire Wire Line
	1025 5550 850  5550
Wire Wire Line
	1025 5650 1025 5550
$Comp
L Device:C_Small C12
U 1 1 605785BD
P 2300 5650
F 0 "C12" H 2208 5604 50  0000 R CNN
F 1 "1uF" H 2208 5695 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2300 5650 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 2300 5650 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 2300 5650 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 2300 5650 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 2300 5650 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2300 5650 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 2300 5650 50  0001 C CNN "Supplier PN"
	1    2300 5650
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C11
U 1 1 60577DD7
P 850 5650
F 0 "C11" H 758 5604 50  0000 R CNN
F 1 "1uF" H 758 5695 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 850 5650 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL10B105KA8NNNC_char.pdf" H 850 5650 50  0001 C CNN
F 4 "CAP CER 1UF 25V X7R 0603" H 850 5650 50  0001 C CNN "Description"
F 5 "Samsung Electro-Mechanics" H 850 5650 50  0001 C CNN "Manufacturer"
F 6 "CL10B105KA8NNNC" H 850 5650 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 850 5650 50  0001 C CNN "Supplier"
F 8 "1276-1184-1-ND" H 850 5650 50  0001 C CNN "Supplier PN"
	1    850  5650
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 605773F6
P 1900 5950
F 0 "#PWR0105" H 1900 5700 50  0001 C CNN
F 1 "GND" H 1905 5777 50  0000 C CNN
F 2 "" H 1900 5950 50  0001 C CNN
F 3 "" H 1900 5950 50  0001 C CNN
	1    1900 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 5650 1900 5750
Wire Wire Line
	1825 5650 1900 5650
$Comp
L Device:C_Small C13
U 1 1 6057632E
P 1900 5850
F 0 "C13" H 1808 5804 50  0000 R CNN
F 1 "22nF" H 1808 5895 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1900 5850 50  0001 C CNN
F 3 "https://product.tdk.com/info/en/documents/chara_sheet/C2012C0G1H223J125AA.pdf" H 1900 5850 50  0001 C CNN
F 4 "CAP CER 0.022UF 50V C0G 0805" H 1900 5850 50  0001 C CNN "Description"
F 5 "TDK Corporation" H 1900 5850 50  0001 C CNN "Manufacturer"
F 6 "C2012C0G1H223J125AA" H 1900 5850 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 1900 5850 50  0001 C CNN "Supplier"
F 8 "445-7522-1-ND" H 1900 5850 50  0001 C CNN "Supplier PN"
	1    1900 5850
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 60575FE2
P 1425 5950
F 0 "#PWR0104" H 1425 5700 50  0001 C CNN
F 1 "GND" H 1430 5777 50  0000 C CNN
F 2 "" H 1425 5950 50  0001 C CNN
F 3 "" H 1425 5950 50  0001 C CNN
	1    1425 5950
	1    0    0    -1  
$EndComp
$Comp
L Project:RT9193 U5
U 1 1 6056456B
P 1425 5650
F 0 "U5" H 1425 5992 50  0000 C CNN
F 1 "RT9193" H 1425 5901 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 1425 5950 50  0001 C CIN
F 3 "https://www.richtek.com/assets/product_file/RT9193/DS9193-17.pdf" H 1425 5650 50  0001 C CNN
F 4 "IC REG LINEAR 3.3V 300MA SOT23-5" H 1425 5650 50  0001 C CNN "Description"
F 5 "Richtek USA Inc." H 1425 5650 50  0001 C CNN "Manufacturer"
F 6 "RT9193-33GB" H 1425 5650 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 1425 5650 50  0001 C CNN "Supplier"
F 8 "1028-1014-1-ND" H 1425 5650 50  0001 C CNN "Supplier PN"
	1    1425 5650
	1    0    0    -1  
$EndComp
Text Notes 4775 3850 0    50   ~ 0
Debug Connector
Wire Notes Line
	4725 3725 8200 3725
Wire Notes Line
	4725 5750 4725 3725
Wire Notes Line
	8200 5750 4725 5750
Wire Notes Line
	8200 3725 8200 5750
$Comp
L power:GND #PWR0127
U 1 1 605CA9D5
P 7450 4675
F 0 "#PWR0127" H 7450 4425 50  0001 C CNN
F 1 "GND" H 7455 4502 50  0000 C CNN
F 2 "" H 7450 4675 50  0001 C CNN
F 3 "" H 7450 4675 50  0001 C CNN
	1    7450 4675
	1    0    0    -1  
$EndComp
Wire Wire Line
	6025 4275 7450 4275
$Comp
L Project:ESDALC6V1-1U2 D7
U 1 1 605C5670
P 7450 4475
F 0 "D7" V 7404 4553 50  0000 L CNN
F 1 "ESDALC6V1-1U2" V 7495 4553 50  0000 L CNN
F 2 "lib_fp:ST0201" H 7450 4275 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/90/b1/aa/a3/9f/73/4d/1e/CD00212838.pdf/files/CD00212838.pdf/jcr:content/translations/en.CD00212838.pdf" H 7450 4525 50  0001 C CNN
F 4 "TVS DIODE 3V ST0201" H 7450 4475 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 7450 4475 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1-1U2" H 7450 4475 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 7450 4475 50  0001 C CNN "Supplier"
F 8 "497-8752-1-ND" H 7450 4475 50  0001 C CNN "Supplier PN"
	1    7450 4475
	0    1    1    0   
$EndComp
Text Label 6175 4275 0    50   ~ 0
RESET
NoConn ~ 6025 4975
NoConn ~ 6025 4775
NoConn ~ 6025 4675
Connection ~ 5425 5475
Wire Wire Line
	5525 5475 5425 5475
Wire Wire Line
	7175 4625 7175 4675
Wire Wire Line
	7125 4625 7175 4625
$Comp
L power:GND #PWR0122
U 1 1 605BDA4D
P 7175 4675
F 0 "#PWR0122" H 7175 4425 50  0001 C CNN
F 1 "GND" H 7180 4502 50  0000 C CNN
F 2 "" H 7175 4675 50  0001 C CNN
F 3 "" H 7175 4675 50  0001 C CNN
	1    7175 4675
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0121
U 1 1 605BCFA5
P 5425 5475
F 0 "#PWR0121" H 5425 5225 50  0001 C CNN
F 1 "GND" H 5430 5302 50  0000 C CNN
F 2 "" H 5425 5475 50  0001 C CNN
F 3 "" H 5425 5475 50  0001 C CNN
	1    5425 5475
	1    0    0    -1  
$EndComp
Wire Wire Line
	6025 5275 6650 5275
Wire Wire Line
	6025 5175 6600 5175
Wire Wire Line
	6025 4575 6725 4575
Wire Wire Line
	6025 4475 6725 4475
Text Label 6150 5175 0    50   ~ 0
USART1_RX
Text Label 6150 5275 0    50   ~ 0
USART1_TX
Text Label 6150 4475 0    50   ~ 0
SWCLK
Text Label 6150 4575 0    50   ~ 0
SWDIO
NoConn ~ 5275 5475
$Comp
L Project:ESDALC6V1W5 D8
U 1 1 605AC89E
P 6925 4625
F 0 "D8" H 6925 4900 50  0000 C CNN
F 1 "ESDALC6V1W5" H 6925 4325 50  0000 C CNN
F 2 "lib_fp:SOT323-5L" H 6925 4250 50  0001 C CNN
F 3 "https://www.st.com/content/ccc/resource/technical/document/datasheet/32/30/02/e6/ac/0f/46/c2/CD00002946.pdf/files/CD00002946.pdf/jcr:content/translations/en.CD00002946.pdf" H 6925 4675 50  0001 C CNN
F 4 "TVS DIODE 3V SOT323-5" H 6925 4625 50  0001 C CNN "Description"
F 5 "STMicroelectronics" H 6925 4625 50  0001 C CNN "Manufacturer"
F 6 "ESDALC6V1W5" H 6925 4625 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 6925 4625 50  0001 C CNN "Supplier"
F 8 "497-7231-1-ND" H 6925 4625 50  0001 C CNN "Supplier PN"
	1    6925 4625
	1    0    0    -1  
$EndComp
$Comp
L Project:Conn_ST_STDC14 J2
U 1 1 60586F63
P 5525 4775
F 0 "J2" H 5082 4821 50  0000 R CNN
F 1 "DNI" H 5075 4625 50  0000 R CNN
F 2 "Connector:Tag-Connect_TC2070-IDC-FP_2x07_P1.27mm_Vertical" H 5525 3675 50  0001 C CNN
F 3 "https://www.tag-connect.com/wp-content/uploads/bsk-pdf-manager/TC2070-IDC_Datasheet_11.pdf" V 5175 3525 50  0001 C CNN
F 4 "Tag-Connect" H 5525 4775 50  0001 C CNN "Manufacturer"
F 5 "TC2070-IDC-050" H 5525 4775 50  0001 C CNN "Manufacturer PN"
F 6 "STDC14" H 4950 4725 50  0000 C CNN "User Label"
	1    5525 4775
	1    0    0    -1  
$EndComp
Wire Notes Line
	550  7850 4050 7850
Wire Notes Line
	550  6275 550  7850
Wire Notes Line
	4050 6275 550  6275
Text Notes 600  6400 0    50   ~ 0
USB Input Port
Wire Notes Line
	4050 7850 4050 6275
Wire Wire Line
	2675 6625 2750 6625
Wire Wire Line
	3875 6625 3875 6550
Wire Wire Line
	3575 6625 3875 6625
Wire Wire Line
	3050 6625 3275 6625
$Comp
L Device:Polyfuse F1
U 1 1 60C2F235
P 2900 6625
F 0 "F1" V 2675 6625 50  0000 C CNN
F 1 "250mA" V 2766 6625 50  0000 C CNN
F 2 "lib_fp:BF_0ZCJ_1206" H 2950 6425 50  0001 L CNN
F 3 "https://www.belfuse.com/resources/datasheets/circuitprotection/ds-cp-0zcj-series.pdf" H 2900 6625 50  0001 C CNN
F 4 "PTC RESET FUSE 16V 250MA 1206" H 2900 6625 50  0001 C CNN "Description"
F 5 "Bel Fuse Inc." H 2900 6625 50  0001 C CNN "Manufacturer"
F 6 "0ZCJ0025FF2E" H 2900 6625 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 2900 6625 50  0001 C CNN "Supplier"
F 8 "507-1798-1-ND" H 2900 6625 50  0001 C CNN "Supplier PN"
	1    2900 6625
	0    1    1    0   
$EndComp
$Comp
L Device:Ferrite_Bead FB1
U 1 1 60BFE585
P 3425 6625
F 0 "FB1" V 3151 6625 50  0000 C CNN
F 1 "600R @ 100MHz" V 3242 6625 50  0000 C CNN
F 2 "Inductor_SMD:L_0603_1608Metric" V 3355 6625 50  0001 C CNN
F 3 "https://product.tdk.com/en/system/files?file=dam/doc/product/emc/emc/beads/catalog/beads_commercial_signal_mmz1608_en.pdf" H 3425 6625 50  0001 C CNN
F 4 "FERRITE BEAD 600 OHM 0603 1LN" H 3425 6625 50  0001 C CNN "Description"
F 5 "TDK Corporation" H 3425 6625 50  0001 C CNN "Manufacturer"
F 6 "MMZ1608B601CTAH0" H 3425 6625 50  0001 C CNN "Manufacturer PN"
F 7 "Digi-Key" H 3425 6625 50  0001 C CNN "Supplier"
F 8 "445-2166-1-ND" H 3425 6625 50  0001 C CNN "Supplier PN"
	1    3425 6625
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0109
U 1 1 60588788
P 3875 6550
F 0 "#PWR0109" H 3875 6400 50  0001 C CNN
F 1 "+5V" H 3890 6723 50  0000 C CNN
F 2 "" H 3875 6550 50  0001 C CNN
F 3 "" H 3875 6550 50  0001 C CNN
	1    3875 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	850  5500 850  5550
Connection ~ 850  5550
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 60E78DF9
P 3875 6625
F 0 "#FLG0104" H 3875 6700 50  0001 C CNN
F 1 "PWR_FLAG" V 3875 6753 50  0001 L CNN
F 2 "" H 3875 6625 50  0001 C CNN
F 3 "~" H 3875 6625 50  0001 C CNN
	1    3875 6625
	0    1    1    0   
$EndComp
Connection ~ 3875 6625
Wire Wire Line
	1125 6975 1525 6975
Wire Wire Line
	6650 5275 6650 4775
Wire Wire Line
	6650 4775 6725 4775
Wire Wire Line
	6600 5175 6600 4675
Wire Wire Line
	6600 4675 6725 4675
$Comp
L power:GND #PWR0124
U 1 1 60CBE3DA
P 9450 5550
F 0 "#PWR0124" H 9450 5300 50  0001 C CNN
F 1 "GND" H 9450 5400 50  0000 C CNN
F 2 "" H 9450 5550 50  0001 C CNN
F 3 "" H 9450 5550 50  0001 C CNN
	1    9450 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0132
U 1 1 60CBE6B6
P 9450 6000
F 0 "#PWR0132" H 9450 5750 50  0001 C CNN
F 1 "GND" H 9450 5850 50  0000 C CNN
F 2 "" H 9450 6000 50  0001 C CNN
F 3 "" H 9450 6000 50  0001 C CNN
	1    9450 6000
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 60C839C0
P 5525 4075
F 0 "#PWR?" H 5525 3925 50  0001 C CNN
F 1 "+3.3V" H 5540 4248 50  0000 C CNN
F 2 "" H 5525 4075 50  0001 C CNN
F 3 "" H 5525 4075 50  0001 C CNN
	1    5525 4075
	1    0    0    -1  
$EndComp
Text Notes 5675 5700 0    50   ~ 0
Tag-Connect 14-pin footprint is used for the debug connection
Text Notes 8325 5250 0    50   ~ 0
Board Mounting
Wire Notes Line
	8275 5125 10450 5125
Wire Notes Line
	10450 5125 10450 6250
Wire Notes Line
	10450 6250 8275 6250
Wire Notes Line
	8275 6250 8275 5125
$EndSCHEMATC
