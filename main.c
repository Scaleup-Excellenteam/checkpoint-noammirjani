#include <stdio.h>
#include <stdlib.h>

#define NUM_COURSES 10
#define NUM_CLASSES 10
#define NUM_LEVELS 7
#define NUM_LEN 15
#define STUDENTS_FILE "students.txt"

struct Course {
    int name; // enum for now
    int grade;
};

struct Student {
    char name[NUM_LEN];
    char lname[NUM_LEN];
    int cell;

    struct Course courses[NUM_COURSES];
    struct Student* next; //use doubly linked list for more efficient in actions
    struct Student* prev;
};


struct School {
    struct Student* DB[NUM_LEVELS][NUM_CLASSES];
};

static struct School S;
enum courses_names_enum {
    MATH, PHYSICS, CHEMISTRY, BIOLOGY, ENGLISH, HISTORY,
    GEOGRAPHY, ART, MUSIC, SPORTS
};

void init_db(){
    FILE* fp = fopen(STUDENTS_FILE, "r");
    if(fp == NULL){
        printf("Error opening file %s\n", STUDENTS_FILE);
        return;
    }

    int level, class;

    while(!feof(fp)){
        struct Student* stud = malloc(sizeof(struct Student));
        fscanf(fp, "%s %s %d %d %d", stud->name, stud->lname, &stud->cell, &level, &class);

        int i;
        for(i = 0; i < NUM_COURSES; i++){
            stud->courses[i].name = i;
            fscanf(fp, "%d", &stud->courses[i].grade);
        }

        if(S.DB[level][class] == NULL) {
            stud->next = NULL;
            stud->prev = NULL;
            S.DB[level][class] = stud;
        }
        else {
            S.DB[level][class]->prev = stud;
            stud->next = S.DB[level][class];
            stud->prev = NULL;
            S.DB[level][class] = stud;
        }
    }
    fclose(fp);
}


void print_db(){
    for (int i = 0; i < NUM_LEVELS; i++) {
        for (int j = 0; j < NUM_CLASSES; j++) {
            printf("Level %d, Class %d:\n", i, j);
            struct Student *student = S.DB[i][j];
            while (student != NULL) {
                printf("Student Name: %s %s, Cell: %d\n", student->name, student->lname, student->cell);
                for (int k = 0; k < NUM_COURSES; k++) {
                    printf("Course %d Grade: %d\n", student->courses[k].name, student->courses[k].grade);
                }
                student = student->next;
            }
        }
    }
}

int main(){

    init_db();
    print_db();

    return 0;
}
