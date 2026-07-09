#include "string_builder_t.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true_malloc (int)1
#define false_malloc (int)0

#define sb_layer_1 1
#define sb_layer_2 2

#define sb_basic_capacity 1024

typedef struct string_builder_t {
    char* value;
    int count;
    int capacity;
} string_builder_t;

sb_status check_standard_errors(const string_builder_t* str, int afterMalloc, int deep) {
    sb_status status = afterMalloc == 1 ? SB_MEMORY_FAULT : SB_IS_NULL;
    if (deep >= sb_layer_1 && str == NULL) return status;
    if (deep >= sb_layer_2 && str->value == NULL) return status;
    if (deep >= sb_layer_2 && str->count < 0) return SB_COUNT_IS_CORRUPTED;
    return SB_OK;
}

sb_status ensure_capacity(string_builder_t* sb, int required_len) {
    if (sb->capacity <= 0) sb->capacity = 2;

    while (sb->capacity < required_len) {
        int current_capacity = sb->capacity;
        char* toKeep = sb->value;
        sb->value = malloc(current_capacity + current_capacity / 2);
        if (sb->value == NULL) return SB_MEMORY_FAULT;
        sb->capacity = current_capacity + current_capacity / 2;
        for (int i = 0; i < sb->count; i++) {
            sb->value[i] = toKeep[i];
        }
        free(toKeep);
    }
    return SB_OK;
}

void sb_handle_status(sb_status status) {
    if (status == SB_OK) return;
    char* message = "";
    switch (status) {
        case SB_MEMORY_FAULT:
            message = "Memory fault";
            break;
        case SB_IS_NULL:
            message = "SB is NULL ";
            break;
        case SB_OUT_OF_BOUNDS:
            message = "SB out of bounds ";
            break;
        default:
            message = "Undefined status ";
            break;
            fprintf(stderr, "\n%s", message);
    }
}

int sb_legth(const string_builder_t* sb) { return sb->count; };

char* sb_copy_of_string_value(const string_builder_t* sb) {
    int count = sb->count;
    char* to_return = malloc(count + 1);
    char* value = sb->value;
    for (int i = 0; i < count; i++) {
        to_return[i] = value[i];
    }
    to_return[count] = '\0';
    return to_return;
}

void sb_free(string_builder_t* sb) {
    free(sb->value);
    free(sb);
}

string_builder_t* sb_allocate_new(const int capacity) {
    string_builder_t* new_sb = malloc(sizeof(string_builder_t));
    sb_status status1 = check_standard_errors(new_sb, true_malloc, sb_layer_1);
    if (status1 != SB_OK) return NULL;

    new_sb = malloc(sizeof(string_builder_t));
    sb_status status2 = check_standard_errors(new_sb, true_malloc, sb_layer_2);
    if (status2 != SB_OK) return NULL;

    new_sb->value = malloc(capacity);
    sb_status status3 = check_standard_errors(new_sb, true_malloc, sb_layer_2);
    if (status3 != SB_OK) return NULL;

    new_sb->count = 0;
    new_sb->capacity = capacity;

    return new_sb;
}

string_builder_t* sb_create() { return sb_allocate_new(sb_basic_capacity); }

string_builder_t* sb_from_string(const char* initialStr) {
    int len = strlen(initialStr);
    string_builder_t* to_return = sb_create();
    if (to_return == NULL) return NULL;

    ensure_capacity(to_return, len);
    for (int i = 0; i < len; i++) {
        to_return->value[i] = initialStr[i];
    }
    to_return->count = len;
    return to_return;
}

string_builder_t* sb_substring(const string_builder_t* sb, const int start, const int end) {
    sb_status status1 = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status1 != SB_OK) return NULL;

    if (start < 0 || end > sb->count || end <= start) {
        return NULL;
    }

    int len = end - start;
    string_builder_t* to_return = sb_create();
    if (to_return == NULL) return NULL;

    sb_status status2 = ensure_capacity(to_return, len);
    sb_handle_status(status2);
    if (status2 == SB_MEMORY_FAULT) return NULL;
    for (int i = start; i < end; i++) {
        to_return->value[i - start] = sb->value[i];
    }

    to_return->count = len;
    return to_return;
}

sb_status sb_print(const string_builder_t* sb) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;
    printf("\n%.*s", sb->count, sb->value);
    return SB_OK;
}

sb_status sb_invert(string_builder_t* sb) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;

    int count = sb->count;

    char* newVal = malloc(count);
    if (newVal == NULL) {
        return SB_MEMORY_FAULT;
    }

    int b = count;
    int i = 0;
    while (i < count) {
        newVal[i++] = sb->value[--b];
    }
    while (b < count) {
        sb->value[b] = newVal[b];
        b++;
    }

    free(newVal);
    return SB_OK;
};

sb_status sb_trim(string_builder_t* sb) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;

    int count = sb->count;
    char* value = sb->value;

    for (int i = 0; i < count; i++) {
        if (value[i] == ' ') {
            sb->count--;
            for (int b = i; b < count - 1; b++) {
                value[b] = value[b + 1];
            }
        }
    }
    return SB_OK;
}

sb_status sb_replace_all_chars_with(string_builder_t* sb, const char toReplaceWith) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;

    for (int i = 0; i < sb->count; i++) {
        sb->value[i] = toReplaceWith;
    }
    return SB_OK;
}

