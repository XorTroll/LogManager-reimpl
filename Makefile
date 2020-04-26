
.PHONY: all clean

all:
	@$(MAKE) -C Atmosphere-libs/
	@$(MAKE) -C LogManager/
	@rm -rf $(CURDIR)/SdOut
	@mkdir -p $(CURDIR)/SdOut/atmosphere/contents/0100000000000015/flags
	@touch $(CURDIR)/SdOut/atmosphere/contents/0100000000000015/flags/boot2.flag
	@cp $(CURDIR)/LogManager/LogManager.nsp $(CURDIR)/SdOut/atmosphere/contents/0100000000000015/exefs.nsp

clean:
	@rm -rf $(CURDIR)/SdOut
	@$(MAKE) clean -C LogManager/