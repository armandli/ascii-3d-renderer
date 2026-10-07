# Thin wrapper around CMake. All real build logic lives in CMakeLists.txt.

BUILD_DIR  ?= build
BUILD_TYPE ?= Debug
GENERATOR  ?= $(if $(shell command -v ninja 2>/dev/null),Ninja,Unix Makefiles)
JOBS       ?= $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

.PHONY: all configure build release test run clean distclean help

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -G "$(GENERATOR)" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR) -j $(JOBS)

release:
	$(MAKE) BUILD_DIR=build-release BUILD_TYPE=Release build

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/src/ascii3d_demo

clean:
	@if [ -d $(BUILD_DIR) ]; then cmake --build $(BUILD_DIR) --target clean; fi

distclean:
	rm -rf build build-*

help:
	@echo "Targets:"
	@echo "  make / make build   configure and build ($(BUILD_TYPE)) into $(BUILD_DIR)/"
	@echo "  make release        Release build into build-release/"
	@echo "  make test           build and run tests via ctest"
	@echo "  make run            build and run the demo"
	@echo "  make clean          clean build outputs"
	@echo "  make distclean      remove all build directories"
	@echo "Variables: BUILD_DIR, BUILD_TYPE, GENERATOR, JOBS"
