# checkpoint-noammirjani
## Student Database Management System
This program manage school student database.

Dependencies
To compile and run this program, you need to have the OpenSSL library installed on your system. Make sure you have the necessary development packages for OpenSSL, including the header files and libraries.

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


## Features
The program provides the following features:
Add Student: Allows users to add a new student to the database, including their personal information, course grades, level, and class.
Remove Student: Allows users to remove a student from the database based on their full name and class information.
Update Student: Enables users to update the information of an existing student in the database, including personal information, course grades, level, and class.
Search Student by Name: Allows users to search for a student in the database based on their full name and view their details.
Search Excellent Students: Finds and displays the top 10 students with the highest grades in a specific course for each level.
Search Unstable Students: Finds and displays students with an average grade lower than 60.
Calculate Level Average: Calculates and displays the average grade for a specific course across all students in a particular level.
Print Database to Screen: Prints the entire student database to the screen.
Print Database to File: Saves the entire student database to a text file named "backupDB.txt".
Clear Database: Clears the entire student database, freeing up the allocated memory.