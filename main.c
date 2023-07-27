#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_COURSES 10
#define NUM_CLASSES 10
#define NUM_LEVELS 7
#define NUM_LEN 15
#define CELL_LEN 11
#define STUDENTS_FILE "students.txt"


//------------ structs ------------//
typedef struct {
    char fname[NUM_LEN];
    char lname[NUM_LEN];
    char cellphone[CELL_LEN];
    int courses_grades[NUM_COURSES];
    int level, class; // for now, not sure if it will stay
    struct Student* next;
    struct Student* prev;
} Student;

typedef struct {
    Student* DB[NUM_LEVELS][NUM_CLASSES];
} School;


//typedef struct { for now, not sure if it will stay
//    int name;
//    int grade;
//} Course;

//------------ global variables ------------//
static School school;

//------------ memory functions ------------//
void freeSchool(){
    int i, j;
    for (i = 0; i < NUM_LEVELS; i++) {
        for (j = 0; j < NUM_CLASSES; j++) {
            Student* curr = (Student *) school.DB[i][j];
            while (curr != NULL) {
                Student* next = (Student *) curr->next;
                free(curr);
                curr = next;
            }
        }
    }
}

void freeStudent(Student* stud){
    int class = stud->class, level = stud->level;

    if(stud == NULL){
        return;
    }
    if(stud->prev == NULL){ // first student in class
        school.DB[level][class] = (Student *) stud->next;
    }
    if(stud->next == NULL){ // last student in class
        ((Student*) stud->prev)->next = NULL;
    }
    else {                 // not first student in class
        ((Student*) stud->prev)->next = stud->next;
        ((Student*) stud->next)->prev = stud->prev;
    }
    free(stud);
}

void programFailed(char* msg){
    perror(msg);
    freeSchool();
    exit(-1);
}

Student* allocateStudent(){
    Student* stud = malloc(sizeof(Student));
    if(stud == NULL) {
        programFailed("Error allocating memory for student\n");
    }
    stud->next = stud->prev = NULL;
    return stud;
}

FILE* fileopen(const char* filename){

    FILE* fp;
    fp = fopen(filename, "r");
    if(fp == NULL){
        perror("Can not open input file\n");
        exit(-1);
    }
    return fp;
}

void clearStdin(){
    while ((getchar()) != '\n'); // clear stdin
}

Student* searchInClass(Student* class, const char fname[NUM_LEN], const char lname[NUM_LEN]){
    Student* stud = class;
    while(stud != NULL && (strcmp(stud->fname, fname) != 0 || strcmp(stud->lname, lname) != 0)){
        stud = (Student *) stud->next;
    }
    return stud;
}

//------------ DB functions ------------//
void parseStudentFromFileLine(char* line){
    int offset = 0;

    Student* stud = allocateStudent();
    int scannedItemsNum = sscanf(line, "%s %s %s %d %d%n", stud->fname, stud->lname, stud->cellphone, &stud->level, &stud->class, &offset);
    if(scannedItemsNum != 5){
        programFailed("Error reading student info\n");
    }

    for(int i = 0, n = 0; i < NUM_COURSES; i++){
        scannedItemsNum = sscanf(line + offset, "%d%n", &stud->courses_grades[i], &n);
        if(scannedItemsNum != 1) {
            programFailed("Error reading grade for course %d\n");
        }
        offset += n;
    }

    //TODO: implement sorting insertion  by names
    Student * currHead = school.DB[stud->level][stud->class];
    stud->next = (struct Student *) currHead;
    if(currHead != NULL){
        currHead->prev = (struct Student *) stud;
    }
    school.DB[stud->level][stud->class] = stud;
}

void initDB(){
    FILE* fp = fileopen(STUDENTS_FILE);

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        parseStudentFromFileLine(line);
    }

    free(line);
    fclose(fp);
}

