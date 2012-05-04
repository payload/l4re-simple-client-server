ifndef L4DIR
  $(error Set L4DIR, please.)
endif
ifndef O
  $(error Set O, please.)
endif

L4DIR := $(abspath $(L4DIR))
O := $(abspath $(O))
QEMU_OPTIONS += -nographic -no-reboot
MODULE_SEARCH_PATH += $(L4DIR)/conf/platforms
export

pwd = $(abspath .)
name = $(subst $(abspath ..)/,,$(pwd))
src = $(pwd)/src
entry-name = $(subst \ ,-,$(name))
ned.lua = $(L4DIR)/conf/platforms/$(entry-name).lua
modules.list = $(L4DIR)/conf/modules.list

all: fast-clean
	ln -s $(src)/ned.lua $(ned.lua)
	echo entry $(entry-name)		>> $(modules.list)
	echo roottask moe rom/$(entry-name).lua	>> $(modules.list)
	echo module $(entry-name).lua		>> $(modules.list)
	cat $(src)/modules.entry		>> $(modules.list)
	echo \#end entry $(entry-name)		>> $(modules.list)
	$(MAKE) -C $(src)
	$(MAKE) -C $(O) qemu E=$(entry-name)

fast-clean:
	rm -f $(ned.lua)
	sed -i '/^entry $(entry-name)/,/^#end entry $(entry-name)/d' $(modules.list)

clean: fast-clean
	$(MAKE) -C $(src) clean
