PKGDIR          ?= .
L4DIR           ?= $(PKGDIR)/../..

TARGET               = simple-server simple-client
REQUIRES_LIBS       := cxx_io cxx_libc_io
SRC_CC_simple-server = server.cc
SRC_CC_simple-client = client.cc

include $(L4DIR)/mk/prog.mk

