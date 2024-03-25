LINUX := linux
WINDOWS := windows
MACOS := macos

################################################################################

default:
        @echo "Please choose from the available targets: linux windows macos linux-test windows-test macos-test linux-all windows-all macos-all"
		@exit 2

################################################################################

.PHONY: linux
linux:
	@make -f build/$(LINUX).make lib
	
.PHONY: linux-test
linux-test:
	@make -f build/$(LINUX).make test

.PHONY: linux-all
linux-all:
	@make -f build/$(LINUX).make all

################################################################################

.PHONY: windows
windows:
	@make -f build/$(WINDOWS).make lib	
	
.PHONY: windows-test
windows-test:
	@make -f build/$(WINDOWS).make test

.PHONY: windows-all
windows-all:
	@make -f build/$(WINDOWS).make all

################################################################################

.PHONY: macos
macos:
	@make -f build/$(MACOS).make lib	
	
.PHONY: macos-test
macos-test:
	@make -f build/$(MACOS).make test

.PHONY: macos-all
macos-all:
	@make -f build/$(MACOS).make all