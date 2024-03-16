#ifndef UTIL_REVERSEARRAY
#define UTIL_REVERSEARRAY
typedef struct list_iter List_iter;
typedef struct list_index List_index;

struct list_iter {
    List_iter* prev;
    List_iter* next;
    void* data;
    List_index* index;
};

struct list_index{
    List_iter* start;
    List_iter* end;    
};

/*
__________   prev _____________
|   to   | <------|return value|

*/

typedef enum {
    after_of,
    before_of
} inseryt_type;
List_iter* add_reverse_array(List_iter* to);
List_iter* add_reverse_array_upd(List_iter** to);
List_iter* insert_reverse_array(inseryt_type position, List_iter* insert_to);

#endif

