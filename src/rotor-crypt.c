/*****************************************************************************
 * (c) 2016 BSD 2 clause adouble42/mrn@sdf                                   *
 * rotor - "If knowledge can create problems, it is not through ignorance    * 
 * that we can solve them." -- isaac asimov                                  *
 *                                                                           *
 * rotor-crypt.c - encryption and decryption functions                       *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include "ntru.h"
#include "passwdqc.h"
#include "shake.h"
#include "rotor.h"
#include "rotor-crypt.h"
#include "rotor-keys.h"
#include "progressbar.h"

#ifdef __ROTOR_MLOCK
#include <sys/mman.h>
#endif

/*
 * rotor-crypt.c - encryption and decryption functions
 * 
 * rotor-decrypt-file: decrypt a file given KeyPair kr, src, dst
 *
 */

void rotor_decrypt_file(NtruEncKeyPair kr, char *sfname, char *ofname) {
  NtruRandGen rng_sk = NTRU_RNG_DEFAULT;
  NtruRandContext rand_sk_ctx;
  uint8_t decp[NTRU_ENCLEN];
  uint8_t dec[NTRU_ENCLEN];
  uint8_t shake_key[170];
  uint8_t stream_block[170];
  uint8_t stream_in[170];
  uint8_t stream_final[170];
  struct fileHeader myInfo;
  const void *decptr = (void *) decp;
  int offset, xx,  blocks, remainder;
  uint16_t dec_len;
  FILE *input, *output;

#ifdef __ROTOR_MLOCK
  mlock(&kr, sizeof(NtruEncKeyPair));
  mlock(&rng_sk, sizeof(NtruRandGen));
  mlock(&rand_sk_ctx, sizeof(NtruRandContext));
  mlock(&decp, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&dec, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&shake_key, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&stream_block, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&stream_in, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&stream_final, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&myInfo, sizeof(struct fileHeader));
#endif

  if (ntru_rand_init(&rand_sk_ctx, &rng_sk) != NTRU_SUCCESS)
      printf("rotor_decrypt_file: rng_sk fail\n");
  input = fopen(sfname, "rb");
  output = fopen(ofname, "wb");
  fread(&myInfo,sizeof(struct fileHeader),1,input);
  fread((void *)decptr,sizeof(char),1495,input);
  ntru_decrypt((void *)decptr, &kr, &EES1087EP2,(uint8_t *)shake_key, (uint16_t *) &dec_len);
  printf("decrypting: source -  %s | target - %s\n",sfname, ofname);
  FIPS202_SHAKE256(shake_key, 170, (unsigned char *) &stream_block, 170);
  blocks = myInfo.fileSize;
  remainder = (int) shake_key[1];
  _passwdqc_memzero(&shake_key, sizeof(shake_key));
  int blockCount = 0;
  while ((fread((void *)decptr,sizeof(char),1495, input)) == 1495) {
    blockCount++;
    ntru_decrypt((uint8_t *)decptr, &kr, &EES1087EP2, (uint8_t *) &dec, &dec_len);
    if ((myInfo.fileSize + 1) == blockCount)
      dec_len = remainder;
    for (xx=0;xx<dec_len;xx++) {
      stream_final[xx] = dec[xx] ^ stream_block[xx];
      if (xx & 1) { // with kerosene around
	dec[xx] = '\72'; // there's always something
	stream_block[xx] = '\72'; // to do
      } else {
	dec[xx] = '\65'; // set me
	stream_block[xx] = '\65'; // on fire
      }
    }
    _passwdqc_memzero(&dec, sizeof(dec));
    _passwdqc_memzero(&stream_block, sizeof(stream_block));
    FIPS202_SHAKE256(stream_final, dec_len, (unsigned char *) &stream_block, dec_len);
    fwrite((uint8_t *)stream_final, sizeof(char) * dec_len,1,output);
  }
  ntru_rand_release(&rand_sk_ctx);
#ifdef __ROTOR_MLOCK
  munlock(&kr, sizeof(NtruEncKeyPair));
  munlock(&rng_sk, sizeof(NtruRandGen));
  munlock(&rand_sk_ctx, sizeof(NtruRandContext));
  munlock(&decp, (sizeof(uint8_t)*NTRU_PRIVLEN));
  munlock(&dec, (sizeof(uint8_t)*NTRU_PRIVLEN));
  munlock(&shake_key, (sizeof(uint8_t)*NTRU_PRIVLEN));
  munlock(&stream_block, (sizeof(uint8_t)*NTRU_PRIVLEN));
  munlock(&stream_in, (sizeof(uint8_t)*NTRU_PRIVLEN));
  munlock(&stream_final, (sizeof(uint8_t)*NTRU_PRIVLEN));
  munlock(&myInfo, sizeof(struct fileHeader));
#endif

  fclose(input);
  fclose(output);
}

/* 
 * rotor_encrypt_file: encrypt a file given KeyPair kr, src, dst
 *
 */

