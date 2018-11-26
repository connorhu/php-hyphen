/*
+----------------------------------------------------------------------+
| PHP Version 5                                                        |
+----------------------------------------------------------------------+
| Copyright (c) 1997-2007 The PHP Group                                |
+----------------------------------------------------------------------+
| This source file is subject to version 3.01 of the PHP license,      |
| that is bundled with this package in the file LICENSE, and is        |
| available through the world-wide-web at the following url:           |
| http://www.php.net/license/3_01.txt                                  |
| If you did not receive a copy of the PHP license and are unable to   |
| obtain it through the world-wide-web, please send a note to          |
| license@php.net so we can mail you a copy immediately.               |
+----------------------------------------------------------------------+
| Author:                                                              |
+----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_hyphen.h"
#include <hyphen.h>

/* True global resources - no need for thread safety here */
static int hyphen_res_counter;
/* }}} */
#define HYPHEN_RES_NAME "Hyphen dictionary"


zend_function_entry php_myhw_functions[] = {
    PHP_FE(hyphen_open,    NULL)
        PHP_FE(hyphen_word,    NULL)
            PHP_FE(hyphen_close,    NULL)
                {NULL, NULL, NULL}
};

/* {{{ hyphen_module_entry
*/
zend_module_entry hyphen_module_entry = {
    STANDARD_MODULE_HEADER,
    "hyphen",
    php_myhw_functions,
    PHP_MINIT(hyphen),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(hyphen),
    "0.1",
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HYPHEN
ZEND_GET_MODULE(hyphen)
#endif

/* {{{ php_hyphen_free_dir */
static void php_hyphen_free_dir(zend_resource *rsrc TSRMLS_DC)
{
    hyphen_rsrc *hyphen_int = (hyphen_rsrc *) rsrc->ptr;

    if (hyphen_int) {
        if (hyphen_int->dict) {
            hnj_hyphen_free(hyphen_int->dict);
            hyphen_int->dict = NULL;
        }

        efree(rsrc->ptr);

        rsrc->ptr = NULL;
    }
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(hyphen)
{
    hyphen_res_counter = zend_register_list_destructors_ex(php_hyphen_free_dir, NULL, HYPHEN_RES_NAME, module_number);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(hyphen)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "hyphen support", "enabled");
    php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource hyphen_open()
Open a hyphen dictionary */
PHP_FUNCTION(hyphen_open)
{
    zval *zfilename;
    char *filename = NULL;
    int filename_length;
    hyphen_rsrc *rsrc_int;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zfilename) == FAILURE) {
        RETURN_NULL();
    }

    switch (Z_TYPE_P(zfilename)) {
        case IS_STRING:
            filename = Z_STRVAL_P(zfilename);
            filename_length = Z_STRLEN_P(zfilename);
            break;
        default:
            RETURN_FALSE;
            break;
    }

    if (filename_length == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string as dict");
        RETURN_FALSE;
    }

    if (access(filename, F_OK) == -1) {
        RETURN_FALSE;
        // zend_throw_exception_ex(mongo_ce_GridFSException, 16 TSRMLS_CC, "could not open destination file %s", filename);
        // return;
    }

    if (php_check_open_basedir(filename TSRMLS_DC)) {
        RETURN_FALSE;
    }

    rsrc_int = (hyphen_rsrc *)emalloc(sizeof(hyphen_rsrc));
  
    rsrc_int->dict = hnj_hyphen_load(filename);
    if (rsrc_int->dict == NULL) {
        RETURN_FALSE;
    }

    RETURN_RES(zend_register_resource(rsrc_int, hyphen_res_counter));
}
/* }}} */

/* {{{ proto resource hyphen_word(resource hyphen, string word [, string mode])
Returns a hyphened word */
PHP_FUNCTION(hyphen_word)
{
    zend_string *zend_hyphenated_word;
    zval *hyphen;
    zval *word;
    char *word_string;
    int word_length;
    
    hyphen_rsrc *hyphen_resource;
    char *hyphenated_word, *hyphens;
    char **replacements = NULL;
    int *pos = NULL;
    int *cut = NULL;
    int i, hyph_count;
    char *retval;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &hyphen, &word) == FAILURE) {
        return;
    }

    if ((hyphen_resource = (hyphen_rsrc *)zend_fetch_resource(Z_RES_P(hyphen), HYPHEN_RES_NAME, hyphen_res_counter)) == NULL) { 
        RETURN_FALSE; 
    }
    
    if (Z_TYPE_P(word) != IS_STRING) {
        RETURN_FALSE;
    }

    if (hyphen_resource->dict != NULL) {
        word_length = Z_STRLEN_P(word);

        // hyp hyp hurray
        hyphens = (char *) emalloc(word_length + 5);
        hyphenated_word = (char *) emalloc(word_length * 2);
        hyphenated_word[0] = '\0';

        // int hnj_hyphen_hyphenate2 (HyphenDict *dict,
        //         const char *word, int word_size, char * hyphens,
        //         char *hyphenated_word, char *** rep, int ** pos, int ** cut);

        const char *world_to_hyphenate = Z_STRVAL_P(word);

        hnj_hyphen_hyphenate2(hyphen_resource->dict, world_to_hyphenate, word_length, hyphens, hyphenated_word, &replacements, &pos, &cut);
    
        zend_hyphenated_word = zend_string_init(hyphenated_word, strlen(hyphenated_word), 0);
        
        efree(hyphens);
        efree(hyphenated_word);
    
        RETURN_STR(zend_hyphenated_word);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto void hyphen_close(resource hyphen)
Close a Hyphen dictionary */
PHP_FUNCTION(hyphen_close)
{
    zval *hyphen;
    zend_resource *zresource;

    hyphen_rsrc *hyphen_resource = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &hyphen) == FAILURE) {
        return;
    }

    if ((hyphen_resource = (hyphen_rsrc *)zend_fetch_resource(Z_RES_P(hyphen), HYPHEN_RES_NAME, hyphen_res_counter)) == NULL) { 
        RETURN_FALSE; 
    }
    
    if (hyphen_resource && hyphen_resource->dict) {
        hnj_hyphen_free(hyphen_resource->dict);
        hyphen_resource->dict = NULL;
    }
    
    zresource = Z_RES_P(hyphen);
    zend_list_close(zresource);
    ZVAL_NULL(hyphen);
}
/* }}} */


/*
* Local variables:
* tab-width: 4
* c-basic-offset: 4
* End:
* vim600: noet sw=4 ts=4 fdm=marker
* vim<600: noet sw=4 ts=4
*/
