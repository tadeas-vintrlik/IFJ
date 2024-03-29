/**
 * @file common.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @brief Header for common functions used in the entire project.
 */
#ifndef _COMMON_H_
#define _COMMON_H_

/**
 * @brief Structure for storing a string of
 * (possibly) infinite length
 */
typedef struct dynamic_string {
    char *content;
    unsigned size;
    unsigned limit;
} dynamic_string_s;

/**
 * @brief Stucture for storing the entire source file.
 * Inflatable array (vector) was used for implementation.
 */
typedef struct source_file {
    char **line;
    unsigned no_lines;
    unsigned limit;
} source_file_s;

/**
 * @brief An enumeration of valid return codes
 */
typedef enum rc {
    RC_OK = 0, /* Success */
    RC_LEX_ERR = 1, /* Lexical analysis error*/
    RC_SYN_ERR = 2, /* Syntactical analysis error */
    RC_SEM_UNDEF_ERR = 3, /* Semantic error -undefined function/variable or redefine */
    RC_SEM_ASSIGN_ERR = 4, /* Semantic error - assignment type incompatibility */
    RC_SEM_CALL_ERR = 5, /* Semantic error - wrong number or type of
                        paramaters or return values */
    RC_SEM_EXP_ERR = 6, /* Semantic error - wrong type compatibility in expressions */
    RC_SEM_OTHER_ERR = 7, /* Semantic error - other */
    RC_RUN_NIL_ERR = 8, /* Run error - unexpected nil */
    RC_RUN_ZERO_DIV_ERR = 9, /* Run error - zero division */
    RC_INTERNAL_ERR = 99 /* Internal error - allocation errors, etc. */
} rc_e;

/**
 * @brief Destructor for the data found inside a generic ADT.
 */
typedef void (*destructor)(void *);

/**
 * @brief A macro for better freeing to avoid double frees.
 */
#define FREE(x)                                                                                    \
    free(x);                                                                                       \
    x = NULL;

/**
 * @brief Free callback that calls the correct destructor for a generic ADT.
 */
#define FREE_VALUE(x, destructor)                                                                  \
    if (!destructor) {                                                                             \
        FREE(x->value);                                                                            \
    } else {                                                                                       \
        (*destructor)(x->value);                                                                   \
    }

/**
 * @brief A marco for unified handling of allocation failure.
 */
#define ALLOC_CHECK(x)                                                                             \
    if (!x) {                                                                                      \
        fputs("Out of memory.", stderr);                                                           \
        fprintf(stderr, "Allocation in %s:%d failed.\n", __FILE__, __LINE__);                      \
        exit(RC_INTERNAL_ERR);                                                                     \
    }

/**
 * @brief A macro for getting maximum of two numbers.
 */
#define MAX(a, b) (a > b ? a : b)

/**
 * @brief A macro for getting absolute value of a number.
 */
#define ABS(a) (a < 0 ? -a : a)

/**
 * @brief A macro for printing error messages.
 */
#define ERR_MSG(msg, line) fprintf(stderr, "Error on line %d: %s", line, msg);

/**
 * @brief Duplicates the @p str.
 *
 * @param[in] str The string to duplicate.
 *
 * @return Pointer to a new string. NULL if memory allocation error.
 */
char *my_strdup(const char *str);

/**
 * @brief Initialize the dynamic_string structure.
 *
 * @param[in,out] dynamic_string Dynamic string to initialize.
 */
void ds_init(dynamic_string_s *dynamic_string);

/**
 * @brief Add a character to the end of dynamic string.
 *
 * @param[in,out] dynamic_string Dynamic string structure where the character c will be stored.
 * @param[in] c character to add.
 *
 * @return RC_OK on success.
 */
rc_e ds_add_char(dynamic_string_s *dynamic_string, const char c);

/**
 * @brief Destructor for the dynamic_string structure.
 *
 * @param[in/out] source The structure to destroy.
 */
void ds_destroy(dynamic_string_s *dynamic_string);

/**
 * @brief Initialization of the source_file structure.
 *
 * @param[in,out] source The structure to initialize.
 */
void sf_init(source_file_s *source);

/**
 * @brief Add a line of source code to the structure.
 *
 * @param[in,out] source The structure where to add the line.
 * @param[in] line The line to add.
 *
 * @return RC_OK on success.
 * @return RC_INTERNAL_ERR when given invalid (NULL) line.
 */
rc_e sf_add_line(source_file_s *source, const char *line);

/**
 * @brief Destructor for the source_file sturcture.
 *
 * @param[in,out] source The structure to destroy.
 */
void sf_destroy(source_file_s *source);

/**
 * @brief Printer for token_types as words
 *
 * @param[in] value token value to be printed
 */
void printer(int value);

/**
 * @brief Printer for token_types as symbols used for error messages.
 *
 * @note Only works for tokens that do not have a value (will work for TOKEN_ADD as +, but will not
 *work for TOKEN_ID)
 * @param[in] type token type to be printed to stderr. Should be token_type but is unsigned to avoid
 *cyclic include.
 */
void err_token_printer(unsigned type);
#endif
