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
static int le_hyphendic;
/* }}} */
#define le_hyphendic_name "Hyphen dictionary"


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
static void php_hyphen_free_dir(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
  hyphen_rsrc * hyphen_int = (hyphen_rsrc *) rsrc->ptr;

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
  le_hyphendic   = zend_register_list_destructors_ex(php_hyphen_free_dir,   NULL, le_hyphendic_name,   module_number);

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
  char     *filename;
  int       filename_len;
  char resolved_path[MAXPATHLEN + 1];
  hyphen_rsrc *rsrc_int;

  if (ZEND_NUM_ARGS() != 1) {
    WRONG_PARAM_COUNT;
  }
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
    return;
  }
  
  if (filename_len == 0) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string as dict");
    RETURN_FALSE;
  }
  
  if (OPENBASEDIR_CHECKPATH(filename)) {
    RETURN_FALSE;
  }

  if(!expand_filepath(filename, resolved_path TSRMLS_CC)) {
    RETURN_FALSE;
  }

  rsrc_int = (hyphen_rsrc *)emalloc(sizeof(hyphen_rsrc));
  
  rsrc_int->dict = hnj_hyphen_load(filename);
  if (rsrc_int->dict == NULL) {
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, rsrc_int, le_hyphendic);
}
/* }}} */

/* {{{ proto resource hyphen_word(resource hyphen, string word [, string mode])
   Returns a hyphened word */
PHP_FUNCTION(hyphen_word)
{
  zval * hyphen;
  zval * word;
  int word_len;
  hyphen_rsrc * hy_rsrc;
  char *hyphenated_word, *hyphens;
  char ** rep = NULL;
  int * pos = NULL;
  int * cut = NULL;
  int i, hyph_count;
  char *retval;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &hyphen, &word, &word_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(hy_rsrc, hyphen_rsrc *, &hyphen, -1, le_hyphendic_name, le_hyphendic);

  if (hy_rsrc->dict != NULL) {
    // hyp hyp hurray
    hyphens = (char *) emalloc(word_len + 8);
    hyphenated_word = (char *) emalloc(word_len * 3);

    hnj_hyphen_hyphenate2(hy_rsrc->dict, &Z_STRVAL_P(word), word_len, hyphens, hyphenated_word, &rep, &pos, &cut);
    
    RETURN_STRINGL(hyphenated_word, strlen(hyphenated_word), 1);
  } else {
    RETURN_FALSE;
  }
}
/* }}} */

/* {{{ proto void hyphen_close(resource hyphen)
   Close a Hyphen dictionary */
PHP_FUNCTION(hyphen_close)
{
  zval * hyphen;
  hyphen_rsrc *hypgen_rsrc = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &hyphen) == FAILURE) {
    return;
  }
  ZEND_FETCH_RESOURCE(hypgen_rsrc, hyphen_rsrc *, &hyphen, -1, le_hyphendic_name, le_hyphendic);

  zend_list_delete(Z_LVAL_P(hyphen));
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
