#
# UEFI utils Makefile
#
# Copyright (c) 2021 Cisco Systems, Inc. <pmoore2@cisco.com>
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

LIBEFIHELPER = ../libefihelpers/libefihelpers.a

CFLAGS += -I../libefihelpers

.PHONY: all
all: build

.PHONY: build
build: hello.efi sbat_read.efi sbat_write.efi

hello.so: hello.o hello.sbat.o ${LIBEFIHELPER}
sbat_read.so: sbat_read.o sbat_read.sbat.o ${LIBEFIHELPER} 
sbat_write.so: sbat_write.o sbat_write.sbat.o ${LIBEFIHELPER}

${LIBEFIHELPER}:
	${MAKE} -C ../libefihelpers build

%.sbat.o: %.sbat.csv
	${CC} -x c -c -o $@ /dev/null
	objcopy \
		--add-section .sbat=$< \
		--set-section-flags .sbat=contents,alloc,load,readonly,data \
		$@

sbat_data.csv: sbat_data.csv.template
	# NOTE: you should supply your own sbat_data.csv file
	cp $< $@

sbat_data.h: sbat_data.csv
	xxd -i $< $@

sbat_write.o: sbat_data.h

.PHONY: clean
clean:
	${RM} -f *.o *.so *.efi
