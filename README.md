# checkpoint-noammirjani
## Student Database Management System
This program manage school student database.

## Installation
1. install libssl-dev:
```bash
sudo apt-get install libssl-dev
```

2. compile and run the encryption and decryption program:
```bash
gcc crypt.c -o crypt -lcrypto
./crypt
```

3. compile and run the student database program:
```bash
gcc main.c -o student_db -lcrypto
./student_db
```
