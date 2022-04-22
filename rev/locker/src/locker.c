#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/random.h>
#include <linux/limits.h>
#include <unistd.h>
#include "chacha20.h"
#include "sha256.h"

#define CHUNK_SIZE 1024
#define DIR "imp0rt4nt_f1l3s"

typedef struct {
    uint8_t key[32];
    uint32_t counter;
    uint8_t nonce[12];
    uint8_t inc;
} chacha_ctx;

chacha_ctx cipher_a, cipher_b;

void init_ctx(chacha_ctx *ctx, uint8_t inc) {
    ctx->counter = 0;
    if(getrandom(ctx->key, 32, 0) < 0) {
        perror("getrandom");
        exit(1);
    }
    if(getrandom(ctx->nonce, 12, 0) < 0) {
        perror("getrandom");
        exit(1);
    }
    
    ctx->inc = inc;
}

void encrypt_chunk(char *buf, size_t len) {
    size_t i;
    for(i = 0; i < len; i++) {
        ChaCha20XOR(cipher_a.key, cipher_a.counter, cipher_a.nonce, &buf[i], &buf[i], 1);
        ChaCha20XOR(cipher_b.key, cipher_b.counter, cipher_b.nonce, &buf[i], &buf[i], 1);

        cipher_a.counter += cipher_a.inc;
        cipher_a.counter %= CHUNK_SIZE;

        cipher_b.counter += cipher_b.inc;
        cipher_b.counter %= CHUNK_SIZE;
    }
    uint8_t old = cipher_a.inc;
    cipher_a.inc = cipher_b.inc;
    cipher_b.inc = old;

    cipher_a.counter = 0;
    cipher_b.counter = 0;
}

void encrypt_file(char *filename) {
    char src_path[PATH_MAX], dst_path[PATH_MAX];
    char buf[CHUNK_SIZE] = {0, };

    snprintf(src_path, sizeof(src_path), "%s/%s", DIR, filename);
    snprintf(dst_path, sizeof(dst_path), "%s.crewcrypt", src_path);

    int in_fd = open(src_path, O_RDONLY);
    if(in_fd < 0) {
        perror("open");
        exit(1);
    }

    unlink(dst_path);
    int out_fd = open(dst_path, O_WRONLY | O_CREAT, 0666);

    while(1) {
        memset(buf, 0, sizeof(buf));
        int len = read(in_fd, buf, CHUNK_SIZE);
        if(len <= 0) {
            break;
        }

        encrypt_chunk(buf, len);
        if(write(out_fd, buf, len) < len) {
            perror("write");
            exit(1);
        }
    }

    close(in_fd);
    unlink(src_path);

    if(write(out_fd, cipher_a.nonce, sizeof(cipher_a.nonce)) < sizeof(cipher_a.nonce)) {
        perror("write");
        exit(1);
    }

    // a mistake on copy-paste below, should've been cipher_b.nonce
    // does not affect the ability to solve the challenge
    if(write(out_fd, cipher_a.nonce, sizeof(cipher_b.nonce)) < sizeof(cipher_b.nonce)) {
        perror("write");
        exit(1);
    }

    close(out_fd);
}

void sha256_buf(char *hash, char *src, size_t len) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, src, len);
    sha256_final(&ctx, (BYTE *)hash);
}

void rekey(chacha_ctx *cipher) {
    char buf[CHUNK_SIZE];
    char new_key[32];
    char new_nonce[32];

    memset(buf, 0, sizeof(buf));

    for(int i = 0; i < sizeof(buf); i++) {
        ChaCha20XOR(cipher->key, cipher->counter, cipher->nonce, &buf[i], &buf[i], 1);
        cipher->counter += 1;
    }

    sha256_buf(new_key, buf, CHUNK_SIZE / 2);
    sha256_buf(new_nonce, &buf[512], CHUNK_SIZE / 2);

    memcpy(cipher->key, new_key, sizeof(cipher->key));
    memcpy(cipher->nonce, new_nonce, sizeof(cipher->nonce));
}

int main(int argc, char*argv[]) {
    init_ctx(&cipher_a, 1);
    init_ctx(&cipher_b, 2);

    struct dirent **namelist;
    int n = scandir(DIR, &namelist, NULL, alphasort);
    if(n < 0) {
        perror("scandir");
        exit(1);
    }

    for(int i = 0; i < n; i++) {
        char *filename = namelist[i]->d_name;
        if(!strcmp(filename, ".") || !strcmp(filename, "..") || !strcmp(strrchr(filename, '.'), ".crewcrypt")) continue;
        encrypt_file(filename);
        rekey(&cipher_a);
        rekey(&cipher_b);
    }

}