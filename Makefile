XBE_TITLE = sdl_audio
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
NXDK_SDL = y

new_all: copy_resources all

include $(NXDK_DIR)/Makefile

copy_resources:
	@mkdir -p $(OUTPUT_DIR)
	@cp $(CURDIR)/assets/426669_44.1.wav $(OUTPUT_DIR)/426669_44.1.wav
	@cp $(CURDIR)/assets/426669_48.wav $(OUTPUT_DIR)/426669_48.wav
	@cp $(CURDIR)/assets/426669_16.wav $(OUTPUT_DIR)/426669_16.wav
	@cp $(CURDIR)/assets/426669_96.wav $(OUTPUT_DIR)/426669_96.wav