sb_status sb_replace_value_with(string_builder_t* sb, const char* toReplaceWith) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;

    int len = strlen(toReplaceWith);
    ensure_capacity(sb, len);
    if (sb->value == NULL) return SB_MEMORY_FAULT;
    sb->count = len;
    for (int i = 0; i < len; i++) {
        sb->value[i] = toReplaceWith[i];
    }
    return SB_OK;
}

sb_status sb_insert(string_builder_t* sb, const char* to_insert, const int start_index) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;
    if (start_index < 0 || start_index > sb->count) return SB_OUT_OF_BOUNDS;
    int length_initial = sb->count;
    int lenght_to_insert = strlen(to_insert);
    int final_lenght = length_initial + lenght_to_insert;

    sb_status status2 = ensure_capacity(sb, final_lenght);
    if (status2 == SB_MEMORY_FAULT) return SB_MEMORY_FAULT;

    char* value = sb->value;
    memmove(value + start_index + lenght_to_insert, value + start_index,
            length_initial - start_index);
    memcpy(value + start_index, to_insert, lenght_to_insert);

    sb->count = final_lenght;
    return SB_OK;
}

sb_status sb_replace(string_builder_t* sb, const char* toReplaceWith, const int start,
                     const int end) {
    if (start > end || start < 0 || end > sb->count) return SB_OUT_OF_BOUNDS;
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return 0;

    int length_initial = sb->count;
    int lenght_to_insert = strlen(toReplaceWith);
    int lentgh_to_delete = end - start;
    int final_lenght = length_initial - lentgh_to_delete + lenght_to_insert;

    sb_status status2 = ensure_capacity(sb, final_lenght);
    if (status2 == SB_MEMORY_FAULT) return SB_MEMORY_FAULT;

    char* value = sb->value;

    memmove(value + start + lenght_to_insert, value + end, length_initial - end);
    memcpy(value + start, toReplaceWith, lenght_to_insert);

    sb->count = final_lenght;
    return SB_OK;
}

sb_status sb_append(string_builder_t* sb, const char* to_append) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;
    int len_initial = sb->count;

    int len_final = strlen(to_append) + len_initial;
    ensure_capacity(sb, len_final);
    if (sb->value == NULL) return SB_MEMORY_FAULT;
    char* value = sb->value;
    for (int i = len_initial; i < len_final; i++) {
        value[i] = to_append[i - len_initial];
    }

    sb->count = len_final;
    return SB_OK;
}

int sb_get_struct_size(){
    return sizeof(string_builder_t);
}

int sb_contains(const string_builder_t* sb, const char* toFind) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return 0;

    int sb_len = sb->count;
    int found = 0;
    int len = strlen(toFind);
    if (len > sb_len) return 0;

    for (int i = 0; i <= sb_len - len && found != 1; i++) {
        int innerHaveToBreak = 0;

        for (int b = 0; b < len && found != 1 && innerHaveToBreak != 1; b++) {
            if (sb->value[i + b] != toFind[b]) innerHaveToBreak = 1;
            if (b == (len - 1) && innerHaveToBreak == 0) {
                found = 1;
            }
        }
    }
    return found;
}

sb_status sb_set_char_at(string_builder_t* sb, const char to_set, const int index) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;
    if (index < 0 || index > sb->count) return SB_OUT_OF_BOUNDS;
    sb->value[index] = to_set;
    return SB_OK;
}

sb_status sb_to_upper_case(string_builder_t* sb) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;

    int shift = 'a' - 'A';
    int len = sb->count;
    char* value = sb->value;

    for (int i = 0; i < len; i++) {
        char* current_char_p = &value[i];
        if (*current_char_p >= 'a' && *current_char_p <= 'z')
            *current_char_p = *current_char_p - shift;
    }

    return SB_OK;
}

sb_status sb_to_lower_case(string_builder_t* sb) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;

    int shift = 'a' - 'A';
    int len = sb->count;
    char* value = sb->value;

    for (int i = 0; i < len; i++) {
        char* current_char_p = &value[i];
        if (*current_char_p >= 'A' && *current_char_p <= 'Z')
            *current_char_p = *current_char_p + shift;
    }

    return SB_OK;
};

sb_status sb_delete_char_at(string_builder_t* sb, const int index) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;
    if (index < 0 || index > sb->count) return SB_OUT_OF_BOUNDS;
    char* value = sb->value;
    memmove(value + index, value + index + 1, sb->count - index);
    sb->count--;
    return SB_OK;
}

sb_status sb_delete(string_builder_t* sb, const int start, const int end) {
    sb_status status = check_standard_errors(sb, false_malloc, sb_layer_2);
    if (status != SB_OK) return status;
    if (start > end || start < 0 || end > sb->count) return SB_OUT_OF_BOUNDS;

    int count = sb->count;
    int length_to_delete = end - start;
    int final_length = count - length_to_delete;

    char* value = sb->value;
    memmove(value + start, value + end, count - end);

    sb->count = final_length;
    return SB_OK;
}

int sb_is_value_equal(string_builder_t* sb1, string_builder_t* sb2){
    int len = sb1->count;
    sb_status s1 = check_standard_errors(sb1, false_malloc, 4);
    sb_status s2 = check_standard_errors(sb2, false_malloc, 4);
    if(s1 != SB_OK){
        sb_handle_status(s1);
        return -1;
    }
    if(s1 != SB_OK){
        sb_handle_status(s2);
        return -1;
    }

    if(len != sb2->count) return 0;
    char* sb1v = sb1->value;
    char* sb2v = sb2->value;
    for(int i = 0; i<len; i++){
        if(sb1v[i] != sb2v[i]) return 0;
    }
    return 1;
}