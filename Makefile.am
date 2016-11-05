# Makefile.am by Marshall Whittaker / oxagast

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
#SUBDIRS = tests
bin_PROGRAMS = rotor
#noinst_LIBRARIES = libbz2.a libpasswdqc.a libprogressbar.a libyescrypt.a libntru.a
#libbz2_a_SOURCES = bzlib/*.c bzlib/*.h
#libpasswdqc_a_SOURCES = passwdqc-1.3.1/pwqgen.c passwdqc-1.3.1/passwdqc_memzero.c passwdqc-1.3.1/concat.c passwdqc-1.3.1/passwdqc_check.c passwdqc-1.3.1/passwdqc_load.c passwdqc-1.3.1/passwdqc_parse.c passwdqc-1.3.1/passwdqc_random.c passwdqc-1.3.1/wordset_4k.c
#libprogressbar_a_SOURCES = progressbar/src/*.c progressbar/include/*.h
#libprogressbar_a_LDFLAGS = -Iprogressbar/include
#libyescrypt_a_SOURCES = zefcrypt/yescrypt-best.c zefcrypt/yescrypt-common.c zefcrypt/blake256.c
#libyescrypt_a_LIDADD = -fopenmp
#libntru_a_SOURCES = libntru/src/bitstring.c libntru/src/encparams.c libntru/src/hash.c libntru/src/idxgen.c libntru/src/key.c libntru/src/mgf.c libntru/src/ntru.c libntru/src/poly.c libntru/src/rand.c libntru/src/arith.c libntru/src/sha1.c libntru/src/sha2.c libntru/src/nist_ctr_drbg.c libntru/src/rijndael.c err.h ntru.h
#libntru_a_LIBADD = -lrt
#libskein_a_sources = skein/Atmel_AVR.c  skein/brg_endian.h  skein/brg_types.h  skein/SHA3api_ref.c  skein/SHA3api_ref.h  skein/skein_block.c  skein/skein.c  skein/skein_debug.c  skein/skein_debug.h  skein/skein.h  skein/skein_port.h  skein/skein_rot_search2.c  skein/skein_test.c
rotor_SOURCES = src/rotor.c  src/rotor-console.c  src/rotor-console.h  src/rotor-crypt.c  src/rotor-crypt.h  src/rotor-extra.c  src/rotor-extra.h  src/rotor.h  src/rotor-keys.c  src/rotor-keys.h  src/salsa20.c  src/salsa20.h src/shake.c  src/shake.h libntru/src/ntru.h include/passwdqc.h include/yescrypt.h zefcrypt/yescrypt-common.h zefcrypt/yescrypt-ref.c zefcrypt/sha256.c include/progressbar.h libntru/src/encparams.h libntru/src/encparams.c libntru/src/arith.c libntru/src/arith.h libntru/src/hash.h libntru/src/hash.c libntru/src/sph_sha2.h libntru/src/sha2.c libntru/src/sph_sha1.h libntru/src/sha1.c include/key.h libntru/src/key.c libntru/src/poly.c libntru/src/rand.h libntru/src/rand.c libntru/src/ntru.c libntru/src/idxgen.c libntru/src/bitstring.c libntru/src/bitstring.h libntru/src/nist_ctr_drbg.h libntru/src/nist_ctr_drbg.c libntru/src/nist_aes_rijndael.h libntru/src/rijndael.c libntru/src/rijndael.h libntru/src/mgf.h libntru/src/mgf.c progressbar/src/progressbar.c passwdqc-1.3.1/passwdqc_parse.c passwdqc-1.3.1/passwdqc_memzero.c passwdqc-1.3.1/concat.c passwdqc-1.3.1/passwdqc_check.c passwdqc-1.3.1/passwdqc_load.c passwdqc-1.3.1/passwdqc_random.c passwdqc-1.3.1/wordset_4k.c
rotor_LDADD = -lcrypto -lm -ltermcap -lomp
rotor_CFLAGS = -O2
rotor_LDFLAGS = -Ilibntru/src -L/usr/local/lib -I./ -Iinclude
