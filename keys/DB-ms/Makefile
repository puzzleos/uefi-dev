#
# UEFI DB-ms keys Makefile
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

include ../../make.conf

CERTS = ${shell ls *.pem}
ESL_FILE = ${shell basename $$(pwd)}.esl

.PHONY: all
all: build

build: ${ESL_FILE}

%.der: %.pem
	openssl x509 -in $^ -outform DER -out $@

%.esl: %.pem %.guid
	cert-to-efi-sig-list -g $$(cat $$(echo "$^" | awk '{ print $$2 }')) \
		$$(echo "$^" | awk '{ print $$1 }') $@

${ESL_FILE}: ${CERTS}
	${RM} -f $@
	for i in ${CERTS}; do \
		${MAKE} -C . $$(echo $$i | sed 's/\.[^\.]\+$$//').esl; \
	done
	cat *.esl > $@

.PHONY: clean-keys
clean-keys: clean
	${RM} -f *.der *.esl

.PHONY: clean
clean:
	# don't automatically clean the keys, use "clean-keys" instead
