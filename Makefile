XBE_TITLE = sdl_audio
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
NXDK_SDL = y

new_all: copy_resources all

include $(NXDK_DIR)/Makefile

copy_resources:
	@mkdir -p $(OUTPUT_DIR)
	@cp $(CURDIR)/assets/500hz48000.wav $(OUTPUT_DIR)/500hz48000.wav