void rotor_encrypt_file(NtruEncKeyPair kr, char *sfname, char *ofname){
    NtruRandGen rng_sk = NTRU_RNG_DEFAULT;
    NtruRandContext rand_sk_ctx;
    uint8_t shake_key[170];
    uint8_t stream_block[170];
    uint8_t stream_in[170];
    uint8_t stream_final[170];
    uint8_t enc[NTRU_ENCLEN];
    signed char fbuf[171];
    const void *fptr = (void *) fbuf;
    struct fileHeader myInfo;
    int nt;
    int remainder, xx;
    float blocks;    
    struct stat in_info;
    FILE *input, *output;

#ifdef __ROTOR_MLOCK
  mlock(&kr, sizeof(NtruEncKeyPair));
  mlock(&rng_sk, sizeof(NtruRandGen));
  mlock(&rand_sk_ctx, sizeof(NtruRandContext));
  mlock(&enc, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&fbuf, (sizeof(char)*171));
  mlock(&shake_key, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&stream_block, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&stream_in, (sizeof(uint8_t)*NTRU_PRIVLEN));
  mlock(&stream_final, (sizeof(uint8_t)*NTRU_PRIVLEN));
#endif

    
    stat(sfname, &in_info);
    input = fopen(sfname, "rb");
    output = fopen(ofname, "wb");
    myInfo.fileSize=in_info.st_size;
    lseek((int)input,0, SEEK_SET);
    blocks = floor((myInfo.fileSize / 170));
    remainder = (myInfo.fileSize - (170 * blocks));
    myInfo.fileSize=blocks;
    fwrite(&myInfo, sizeof(struct fileHeader), 1, output);
    if (ntru_rand_init(&rand_sk_ctx, &rng_sk) != NTRU_SUCCESS)
        printf("rng_sk fail\n");
    if (ntru_rand_generate(shake_key, 170, &rand_sk_ctx) != NTRU_SUCCESS) {
      exit(NTRU_ERR_PRNG);
    } else {
      printf("generated 170 byte random key for SHAKE-256 stream\n");
    }
    shake_key[1] = (uint8_t) remainder; // encode actual size of final block
    printf("encrypting: source -  %s | target - %s\n",sfname, ofname);
    if (ntru_encrypt(shake_key, 170, &kr.pub, &EES1087EP2, &rand_sk_ctx, enc) == NTRU_SUCCESS)
	fwrite(enc, sizeof(enc),1, output);
    FIPS202_SHAKE256(shake_key, 170, (uint8_t *) stream_block, 170);
    _passwdqc_memzero(&shake_key, 170);
    while ((nt=fread((void *)fptr,sizeof(char), 170, input))) {
      for (xx=0;xx<nt;xx++) {
	stream_final[xx] = fbuf[xx] ^ stream_block[xx];
	if (xx & 1) { // destroy previous state as we decrypt
	  fbuf[xx] = '\72'; // always something
	  stream_block[xx] = '\72'; // there to
	} else {
	  fbuf[xx] = '\65'; // remind
	  stream_block[xx] = '\65'; // me
	}
      }
      _passwdqc_memzero(&fbuf, sizeof(fbuf));
      _passwdqc_memzero(&stream_block, sizeof(stream_block));
      FIPS202_SHAKE256(fptr, nt, (uint8_t *) stream_block, 170);
      if (ntru_encrypt(stream_final, 170, &kr.pub, &EES1087EP2, &rand_sk_ctx, enc) == NTRU_SUCCESS)
	fwrite(enc, sizeof(enc),1,output);
    }

    rewind(input);
    fseek(input, (0-nt), SEEK_END);
    memset((void *)fptr,0,sizeof(fptr));
    fread((void *)fptr,sizeof(char), nt, input);
    fbuf[nt] = '\0';
    for (xx=0;xx<nt;xx++) {
      stream_final[xx] = fbuf[xx] ^ stream_block[xx];
      if (xx & 1) { // destroy previous state as we decrypt
	fbuf[xx] = '\72'; // always something
	stream_block[xx] = '\72'; // there to
      } else {
	fbuf[xx] = '\65'; // remind
	stream_block[xx] = '\65'; // me
      }
    }
    _passwdqc_memzero(&fbuf, sizeof(fbuf));
    _passwdqc_memzero(&stream_block, sizeof(stream_block));
    
    ntru_encrypt(stream_final, nt, &kr.pub, &EES1087EP2, &rand_sk_ctx, enc);
    fwrite(enc, sizeof(enc),1,output);   
    
    ntru_rand_release(&rand_sk_ctx);
#ifdef __ROTOR_MLOCK
    munlock(&kr, sizeof(NtruEncKeyPair));
    munlock(&rng_sk, sizeof(NtruRandGen));
    munlock(&rand_sk_ctx, sizeof(NtruRandContext));
    munlock(&enc, (sizeof(uint8_t)*NTRU_PRIVLEN));
    munlock(&fbuf, (sizeof(char)*171));
    munlock(&shake_key, (sizeof(uint8_t)*NTRU_PRIVLEN));
    munlock(&stream_block, (sizeof(uint8_t)*NTRU_PRIVLEN));
    munlock(&stream_in, (sizeof(uint8_t)*NTRU_PRIVLEN));
    munlock(&stream_final, (sizeof(uint8_t)*NTRU_PRIVLEN));
#endif

    fclose(input);
    fclose(output);
}
