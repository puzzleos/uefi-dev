#
# UEFI keys Makefile
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

SETUP_OBJS = \
	sb_setup.o

CFLAGS += -I../libefihelpers

.PHONY: all
all: build

build: sb_setup.efi

../libefihelpers/libefihelpers.a:
	${MAKE} -C ../libefihelpers build

sb_setup.o: PK.h KEK.h DB.h

sb_setup.so: ${SETUP_OBJS} ../libefihelpers/libefihelpers.a
	${LD} ${LDFLAGS} ${SETUP_OBJS} -o $@ \
		-lefi -lgnuefi ../libefihelpers/libefihelpers.a

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-${ARCH} $^ $@

%.h: %.auth
	xxd -i $< > $@

%.der: %.pem
	openssl x509 -in $^ -outform DER -out $@

%.esl: %.pem %.guid
	cert-to-efi-sig-list -g $$(cat $$(echo "$^" | awk '{ print $$2 }')) \
		$$(echo "$^" | awk '{ print $$1 }') $@

PK.auth: PK.esl PK.key PK.pem
	sign-efi-sig-list -k PK.key -c PK.pem \
		$$(echo $< | sed 's/\.[^\.]\+$$//') $< $@

KEK.auth: KEK.esl PK.key PK.pem
	sign-efi-sig-list -k PK.key -c PK.pem \
		$$(echo $< | sed 's/\.[^\.]\+$$//') $< $@

DB.auth: DB.esl KEK.key KEK.pem
	sign-efi-sig-list -k KEK.key -c KEK.pem \
		$$(echo $< | sed 's/\.[^\.]\+$$//') $< $@

PK.guid: PK.pem
PK.key: PK.pem
PK.pem:
	openssl req -new -x509 -newkey rsa:2048 -days 3650 -nodes -sha256 \
		-subj "/CN=OVMF SB TEST PK/" \
		-keyout PK.key -out PK.pem
	uuidgen -r > PK.guid

KEK.guid: KEK.pem
KEK.key: KEK.pem
KEK.pem:
	openssl req -new -x509 -newkey rsa:2048 -days 3650 -nodes -sha256 \
		-subj "/CN=OVMF SB TEST KEK/" \
		-keyout KEK.key -out KEK.pem
	uuidgen -r > KEK.guid

DB-rand.guid: DB-rand.pem
DB-rand.key: DB-rand.pem
DB-rand.pem:
	openssl req -new -x509 -newkey rsa:2048 -days 3650 -nodes -sha256 \
		-subj "/CN=OVMF SB TEST db_random/" \
		-keyout DB-rand.key -out DB-rand.pem
	uuidgen -r > DB-rand.guid

DB.esl: DB-ms.esl DB-extra.esl
	cat $$(find . -name "DB-*.esl") > $@

DB-ms.esl: DB-ms
	diff -q DB-ms/DB-ms.esl $@ || cp DB-ms/DB-ms.esl $@

.PHONY: DB-ms
DB-ms:
	${MAKE} -C DB-ms build

DB-extra.esl: DB-extra
	diff -q DB-extra/DB-extra.esl $@ || cp DB-extra/DB-extra.esl $@

.PHONY: DB-extra
DB-extra:
	${MAKE} -C DB-extra build

.PHONY: clean-keys
clean-keys: clean
	${RM} -f *.pem *.der *.key *.guid *.esl *.auth PK.h KEK.h DB.h
	for i in DB-ms DB-extra; do \
		${MAKE} -C $$i clean-keys; \
	done

.PHONY: clean
clean:
	# don't automatically clean the keys, use "clean-keys" instead
	${RM} -f sb_setup.efi *.so *.o
