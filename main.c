#include <stdio.h>

#define NUM_COURSES 10
#define NUM_CLASSES 10
#define NUM_LEVELS 7
#define NUM_LEN 15
#define STUDENTS_FILE "students.txt"


struct Student {
    char name[NUM_LEN];
    char lname[NUM_LEN];
    int cell;
    struct Courses* courses[NUM_COURSES];

    struct Student* next;
    struct Student* prev;
};


struct School {
    struct Student DB[NUM_LEVELS][NUM_CLASSES];
};


struct Course {
    char name[NUM_LEN];
    int grade;
};


static struct Course C[NUM_COURSES];
static struct School S;


int main(){

    init_db();

    return 0;
}