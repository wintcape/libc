LINUX := linux
WINDOWS := windows
MACOS := macos

################################################################################

default:
        @echo "Please choose from the available targets: linux windows macos linux-run windows-run macos-run linux-test windows-test macos-test"
		@exit 2

################################################################################

.PHONY: linux
linux:
	@make -f build/$(LINUX).make app
	
.PHONY: linux-run
linux-run:
	@make -f build/$(LINUX).make all

	
.PHONY: linux-test
linux-test:
	@make -f build/$(LINUX).make test

################################################################################

.PHONY: windows
windows:
	@make -f build/$(WINDOWS).make app	

.PHONY: windows-run
windows-run:
	@make -f build/$(WINDOWS).make all
	
.PHONY: windows-test
windows-test:
	@make -f build/$(WINDOWS).make test

################################################################################

.PHONY: macos
macos:
	@make -f build/$(MACOS).make app	

.PHONY: macos-run
macos-run:
	@make -f build/$(MACOS).make all
	
.PHONY: macos-test
macos-test:
	@make -f build/$(MACOS).make test
