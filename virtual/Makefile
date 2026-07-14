CC 				= clang
TARGET_ARCH 	?=
SYSROOT 		?=
CROSS_FLAGS 	?=
LDFLAGS 		?=

ifneq ($(TARGET_ARCH),)
	CROSS_FLAGS += -target $(TARGET_ARCH)
	LDFLAGS		+= -target $(TARGET_ARCH) -fuse-ld=lld
endif

ifneq ($(SYSROOT),)
	CROSS_FLAGS += --sysroot=$(SYSROOT)
	LDFLAGS	+= --sysroot=$(SYSROOT)
endif

SRC_DIR = src
TARGET_BASE = prg
SRC = $(wildcard $(SRC_DIR)/*.c)

# Build Configurations
CONFIGS = debug uba point leak thread opt

COMMON_FLAGS = -Wall -Wextra -Werror -Wpedantic -MMD -MP -std=c23 $(CROSS_FLAGS)

debug_dir 		= dbg
debug_suffix 	= -dbg
debug_cflags 	= $(COMMON_FLAGS) -O0 -g
debug_ldflags 	= 

uba_dir 		= uba
uba_suffix 		= -uba
uba_cflags 		= $(COMMON_FLAGS) -O0 -fsanitize=undefined,address
uba_ldflags 	= -fsanitize=undefined,address

point_dir 		= point
point_suffix 	= -point
point_cflags 	= $(COMMON_FLAGS) -O0 -fsanitize=address,pointer-compare
point_ldflags 	= -fsanitize=address,pointer-compare

leak_dir 		= leak
leak_suffix 	= -leak
leak_cflags 	= $(COMMON_FLAGS) -O0 -fsanitize=leak
leak_ldflags 	= -fsanitize=leak

thread_dir 		= thread
thread_suffix 	= -thread
thread_cflags 	= $(COMMON_FLAGS) -O0 -fsanitize=thread
thread_ldflags 	= -fsanitize=thread

opt_dir 		= opt
opt_suffix 		= 
opt_cflags 		= $(COMMON_FLAGS) -fPIE -O2
opt_ldflags 	= 

define run_binary
	$(if $(TARGET_ARCH), @echo "Cross-compiled binary created. Skipping execution.",./$<)
endef

.PHONY: $(CONFIGS) all clean

define CONFIG_template

objs_$(1)		:= $$(patsubst $$(SRC_DIR)/%.c,$$($(1)_dir)/%.o,$$(SRC))
deps_$(1)		:= $$(objs_$(1):.o=.d)
target_$(1)		:= $$(TARGET_BASE)$$($(1)_suffix)

$(1): $$(target_$(1))
	$$(call run_binary)

$$(target_$(1)): $$(objs_$(1))
	$$(CC) $$(LDFLAGS) -o $$@ $$^ $$($(1)_ldflags)

$$($(1)_dir)/%.o: $$(SRC_DIR)/%.c
	@mkdir -p $$(@D)
	$$(CC) $$($(1)_cflags) -c $$< -o $$@

ALL_DIRS    := $(ALL_DIRS) $$($(1)_dir)
ALL_TARGETS := $(ALL_TARGETS) $$(target_$(1))

-include $$(deps_$(1))
endef

$(foreach config,$(CONFIGS),$(eval $(call CONFIG_template,$(config))))

all: $(CONFIGS)

clean:
	rm -rf $(ALL_DIRS) $(ALL_TARGETS)
