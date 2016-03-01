DEV_VERSION := $(shell cat VERSION)
MICROHTTPD_VERSION := 0.9.48

JOBS := 12

ifndef CC
export CC := /usr/bin/clang
endif
ifndef CXX
export CXX := /usr/bin/clang++
endif

ifndef BUILDTYPE
BUILDTYPE := Release
endif

# -----------------------------------------------

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))

MICROHTTPD_SOURCE_HOME := deps/microhttpd/libmicrohttpd-$(MICROHTTPD_VERSION)


EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Linux)
	LOCAL_PLATFORM := linux
endif
ifeq ($(UNAME), Darwin)
	LOCAL_PLATFORM := darwin
endif

ifeq ($(RELEASE), true)
DEPLOY := true
VERSION := $(DEV_VERSION)
else
export VERSION_SUFFIX=SNAPSHOT
ifdef GIT_BRANCH
export VERSION_SUFFIX=$(notdir $(GIT_BRANCH))
endif
VERSION := $(addsuffix -$(VERSION_SUFFIX),$(DEV_VERSION))
endif

LOCAL_ARCH := $(shell uname -m)

.PHONY: all clean_all clean_setup clean_microhttpd clean_build test dev_xcode
	
# ---------------------------- Cleaning ---------------------
# targets: clean_all, clean_setup, clean_dev_xcode, clean_build, clean_ios_build, clean_osx_build, clean_android_build, clean_server_build

clean_build:
	-rm -rf build/
	-rm -rf deps/build/
	make cleanup_gyp

clean_setup: clean_microhttpd
	-rm -f variables.gypi
	
clean_microhttpd:
	-rm -f deps/microhttpd
	-rm -rf deps/libs/microhttpd

clean_dev_xcode:
	-rm -rf	dev_xcode

clean_all: clean_build clean_setup clean_dev_xcode
	
cleanup_gyp: ./deps/gyp px.gyp common.gypi
	deps/gyp/tools/pretty_gyp.py px.gyp > px_temp.gyp && mv px_temp.gyp px.gyp
	deps/gyp/tools/pretty_gyp.py common.gypi > common_temp.gypi && mv common_temp.gypi common.gypi	
	
# ------------------------- Buildsystem & Deps --------------------------

gyp: ./deps/gyp

./deps/gyp:
	git clone --depth 1 https://chromium.googlesource.com/external/gyp.git ./deps/gyp

_variables: Makefile
	@echo "{"                                                        >variables.gypi
	@echo "  'variables': {"                                         >>variables.gypi
	@echo "    'LOCAL_PLATFORM': '$(LOCAL_PLATFORM)',"               >>variables.gypi
	@echo "  },"                                                     >>variables.gypi
	@echo "}"                                                        >>variables.gypi              >>variables.gypi	
	
dev_xcode: deps/gyp px.gyp
	deps/gyp/gyp px.gyp -DOS=darwin --depth=. -f xcode --generator-output=./dev_xcode -Icommon.gypi -Ivariables.gypi	

# ----------------- CREATE BUILD DIRECTORIES --------------------
	
build_server: deps/gyp px.gyp
	deps/gyp/gyp px.gyp -DOS=$(LOCAL_PLATFORM) --depth=. --format=make --generator-output=./build/server --root-target cpp_server -Icommon.gypi -Ivariables.gypi

build_test: deps/gyp px.gyp
	deps/gyp/gyp px.gyp -DOS=$(LOCAL_PLATFORM) --depth=. --format=make --generator-output=./build/test --root-target test -Icommon.gypi -Ivariables.gypi


# ------------------- SETUP ------------------------------------

deps/microhttpd: 
	mkdir -p deps/microhttpd
	curl -L http://mirror.netcologne.de/gnu/libmicrohttpd/libmicrohttpd-$(MICROHTTPD_VERSION).tar.gz >$(MICROHTTPD_SOURCE_HOME).tar.gz
	tar -C deps/microhttpd -xzf $(MICROHTTPD_SOURCE_HOME).tar.gz

deps/libs/microhttpd:
ifeq ($(LOCAL_PLATFORM),linux)
	export CPPFLAGS="-std=c++11 -stdlib=libc++ -Wno-deprecated-register -fvisibility=default -fPIC" 
	export LDFLAGS="-nodefaultlibs -lc++ -lc++abi -lm -lc -lgcc_s -lgcc"
	cd $(MICROHTTPD_SOURCE_HOME); autoreconf -ivf .; ./configure --prefix=${current_dir}deps/libs/microhttpd/ --exec-prefix=${current_dir}deps/libs/microhttpd/ --enable-shared=NO --disable-doc --disable-examples --disable-curl --disable-https; make; make install;
else
	export CPPFLAGS="-std=c++11 -stdlib=libc++ -Wno-deprecated-register -fvisibility=default -fPIC" 
	cd $(MICROHTTPD_SOURCE_HOME); ./configure --prefix=${current_dir}deps/libs/microhttpd/ --exec-prefix=${current_dir}deps/libs/microhttpd/ --enable-shared=NO --disable-doc --disable-examples --disable-curl --disable-https; make; make install;
endif

setup_server: _variables deps/microhttpd deps/libs/microhttpd
	mkdir -p test

setup_all: setup_server

setup: setup_all

xb-prettifier := $(shell command -v xcpretty >/dev/null 2>&1 && echo "xcpretty -c" || echo "cat")

# ----------------- Compiling -----------------------------------

compile_server: build_server
	make -j $(JOBS) -C ./build/server all BUILDTYPE=$(BUILDTYPE)

# ----------------- Packaging -----------------------------------

server: compile_server

all: server
	
install: server
	cp ./build/server/out/Release/cpp_server /usr/bin/px-cpp-server

# --------------------- Other Stuff ---------------------------

test: build_test
	make -j $(JOBS) -C build/test all BUILDTYPE=$(BUILDTYPE)
	./build/test/out/$(BUILDTYPE)/test