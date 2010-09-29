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

#ifndef PHP_HYPHEN_H
#define PHP_HYPHEN_H

extern zend_module_entry hyphen_module_entry;
#define phpext_hyphen_ptr &hyphen_module_entry

#ifdef PHP_WIN32
#define PHP_HYPHEN_API __declspec(dllexport)
#else
#define PHP_HYPHEN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif
#include <hyphen.h>

/* {{{ OPENBASEDIR_CHECKPATH(filename) */
#if (PHP_MAJOR_VERSION < 6)
#define OPENBASEDIR_CHECKPATH(filename) \
  (PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(filename TSRMLS_CC)
#else 
#define OPENBASEDIR_CHECKPATH(filename) \
  php_check_open_basedir(filename TSRMLS_CC)
#endif
/* }}} */

PHP_MINIT_FUNCTION(hyphen);
PHP_MINFO_FUNCTION(hyphen);
PHP_FUNCTION(hyphen_open);
PHP_FUNCTION(hyphen_word);
PHP_FUNCTION(hyphen_close);

#ifdef ZTS
#define HYPHEN_G(v) TSRMG(hyphen_globals_id, zend_hyphen_globals *, v)
#else
#define HYPHEN_G(v) (hyphen_globals.v)
#endif

#define HYPHEN_SEP_TEXT      1
#define HYPHEN_SEP_HTML      2

typedef struct _ze_hyphen_rsrc {
  HyphenDict *dict;
} hyphen_rsrc;


#endif  /* PHP_HYPHEN_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
