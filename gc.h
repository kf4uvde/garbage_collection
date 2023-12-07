#define MAX_STRUCTURE_NAME_LENGTH 128
#define MAX_FIELD_NAME_LENGTH 128

#define OFFSETOF(struct_name, field_name) (unsigned int)&(((struct_name *)0)->field_name)
#define FIELD_SIZE(struct_name, field_name) sizeof(((struct_name *)0)->field_name)
#define STRUCT_SIZE(struct_name) sizeof(#struct_name)

typedef enum {
	CHAR,
	INT32,
	UINT8,
	UINT32,
	FLOAT,
	DOUBLE,
	OBJ_PTR,
	OBJ_STRUCT
} data_type_t;

typedef struct _field_info_{
	char name[MAX_FIELD_NAME_LENGTH];
	data_type_t dtype;
	unsigned int size;
	unsigned int offset;
	char nested_struct[MAX_STRUCTURE_NAME_LENGTH];
} field_info_t;

typedef struct _struct_info_t{
	struct _struct_info_t *next;
	char name[MAX_STRUCTURE_NAME_LENGTH];
	unsigned int size;
	unsigned int fields_num;
	field_info_t *fields;
}struct_info_t;

typedef struct _struct_db_{
    struct_info_t *head;
    unsigned int count;
} struct_db_t;

//abstract
void print_struct_info (struct_info_t *struct_info);
void print_struct_db();
int struct_register(char *struct_name,unsigned int struct_size,unsigned int field_num,field_info_t *fields_array);
struct_info_t* struct_database_look_up(char *struct_name);

//register marco
#define FIELD_EXTEND(struct_name, fld_name, dtype, nested_struct) {#fld_name,dtype,FIELD_SIZE(struct_name,fld_name),OFFSETOF(struct_name,fld_name),#nested_struct}
#define STRUCT_REGISTER(struct_name,fields_array) struct_register(#struct_name,(unsigned int)sizeof(struct_name),(unsigned int)sizeof(fields_array)/sizeof(field_info_t),fields_array)


//object-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct _object_info_{
    struct _object_info_ *next;
    void *ptr;
    unsigned int units;
    struct_info_t *struct_info;
}object_info_t;

typedef struct _object_db_{
    //struct_db_t *struct_db;???
    object_info_t *head;
    unsigned int count;
} object_db_t;


void print_object_info(object_info_t *obj_info);
void print_object_info_detail(object_info_t *obj_info);
void print_object_db();
void* jalloc(char *struct_name, int units);
void jfree(void *ptr);
