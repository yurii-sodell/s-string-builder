#  string_builder_t
Javalike String Builder

# Usage Guide

A simple dynamic string type for C, backed by `string_builder_t`. All operations are accessed through function calls; the struct itself is opaque to the caller.

## Status codes

Most functions return `sb_status`, used to report success or failure:

- `SB_OK` -> operation succeeded
- `SB_COUNT_IS_CORRUPTED` -> internal count is invalid (negative)
- `SB_OUT_OF_BOUNDS` -> provided index or range is invalid
- `SB_MEMORY_FAULT` -> allocation failed
- `SB_IS_NULL` -> the string builder (or its buffer) is NULL (Usually is used for enternal handling)
Use `sb_handle_status(status)` to print a human-readable message for a status code.

## Creating a string builder

```c
string_builder_t* sb_create();
```
Creates an empty string builder with a default capacity (1024).

```c
string_builder_t* sb_from_string(const char* initialStr);
```
Creates a string builder initialized with the contents of `initialStr`.

```c
string_builder_t* sb_substring(const string_builder_t* sb, int start, int end);
```
Creates a new string builder containing the characters in `[start, end)` from `sb`. Returns NULL if the range is invalid.

Always check the return value for NULL before using it.

## Reading data

```c
int sb_legth(const string_builder_t* sb);
```
Returns the current number of characters stored.

```c
char* sb_copy_of_string_value(const string_builder_t* sb);
```
Returns a newly allocated, null-terminated copy of the string. Caller is responsible for freeing it.

```c
int sb_contains(const string_builder_t* sb, const char* toFind);
```
Returns 1 if `toFind` is a substring of `sb`, 0 otherwise.

```c
sb_status sb_print(const string_builder_t* sb);
```
Prints the current contents to stdout.

## Modifying contents

```c
sb_status sb_append(string_builder_t* sb, const char* to_append);
```
Appends `to_append` to the end of `sb`.

```c
sb_status sb_insert(string_builder_t* sb, const char* to_insert, int start_index);
```
Inserts `to_insert` at `start_index`, shifting existing characters to the right.

```c
sb_status sb_set_char_at(string_builder_t* sb, char to_set, int index);
```
Overwrites a single character at `index`.

```c
sb_status sb_replace(string_builder_t* sb, const char* toReplaceWith, int start, int end);
```
Replaces the range `[start, end)` with `toReplaceWith`. The new string may be a different length than the range it replaces.

```c
sb_status sb_replace_value_with(string_builder_t* sb, const char* toReplaceWith);
```
Replaces the entire contents of `sb` with `toReplaceWith`.

```c
sb_status sb_replace_all_chars_with(string_builder_t* sb, char toReplaceWith);
```
Overwrites every existing character with `toReplaceWith`, keeping the same length.

```c
sb_status sb_delete_char_at(string_builder_t* sb, int index);
```
Removes the character at `index`.

```c
sb_status sb_delete(string_builder_t* sb, int start, int end);
```
Removes the range `[start, end)`.

```c
sb_status sb_trim(string_builder_t* sb);
```
Removes all space characters from the string.

```c
sb_status sb_invert(string_builder_t* sb);
```
Reverses the string in place.

```c
sb_status sb_to_upper_case(string_builder_t* sb);
sb_status sb_to_lower_case(string_builder_t* sb);
```
Convert ASCII letters to upper/lower case.

## Cleanup

```c
void sb_free(string_builder_t* sb);
```
Frees the string builder and its internal buffer. Call this once you're done with any `string_builder_t*`, including ones returned by `sb_substring`.

## Typical usage pattern

```c
string_builder_t* sb = sb_from_string("hello world");
if (sb == NULL) {
    sb_handle_status(SB_IS_NULL);
    return 1;
}

sb_handle_status(sb_to_upper_case(sb));
sb_print(sb); // HELLO WORLD

sb_free(sb);
```

## Notes and caveats

- Always check for NULL after any function that returns `string_builder_t*`.
- Functions that return `sb_status` should have their result checked (or at least handled with `sb_handle_status`) before continuing, since a failed operation may leave `sb` unchanged.
- `sb_copy_of_string_value` allocates memory that you must `free()` yourself; it is separate from `sb_free`, which only frees the builder.
- Index-based (`sb_set_char_at`, `sb_delete_char_at`) or ranges-based (in `sb_replace`/`sb_delete`/`sb_substring`) functions expect 0-based indices, with `end` typically exclusive.
