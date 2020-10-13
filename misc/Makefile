#
# UEFI misc tools Makefile
#
# Copyright (c) 2020 Cisco Systems, Inc. <pmoore2@cisco.com>
#

#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

include ../make.conf

HELLO_OBJS = \
	hello.o

CFLAGS +=  -I../libefihelpers 

.PHONY: all
all: build

.PHONY: build
build: hello.efi

../libefihelpers/libefihelpers.a:
	${MAKE} -C ../libefihelpers build

hello.so: ${HELLO_OBJS} ../libefihelpers/libefihelpers.a
	${LD} ${LDFLAGS} ${HELLO_OBJS} -o $@ \
		-lefi -lgnuefi ../libefihelpers/libefihelpers.a

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-${ARCH} $^ $@

.PHONY: clean
clean:
	${RM} -rf *.efi *.so *.o
