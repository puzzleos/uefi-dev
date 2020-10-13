#
# UEFI libefihelpers Makefile
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

OBJS = \
	mem.o print.o

.PHONY: all
all: build

.PHONY: build
build: libefihelpers.a

libefihelpers.a: ${OBJS}
	${AR} rcv $@ $^

.PHONY: clean
clean:
	${RM} -rf *.a *.o
