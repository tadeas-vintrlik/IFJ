/**
 * @file common.h
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for common functions used in the entire project.
 */
#ifndef _COMMON_H_
#define _COMMON_H_

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
    RC_OK = 0, /*<< Success */
    RC_LEX_ERR = 1, /*<< Lexical analysis error*/
    RC_SYN_ERR = 2, /*<< Syntactical analysis error */
    RC_SEM_UNDEF_ERR = 3, /*<< Semantic errror -undefined function/variable or redefine */
    RC_SEM_ASSIGN_ERR = 4, /*<< Semantic error - assignment type incompatibilty */
    RC_SEM_CALL_ERR = 5, /*<< Semantic error - wrong number or type of
                            paramaters or return values */
    RC_SEM_EXP_ERR = 6, /*<< Semantic error - wrong type compatibility in expressions */
    RC_SEM_OTHER_ERR = 7, /*<< Semantic error - other */
    RC_RUN_NIL_ERR = 8, /*<< Run error - unexpected nil */
    RC_RUN_ZERO_DIV_ERR = 9, /*<< Run error - zero division */
    RC_INTERNAL_ERR = 99 /*<< Internal error - allocation errors, etc. */
} rc_e;

/**
 * @brief A macro for better freeing to avoid double frees.
 */
#define FREE(x)                                                                                    \
    free(x);                                                                                       \
    x = NULL;

/**
 * @brief A marco for unified handling of allocation failure.
 */
#define ALLOCK_CHECK(x)                                                                            \
    if (!x) {                                                                                      \
        fputs("Out of memory.", stderr);                                                           \
        fprintf(stderr, "Allocation in %s:%d failed.\n", __FILE__, __LINE__);                      \
        exit(RC_INTERNAL_ERR);                                                                     \
    }

/**
 * @brief Initialization of the source_file structure.
 *
 * @param[in/out] source The structure to initialize.
 */
void sf_init(source_file_s *source);

/**
 * @brief Add a line of source code to the structure.
 *
 * @param[in/out] source The structure where to add the line.
 * @param[in] line The line to add.
 *
 * @return RC_OK on success.
 * @return RC_INTERNAL_ERR when given invalid (NULL) line.
 */
rc_e sf_add_line(source_file_s *source, const char *line);

/**
 * @brief Destructor for the source_file sturcture.
 *
 * @param[in/out] source The structure to destroy.
 */
void sf_destroy(source_file_s *source);

#endif