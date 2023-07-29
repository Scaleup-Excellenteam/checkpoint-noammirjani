#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//--------------------------- defines ---------------------------//
#define NUM_COURSES 10
#define NUM_CLASSES 10
#define NUM_LEVELS 12
#define NUM_LEN 15
#define CELL_LEN 11
#define STUDENTS_FILE "students.txt"
#define OUTPUT_FILE "backupDB.txt"
#define TOP_STUDENTS 10


//--------------------------- structs ---------------------------//
typedef struct Student Student;
typedef struct School School;

struct Student{
    char fname[NUM_LEN];
    char lname[NUM_LEN];
    char cellphone[CELL_LEN];
    int courses_grades[NUM_COURSES];
    int level, class;
    double avg;
    Student* next;
    Student* prev;
};

struct School{
    Student* DB[NUM_LEVELS][NUM_CLASSES];
};


const char* const courses_names[NUM_COURSES] = {
        "Math","Physics","Chemistry","Biology","History",
        "Literature","English","Arabic","Art","Music"
};



//--------------------------- global variables ---------------------------//
static School school;


//--------------------------- memory functions ---------------------------//
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
    if(stud == NULL) return;        // no student

    int class = stud->class, level = stud->level;

    if(stud->prev == NULL){         // first student in class
        school.DB[level][class] = stud->next;
    }
    else if(stud->next == NULL){    // last student in class
        (stud->prev)->next = NULL;
    }
    else {                          // student in the middle
        (stud->prev)->next = stud->next;
        (stud->next)->prev = stud->prev;
    }
    free(stud);
}

void programFailed(const char* msg){
    perror(msg);
    freeSchool();
    exit(EXIT_FAILURE);
}

Student* allocateStudent(){
    Student* stud = malloc(sizeof(Student));
    if(stud == NULL) {
        programFailed("Error allocating memory for student\n");
    }
    stud->next = stud->prev = NULL;
    return stud;
}


//--------------------------- utility functions ---------------------------//
void checkRangeInput(const int var, const int max){
    if(var < 0 || var >= max){
        char message[100];
        sprintf(message, "Invalid input %d is not in the valid range {1, %d}", var, max);
        programFailed(message);
    }
}

FILE* fileopen(const char* filename, const char* mode){
    FILE* fp;
    fp = fopen(filename, mode);
    if(fp == NULL){
        programFailed("Error opening file\n");
    }
    return fp;
}

void clearStdin(){
    while ((getchar()) != '\n');
}

void printStudent(const Student* stud){
    printf("Student info:\n");
    printf("First name: %s\n", stud->fname);
    printf("Last name: %s\n", stud->lname);
    printf("Cellphone: %s\n", stud->cellphone);
    printf("Level: %d\n", stud->level+1);
    printf("Class: %d\n", stud->class+1);
    printf("Courses grades: ");
    for(int i = 0; i < NUM_COURSES; i++){
        printf("%d ", stud->courses_grades[i]);
    }
    printf("\n");
}

void printCourses(){
    printf("Courses:\n");
    for(int i = 0; i < NUM_COURSES; i++){
        printf("%d. %s ", i + 1, courses_names[i]);
    }
    printf("\n");
}

int readValFromUser(const char* type, const int maxVal){
    int val;
    printf("please enter %s: \n", type);

    if(scanf("%d", &val) != 1){
        programFailed("Error reading student info\n");
    }

    val--;
    checkRangeInput(val, maxVal);

    clearStdin();
    return val;
}

double calcStudAvg(const Student* stud){
    double sum = 0;
    for(int i = 0; i < NUM_COURSES; i++){
        sum += stud->courses_grades[i];
    }
    if(NUM_COURSES == 0) return 0;
    return sum / NUM_COURSES;
}


//--------------------------- DB functions -------------------------------//
void parseStudentFromFileLine(char* line){
    int offset = 0;
    Student* stud = allocateStudent();

    //read student info - fname, lname, cellphone, level, class
    int scannedItemsNum = sscanf(line, "%s %s %s %d %d%n", stud->fname, stud->lname, stud->cellphone, &stud->level, &stud->class, &offset);
    if(scannedItemsNum != 5){
        programFailed("Error reading student info\n");
    }

    stud->level--;
    stud->class--;
    checkRangeInput(stud->level, NUM_LEVELS);
    checkRangeInput(stud->class, NUM_CLASSES);

    //read courses grades
    for(int i = 0, n = 0; i < NUM_COURSES; i++){
        scannedItemsNum = sscanf(line + offset, "%d%n", &stud->courses_grades[i], &n);
        if(scannedItemsNum != 1) {
            programFailed("Error reading grade - grade was not found as expected");
        }
        offset += n;
    }

    stud->avg = calcStudAvg(stud);

    //add student to DB (at the head of the list)
    Student * currHead = school.DB[stud->level][stud->class];
    stud->next = currHead;
    if(currHead != NULL){
        currHead->prev = stud;
    }
    school.DB[stud->level][stud->class] = stud;
}

