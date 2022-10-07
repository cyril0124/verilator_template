include $(shell pwd)/config.mk

# TOPNAME = top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc \
				-O3 --x-assign fast --x-initial fast --trace -j

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -D TOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lSDL2 -lSDL2_image

# build options
ifeq ($(TRACE_ENABLE),y)
CFLAGS += -D TRACE_ENABLE
endif

ifeq ($(NVBOARD_ENABLE),y)
CFLAGS += -D NVBOARD_ENABLE
endif

CFLAGS += -D MAX_SIM_TIME=$(MAX_SIM_TIME)
CFLAGS += -D V_TOPNAME=$(V_TOPNAME)

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

all: default

run: $(BIN)
	@$^

wave: run
	gtkwave ./*.vcd ./*.gtkw

show-config:
	$(VERILATOR) -V

check:
	$(VERILATOR) --lint-only -Wall $(VSRCS)

clean:
	-rm -rf obj_dir *.vcd logs *.log *.dmp *.vpd coverage.dat core $(BUILD_DIR)

.PHONY: default all clean run show-config check
