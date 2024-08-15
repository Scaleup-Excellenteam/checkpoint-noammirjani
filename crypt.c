/**
 *
 *   This program encrypts and decrypts a file using the AES-256-CBC algorithm.
 *
 *   To compile: gcc crypt.c -o crypt -lcrypto
 *   To run: ./crypt
 *
 *   Notice: there is use of the openssl library in this program.
 *   To install openssl in linux: sudo apt-get install libssl-dev
 *   To install openssl in mac: brew install openssl
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>


//--------------------------- defines ---------------------------//
#define STUDENTS_FILE "students.txt"
#define OUTPUT_FILE "backupDB.txt"
#define ENCRYPTED_FILE "encrypted.txt"
#define DECRYPTED_FILE "decrypted.txt"


//--------------------------- memory functions ---------------------------//
void programFailed(const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}


FILE* fileopen(const char* filename, const char* mode){
    FILE* fp;
    fp = fopen(filename, mode);
    if(fp == NULL){
        programFailed("Error opening file\n");
    }
    return fp;
}


void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}


//--------------------------- encryption & decryption ----------------------//
// Encrypts a single line of text
int encryptLine(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len, ciphertext_len;

    // Create and initialise the context
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    // Initialise the encryption operation.
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    // Provide the message to be encrypted, and obtain the encrypted output.
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    // Handle any remaining data and finalize the encryption.
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}


void encrypt(unsigned char key[], unsigned char iv[]) {
    FILE *plaintextFile = fileopen(STUDENTS_FILE, "r");
    FILE *encryptedFile = fileopen(ENCRYPTED_FILE, "wb");

    unsigned char ciphertext[128];
    char plaintext[128]; // Changed from unsigned char to char

    while (fgets(plaintext, sizeof plaintext, plaintextFile) != NULL) {
        int ciphertext_len = encryptLine((unsigned char *)plaintext, strlen(plaintext), key, iv, ciphertext);

        // write the length of the encrypted message to file
        fwrite(&ciphertext_len, sizeof(int), 1, encryptedFile);
        // Write the encrypted message to file
        fwrite(ciphertext, sizeof(unsigned char), ciphertext_len, encryptedFile);
    }

    fclose(plaintextFile);
    fclose(encryptedFile);
}

int decryptLine(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len, plaintext_len;

    // Create and initialise the context
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    // Initialise the decryption operation.
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    // Provide the message to be decrypted, and obtain the plaintext output.
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    // Finalise the decryption. Further plaintext bytes may be written at this stage.
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


void decrypt(unsigned char key[], unsigned char iv[]) {

    FILE *encryptedFile = fileopen(ENCRYPTED_FILE, "rb");
    FILE *decryptedFile = fileopen(DECRYPTED_FILE, "w");

    unsigned char ciphertext[128];
    unsigned char decryptedText[128];
    int ciphertext_len;

    while (fread(&ciphertext_len, sizeof(int), 1, encryptedFile) > 0) {
        fread(ciphertext, sizeof(unsigned char), ciphertext_len, encryptedFile);
        int decryptedTextLen = decryptLine(ciphertext, ciphertext_len, key, iv, decryptedText);
        decryptedText[decryptedTextLen] = '\0';
        fwrite(decryptedText, sizeof(char), decryptedTextLen, decryptedFile);
    }

    fclose(encryptedFile);
    fclose(decryptedFile);
}


void prepareEncryption(){
    /* A 256 bit key */
    unsigned char key[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                            0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
                            0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
                            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31 };

    /* A 128 bit IV */
    unsigned char iv[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                           0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 };

    encrypt(key, iv);
    decrypt(key, iv);
}


int main(){

    prepareEncryption();

    return 0;
}