void printDB(){
    for(int i = 0; i < NUM_LEVELS; i++){
        for(int j = 0; j < NUM_CLASSES; j++){
            printf("Level %d, Class %d:\n", i, j);
            Student* curr = (Student *) school.DB[i][j];
            while(curr != NULL){
                printf("%s %s %s ", curr->fname, curr->lname, curr->cellphone);
                for(int k = 0; k < NUM_COURSES; k++){
                    printf("%d ", curr->courses_grades[k]);
                }
                printf("\n");
                curr = (Student *) curr->next;
            }
        }
    }
}

void printStudent(const Student* stud){
    printf("Student info:\n");
    printf("First name: %s\n", stud->fname);
    printf("Last name: %s\n", stud->lname);
    printf("Cellphone: %s\n", stud->cellphone);
    printf("Level: %d\n", stud->level);
    printf("Class: %d\n", stud->class);
    printf("Courses grades: ");
    for(int i = 0; i < NUM_COURSES; i++){
        printf("%d ", stud->courses_grades[i]);
    }
    printf("\n");
}

//------------ menu functions ------------//
void printMenu(){
    printf("Choose task:\n");
    printf("1. Add student\n");
    printf("2. Remove student\n");
    printf("3. Update student\n");
    printf("4. Search student by name\n");
    printf("5. Search excellent students\n");
    printf("6. Search unstable students\n");
    printf("7. Calculate level average\n");
    printf("8. Print DB\n");
    printf("0. Exit\n");
    printf("Your choice: ");
}

void addStudent(){
    printf("Enter student info:\n First name, Last name, Cellphone, Level, Class, Grades of the classes courses\n");

    char *line = NULL;
    size_t len = 0;
    if(getline(&line, &len, stdin) == -1){
        programFailed("Error reading student info\n");
    }
    parseStudentFromFileLine(line);
    free(line);
}

void removeStudent(){
    char fname[NUM_LEN], lname[NUM_LEN];
    int level, class;

    printf("please enter student full-name, level and class: \n");
    if(scanf("%s %s %d %d", fname, lname, &level, &class) != 4){
        programFailed("Error reading student info\n");
    }
    clearStdin();
    if(level < 0 || level > NUM_LEVELS || class < 0 || class > NUM_CLASSES){
        printf("Invalid level or class\n");
        return;
    }

    Student* stud = searchInClass(school.DB[level][class], fname, lname);

    if(stud == NULL){
        printf("Student not found\n");
    }
    else{
        freeStudent(stud);
    }
}

Student* searchStudent(){
    // read student from user
    char fname[NUM_LEN], lname[NUM_LEN];

    printf("please enter student full-name: \n");
    if(scanf("%s %s", fname, lname) != 2){
        programFailed("Error reading student info\n");
    }
    clearStdin();

    // search student in DB
    for (int i = 0; i < NUM_LEVELS; i++) {
        for (int j = 0; j < NUM_CLASSES; j++) {
            Student* stud = searchInClass(school.DB[i][j], fname, lname);
            if(stud != NULL){
                printStudent(stud);
                return stud;
            }
        }
    }

    printf("Student not found\n");
    return NULL;
}

void updateStudent(){
    Student* stud = searchStudent();

    if(stud == NULL)
        return;

    addStudent();
    freeStudent(stud);
}

void executeTask(const int task){

    switch(task){
        case 0: //exit
            return;
        case 1:
            addStudent();
            break;
        case 2:
            removeStudent();
            break;
        case 3:
            updateStudent();
            break;
        case 4:
            searchStudent();
            break;
        case 5:
            printf("Search excellent students\n");
            break;
        case 6:
            printf("Search unstable students\n");
            break;
        case 7:
            printf("Calculate level average\n");
            break;
        case 8:
            printDB();
            break;
        default:
            printf("Invalid task\n");
            break;
    }

}

void menu(){
    int task, inpStatus;

    do{
        printMenu();
        inpStatus = scanf("%d", &task);
        clearStdin();
        if(inpStatus != 1){
            printf("Invalid input\n");
            continue;
        }
        executeTask(task);
    }while(task != 0);
}


int main(){

    initDB();
  //  printDB();
    menu();
    freeSchool();

    return 0;
}
