# Makefile for building board artifacts

.PHONY: all clean

KIKIT := kikit

BOARDS = main-board trans-led-board
PANELS = $(addsuffix -panel, $(BOARDS))
MINIPANELS = $(addsuffix -panel-mini, $(BOARDS))
BOARDFILES = $(foreach board,$(BOARDS),$(addsuffix .kicad_pcb, $(addprefix $(board)/,$(board))))
PANELFILES = $(addprefix build/, $(PANELS:=.kicad_pcb))
MINIPANELFILES = $(addprefix build/, $(MINIPANELS:=.kicad_pcb))
GERBERS = $(addprefix build/, $(PANELS:=-gerber.zip))

all: $(GERBERS) build/web/index.html

panels: $(PANELFILES)

mini-panels: $(MINIPANELFILES)

build/main-board-panel.kicad_pcb: main-board/main-board.kicad_pcb main-board/kikit-panelize.json tools/kikit-post.py build
	$(KIKIT) panelize -p main-board/kikit-panelize.json $< $@

build/main-board-panel-mini.kicad_pcb: main-board/main-board.kicad_pcb main-board/kikit-panelize.json tools/kikit-post.py build
	$(KIKIT) panelize -p main-board/kikit-panelize.json \
		-l 'grid; rows: 2; cols: 1;' \
		$< $@

build/trans-led-board-panel.kicad_pcb: trans-led-board/trans-led-board.kicad_pcb trans-led-board/kikit-panelize.json tools/kikit-post.py build
	$(KIKIT) panelize -p trans-led-board/kikit-panelize.json $< $@

build/trans-led-board-panel-mini.kicad_pcb: trans-led-board/trans-led-board.kicad_pcb trans-led-board/kikit-panelize.json tools/kikit-post.py build
	$(KIKIT) panelize -p trans-led-board/kikit-panelize.json \
		-l 'grid; rows: 1; cols: 5;' \
		-r 'railstb; width: 5mm; space: 2mm;' \
		-t 'text: Transmission LED Board Panel' \
		$< $@

%-gerber: %.kicad_pcb
	kikit export gerber $< $@

%-gerber.zip: %-gerber
	zip -j $@ `find $<`

web: build/web/index.html

build:
	mkdir -p build

build/web: build
	mkdir -p build/web

build/web/index.html: build/web $(BOARDFILES) $(PANELFILES)
	$(KIKIT) present boardpage \
		-d ../README.md \
		--name "Printalyzer Densitometer" \
		-b "Main Board" " " main-board/main-board.kicad_pcb \
		-b "Transmission LED Board" " " trans-led-board/trans-led-board.kicad_pcb \
		-b "Main Board Panelized" " " build/main-board-panel.kicad_pcb \
		-b "Transmission LED Board Panelized" " " build/trans-led-board-panel.kicad_pcb \
		--repository 'https://github.com/dektronics/printalyzer-densitometer' \
		build/web

clean:
	rm -rf build