void initDB(){
    FILE* fp = fileopen(STUDENTS_FILE, "r");

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        parseStudentFromFileLine(line);
    }

    free(line);
    fclose(fp);
}

void printDB(FILE* fp){

    for(int i = 0; i < NUM_LEVELS; i++){
        for(int j = 0; j < NUM_CLASSES; j++){
            fprintf(fp, "Level %d, Class %d:\n", i+1, j+1);
            Student* curr = (Student *) school.DB[i][j];
            while(curr != NULL){
                fprintf(fp, "%s %s %s ", curr->fname, curr->lname, curr->cellphone);
                for(int k = 0; k < NUM_COURSES; k++){
                    fprintf(fp, "%d ", curr->courses_grades[k]);
                }
                fprintf(fp, "\n");
                curr = (Student *) curr->next;
            }
        }
    }
}

void printDBtoFile(){
    FILE *fp = fileopen(OUTPUT_FILE, "w");
    printDB(fp);
    fclose(fp);
}

Student* searchInClass(Student* class, const char fname[NUM_LEN], const char lname[NUM_LEN]){
    Student* stud = class;
    while (stud != NULL) {
        if(strcmp(stud->fname, fname) == 0 && strcmp(stud->lname, lname) == 0){
            return stud;
        }
        stud = (Student *) stud->next;
    }
    return stud;
}


//--------------------------- menu functions -----------------------------//
void printMenu(){
    printf("Choose task:\n");
    printf("1. Add student\n");
    printf("2. Remove student\n");
    printf("3. Update student\n");
    printf("4. Search student by name\n");
    printf("5. Search excellent students\n");
    printf("6. Search unstable students\n");
    printf("7. Calculate level average\n");
    printf("8. Print DB to screen\n");
    printf("9. Print DB to file\n");
    printf("10. clear DB\n");
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

    printf("please enter student full-name: \n");
    if(scanf("%s %s", fname, lname) != 2){
        programFailed("Error reading student info\n");
    }
    clearStdin();
    int level = readValFromUser("level", NUM_LEVELS);
    int class = readValFromUser("class", NUM_CLASSES);

    Student* stud = searchInClass(school.DB[level][class], fname, lname);
    if(stud == NULL){
        printf("Student not found\n");
    }
    else{
        freeStudent(stud);
        printf("Student removed\n");
    }
}

Student* searchStudent(){
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

void printTop10InLevel(const int level, Student* top[TOP_STUDENTS], const int course){
    printf("Level %d:\n", level + 1);

    for(int i = 0; i < TOP_STUDENTS; i++){
        if(top[i] != NULL){
            printf("%d. %s %s %d\n", i+15, top[i]->fname, top[i]->lname, top[i]->courses_grades[course]);
        }
    }
}

void findTop10InLevel(const int level, const int course){
    Student* top[TOP_STUDENTS] = {NULL};

    for(int class = 0; class < NUM_CLASSES; class++){
        Student* curr = school.DB[level][class];
        while(curr != NULL){
            int grade = curr->courses_grades[course];
            for(int i = 0; i < TOP_STUDENTS; i++){
                if(top[i] == NULL || top[i]->courses_grades[course] < grade){
                    Student* temp = top[i];
                    top[i] = curr;
                    curr = temp;
                }
            }
        }
    }
    printTop10InLevel(level, top, course);
}

void findTop10ExcellentStudents(){
    printCourses();
    int course = readValFromUser("course", NUM_COURSES);

    for(int i = 0; i < NUM_LEVELS; i++){
        findTop10InLevel(i, course);
    }
}

void calculateLevelAverage(){
    int level = readValFromUser("level", NUM_LEVELS);
    printCourses();
    int course = readValFromUser("course id", NUM_COURSES);

    int sum = 0, count = 0;
    for (int i = 0; i < NUM_CLASSES; i++) {
        Student* stud = (Student *) school.DB[level][i];
        while(stud != NULL){
            sum += stud->courses_grades[course];
            count++;
            stud = (Student *) stud->next;
        }
    }

    if(count == 0){
        printf("No students in level %d\n", level + 1);
    }
    else{
        printf("Level %d, course: %s average: %.3f\n", level + 1, courses_names[course], (float)sum/(float)count);
    }

}

void searchUnstableStudents(){
    printf("Unstable students - students with average lower than 60: \n");

    for(int level = 0; level < NUM_LEVELS; level++){
        for(int class = 0; class < NUM_CLASSES; class++){
            Student* stud = school.DB[level][class];
            while(stud != NULL){
                if(stud->avg < 60) {
                    printf("Level %d, class %d name: %s %s, avg: %.2f\n",
                           stud->level + 1, stud->class + 1, stud->fname, stud->lname, stud->avg);
                }
                stud = stud->next;
            }
        }
    }
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
            findTop10ExcellentStudents();
            break;
        case 6:
            searchUnstableStudents();
            break;
        case 7:
            calculateLevelAverage();
            break;
        case 8:
            printDB(stdout);
            break;
        case 9:
            printDBtoFile();
            break;
        case 10:
            freeSchool();
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
    //  printDB(stdout);
    menu();
    freeSchool();

    return 0;
}
