# SPDX-License-Identifier: GPL-2.0
obj-m += ps/
obj-m += hello/
obj-m += scull/
obj-m += sleepy/
obj-m += scullp/
obj-m += sculld/

KERNDIR ?= /lib/modules/$(shell uname -r)/build

all default: modules
install: modules_install
modules modules_install help clean:
	$(MAKE) -C $(KERNDIR) M=$(shell pwd) $@
load: modules_install
	for MODULE in $(patsubst %/,%,$(obj-m)); do modprobe $$MODULE; done
unload:
	for MODULE in $(patsubst %/,%,$(obj-m)); do rmmod $$MODULE; done

TARGETS = scull
TARGETS += scullp
TARGETS += sculld
run_tests check: kselftest
kselftest:
	@for TARGET in $(TARGETS); do                                  \
		$(MAKE) OUTPUT=$(shell pwd)/$$TARGET/tests            \
			CFLAGS="-I$(KERNDIR)/tools/testing/selftests" \
			-C ./$$TARGET/tests/ run_tests;               \
	done
kselftest-clean:
	@for TARGET in $(TARGETS); do                                  \
		$(MAKE) OUTPUT=$(shell pwd)/$$TARGET/tests            \
			CFLAGS="-I$(KERNDIR)/tools/testing/selftests" \
			-C ./$$TARGET/tests/ clean;                   \
	done
