LINUX := linux
WINDOWS := windows

################################################################################

default:
        @echo "Please choose from the available targets: linux windows linux-run windows-run linux-test windows-test"
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
