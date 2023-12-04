#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"

char *data_type_str[] ={"CHAR","INT32","UINT8","UINT32","FLOAT","DOUBLE","OBJ_PTR","OBJ_STRUCT"};

struct_db_t *struct_db = NULL;
void print_struct_info (struct_info_t *struct_info);
void print_struct_db();

int struct_database_add(struct_info_t *struct_info)
{
	if(!struct_db) {
		struct_db = (struct_db_t*) calloc(1,sizeof(struct_db_t));
		if(!struct_db) return 1;
	}

	if(!struct_db->head) {
		struct_db->head = struct_info;
		struct_db->count = 1;
	}
	else
	{
		struct_info->next = struct_db->head;
		struct_db->head = struct_info;
		struct_db->count++;
	}
	return 0;
}

int struct_register(char *struct_name,unsigned int struct_size,unsigned int field_num,field_info_t *fields_array) {
	struct_info_t *info = (struct_info_t*) calloc(1,sizeof(struct_info_t));
	if(!info) {
		printf("memory allocation failed\n");
		return 1;
	}
	info->next = NULL;
	strncpy(info->name,struct_name,MAX_STRUCTURE_NAME_LENGTH);
	info->size = struct_size;
	info->fields_num = field_num;
	info->fields = fields_array;
	return struct_database_add(info);
}

struct_info_t* struct_database_look_up(char *struct_name)
{
	if(!struct_db) return NULL;
	struct_info_t *node = struct_db->head;
	while(node) {
		if(!strcmp(node->name,struct_name))
			return node;
		node = node->next;
	}
	return NULL;
}

void print_struct_info(struct_info_t *struct_info)
{
	if(!struct_info) {
		printf("This structure is nothing to print\n");
		return;
	}
	
	printf("|----------------------------------------------|\n"); //40
	printf("|%-10s| size = %-8d| fields = %-8d|\n",struct_info->name,struct_info->size,struct_info->fields_num);
	printf("|----------------------------------------------|\n"); //40
	field_info_t *field = struct_info->fields;
	int fieldnum;
	for(fieldnum=0;fieldnum<struct_info->fields_num;fieldnum++)
	{
		printf("%-11s|%-3d%-15s| ","",fieldnum,field->name);
		printf("data type = %-8s| ",data_type_str[fieldnum,field->dtype]);
		printf("size = %-8d| ",field->size);
		printf("offset = %-8d| ",field->offset);
		printf("nested struct = %-15s|\n",field->nested_struct);
		printf("%-11s|-------------------------------------------------------------------------------------------------------------|\n","");
		
		field++;
	}
	printf("\n");
}

void print_struct_db() {
	if(!struct_db) return;
	printf("print structure database\n");
	printf("Number of registered structure is %d\n",struct_db->count);
	
	int currentcount = 1;
	struct_info_t *node = struct_db->head;
	while(node) {
		printf("Structure no = %d (%p)\n",currentcount++,node);
		print_struct_info(node);
		node = node->next;
	}
}


//object-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
object_db_t *object_db = NULL;

static void object_database_add(object_info_t* object_info) {
	if(!object_db) {
		object_db = (object_db_t*) calloc(1,sizeof(object_db_t));
		if(!object_db) {
			printf("object database allocate memory fail\n");
			return;
		}
	}
	
	object_info_t *head = object_db->head;
	if(head) {
		object_info->next = head;
		object_db->head = object_info;
		object_db->count++;
	}
	else {
		object_db->head = object_info;
		object_db->count = 1;
	}
}

void* jalloc(char *struct_name, int units) {
	struct_info_t* struct_info = (struct_info_t*) struct_database_look_up(struct_name);
	if(!struct_info) {
		printf("there is no %s struct",struct_name);
		return NULL;
	}
	
	object_info_t* object_info = (object_info_t*) calloc(1, sizeof(object_info));
	if(!object_info) {
		printf("object info allocate memory fail\n");
		return NULL;
	}
	
	void *ptr = calloc(units,struct_info->size);
	if(!ptr) {
		printf("object allocate memory fail\n");
		free(object_info);
		return NULL;
	}
	
    object_info->next = NULL;
    object_info->ptr = ptr;
    object_info->units = units;
    object_info->struct_info = struct_info;
	object_database_add(object_info);
	return ptr;
}

void print_object_info(object_info_t *obj_info) {
	if(!obj_info) return;
	printf("|----------------------------------------------------------------------------------------|\n");
    printf("|ptr = %-10p | next = %-15p | units = %-4d | struct_name = %-10s |\n",obj_info->ptr,obj_info->next,obj_info->units,obj_info->struct_info->name);
	printf("|----------------------------------------------------------------------------------------|\n");
}

void print_object_db() {
	if(!object_db) return;
	printf("Print object database\n");
	
	object_info_t *node = object_db->head;
	int i;
	for(i = 0;i<object_db->count;i++) {
		print_object_info(node);
		node = node->next;
	}
	
}
