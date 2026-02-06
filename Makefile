# pacPrism Makefile
# Simple build system for Linux

CMAKE = cmake
CMAKE_BUILD_DIR = build

.PHONY: all clean release debug install

all: release

release:
	@echo "Building pacPrism (Release)..."
	@mkdir -p $(CMAKE_BUILD_DIR)
	@cd $(CMAKE_BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Release ..
	@$(CMAKE) --build $(CMAKE_BUILD_DIR) --parallel
	@echo "Build complete. Binary: $(CMAKE_BUILD_DIR)/bin/pacprism"

debug:
	@echo "Building pacPrism (Debug)..."
	@mkdir -p $(CMAKE_BUILD_DIR)
	@cd $(CMAKE_BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug ..
	@$(CMAKE) --build $(CMAKE_BUILD_DIR) --parallel
	@echo "Build complete. Binary: $(CMAKE_BUILD_DIR)/bin/pacprism"

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(CMAKE_BUILD_DIR)
	@echo "Clean complete."

install: release
	@echo "Installing pacPrism..."
	@mkdir -p $(DESTDIR)/usr/bin
	@mkdir -p $(DESTDIR)/etc/pacprism
	@cp $(CMAKE_BUILD_DIR)/bin/pacprism $(DESTDIR)/usr/bin/
	@cp config/pacprism.conf $(DESTDIR)/etc/pacprism/
	@echo "Install complete."
