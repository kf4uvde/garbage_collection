#include <stdlib.h>#include <stdio.h>#include "gc.h"typedef struct emp_ {    struct emp_ *manager;	char name[30];    unsigned int id;    unsigned int age;    float salary;} emp_t;typedef struct student_ {    char name[32];    unsigned int rollnum;    unsigned int age;    float GPA;    struct student_ *best_colleage;} student_t;int main(void){        //---------------------------------------------------- init	static field_info_t emp_fields[] = {        FIELD_EXTEND(emp_t, manager, OBJ_PTR, emp_t),        FIELD_EXTEND(emp_t, name, CHAR, 0),        FIELD_EXTEND(emp_t, id, UINT32, 0),        FIELD_EXTEND(emp_t, age, UINT32, 0),        FIELD_EXTEND(emp_t, salary, FLOAT, 0)    };    STRUCT_REGISTER(emp_t, emp_fields);    static field_info_t stud_fiels[] = {        FIELD_EXTEND(student_t, name, CHAR, 0),        FIELD_EXTEND(student_t, rollnum, UINT32, 0),        FIELD_EXTEND(student_t, age, UINT32, 0),        FIELD_EXTEND(student_t, GPA, FLOAT, 0),        FIELD_EXTEND(student_t, best_colleage, OBJ_PTR, student_t)    };    STRUCT_REGISTER(student_t, stud_fiels);    print_struct_db();			//----------------------------------------------------using	student_t *studrct1 = jalloc("student_t",1);	strncpy(studrct1->name, "Jimmy", strlen("Jimmy"));	student_t *studrct2 = jalloc("student_t",1);	strncpy(studrct2->name, "Nora", strlen("Nora"));	student_t *studrct3 = jalloc("student_t",1);	strncpy(studrct3->name, "David", strlen("David"));	print_object_db();		set_object_as_root(studrct1);	studrct1->best_colleage = studrct2;    emp_t *joseph = jalloc("emp_t",2);	strncpy(joseph->name, "Kevin", strlen("Kevin"));	joseph->id = 1;	joseph->age = 99;	joseph->salary = 1.5f;	    Set_all_reachable_obj_to_visited();	report_leaked_objects();    return 0;}