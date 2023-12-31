#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gc.h"


char *data_type_str[] ={"CHAR","INT32","UINT8","UINT32","FLOAT","DOUBLE","OBJ_PTR","OBJ_STRUCT"};
//struct & field-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct_db_t *struct_db = NULL;
void print_struct_info (struct_info_t *struct_info);
void print_struct_db();

int struct_database_add(struct_info_t *struct_info)
{
	if(!struct_db) {
		struct_db = (struct_db_t*) calloc(1,sizeof(struct_db_t));
		if(!struct_db) return 1;
		STRUCT_REGISTER(int , 0);
		STRUCT_REGISTER(float , 0);
		STRUCT_REGISTER(double , 0);
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

void print_object_info(object_info_t *obj_info) {
	if(!obj_info) return;
	printf("|----------------------------------------------------------------------------------------|\n");
    printf("|ptr = %-10p | next = %-15p | units = %-4d | struct_name = %-10s |\n",obj_info->ptr,obj_info->next,obj_info->units,obj_info->struct_info->name);
	printf("|----------------------------------------------------------------------------------------|\n");
}

void print_object_info_detail(object_info_t *obj_info) {
	if(!obj_info) return;
	
	struct_info_t *struct_info = obj_info->struct_info;
	
	unsigned int unit;
	for(unit=0;unit<obj_info->units;unit++)
	{
		int i;
		char* current_obj = obj_info->ptr + unit*struct_info->size;
		for(i=0;i<struct_info->fields_num;i++) {
			
			printf("%s[%d]->%s = ",struct_info->name,unit,struct_info->fields[i].name);
			
			data_type_t dtype = struct_info->fields[i].dtype;
			
			switch(dtype) {
				case CHAR:
					printf("%s",current_obj+struct_info->fields[i].offset);
				case INT32:
					printf("%d",*(int*)(current_obj+struct_info->fields[i].offset));
					break;
				case UINT8:
					printf("%hhu",*(unsigned char*)(current_obj+struct_info->fields[i].offset));
					break;
				case UINT32:
					printf("%u",*(unsigned int*)(current_obj+struct_info->fields[i].offset));
					break;
				case FLOAT:
					printf("%f",*(float*)(current_obj+struct_info->fields[i].offset));
					break;
				case DOUBLE:
					printf("%lf",*(double*)(current_obj+struct_info->fields[i].offset));
					break;
				case OBJ_PTR:
				case OBJ_STRUCT:
					printf("%p",current_obj+struct_info->fields[i].offset);
					break;
			}
			printf("\n");
		}
	}
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
	object_info->is_visited = False;
    object_info->is_root = False; 
	object_database_add(object_info);
	return ptr;
}

object_info_t* take_object_from_db(void *ptr) {
	if(!object_db) {return NULL;}
	
	object_info_t *node = object_db->head;
	if(!node) {return NULL;}
	
	if(node->ptr == ptr) {
		object_db->head = object_db->head->next;
		object_db->count--;
		return node;
	}
	
	while(node->next != NULL)
	{
		if(node->next->ptr == ptr) {
			object_info_t* target = node->next;
			node->next = node->next->next;
			object_db->count--;
			return target;
		}
		node = node->next;
	}
	return NULL;
}

void jfree(void *ptr) {
	object_info_t* object_info = take_object_from_db(ptr);
	assert(object_info);
	
	free(object_info->ptr);
	free(object_info);
}

void register_root_object(void *obj,char *struct_name) {
	struct_info_t* struct_info = (struct_info_t*) struct_database_look_up(struct_name);
	if(!struct_info) {
		printf("there is no %s struct",struct_name);
		return;
	}
	
	((object_info_t*)obj)->is_root = True; 
	object_database_add(obj);	
}

void set_object_as_root(void *ptr) {
	object_info_t* object_info = take_object_from_db(ptr);
	assert(object_info);
	
	object_info->is_root = True;
}

static void Set_reachable_obj_by_root(object_info_t *root) {
	unsigned int i , fields_num;
	char *root_obj_ptr = NULL;
	char *connect_obj_ptr = NULL;
	object_info_t *connect_object_info;
	struct_info_t *root_struct_info = root->struct_info;
	field_info_t *field;
	
	if(root_struct_info->fields_num<=0) {return;} //to handle void*
	
	for(i=0; i<root->units; i++) {
		
		root_obj_ptr = (char *)root->ptr + i*root_struct_info->size;
		
		for(fields_num=0; fields_num < root_struct_info->fields_num; fields_num++){
			field = &root_struct_info->fields[fields_num];
			
			if(field->dtype != OBJ_PTR) {break;}
			connect_obj_ptr = root_obj_ptr + field->offset;
			if(*connect_obj_ptr==NULL) continue;
			
			connect_object_info = take_object_from_db(*connect_obj_ptr);
			assert(connect_object_info);
			
			if(connect_object_info->is_visited == False) {
				connect_object_info->is_visited = True;
				Set_reachable_obj_by_root(connect_object_info);
			}
		}
	}
}

void Set_all_reachable_obj_to_visited(){

    if(!object_db) {return;}
	object_info_t *node = object_db->head;
	if(!node) {return;}
	while(node != NULL)
	{
		node->is_visited = False;
		node = node->next;
	}

	node = object_db->head;
	while(node != NULL)
	{
		if(node->is_root == False) {node = node->next;continue;}
		if(node->is_visited == True) {node = node->next;continue;}
		node->is_visited = True;
		
		Set_reachable_obj_by_root(node);
		node = node->next;
	}
}

void report_leaked_objects(){
	
	Set_all_reachable_obj_to_visited();
    object_info_t *node = object_db->head;

    printf("\nDumping Leaked Objects\n");

	while(node) {
		if(node->is_visited == False){
			print_object_info(node);
			printf("\n");
            print_object_info_detail(node);
            printf("\n\n");
        }
		node = node->next;
	}
}
    
    
