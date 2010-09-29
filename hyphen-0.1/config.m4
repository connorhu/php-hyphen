dnl $Id$
dnl config.m4 for extension hyphen

PHP_ARG_WITH(hyphen,for hyphenation support,
[  --with-hyphen[=DIR]    Include hyphenation support])


if test "$PHP_HYPHEN" != "no"; then
  dnl Write more examples of tests here...

  SEARCH_PATH="/usr /usr/local"     # you might want to change this
  SEARCH_FOR="/include/hyphen.h"  # you most likely want to change this
  if test -r $PHP_HYPHEN/$SEARCH_FOR; then # path given as parameter
    HYPHEN_DIR=$PHP_HYPHEN
  else # search default path list
    AC_MSG_CHECKING([for hyphen files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        HYPHEN_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$HYPHEN_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libhyphen distribution])
  fi

  # --with-hyphen -> add include path
  PHP_ADD_INCLUDE($HYPHEN_DIR/include)

  dnl # --with-hyphen -> check for lib and symbol presence
  LIBNAME=hyphen # you may want to change this
  LIBSYMBOL=hnj_free # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $HYPHEN_DIR/lib, HYPHEN_SHARED_LIBADD)
    AC_DEFINE(HAVE_HYPHENLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong hyphen lib version or lib not found])
  ],[
    -L$HYPHEN_DIR/lib -lm -ldl
  ])
  
  PHP_SUBST(HYPHEN_SHARED_LIBADD)
  PHP_NEW_EXTENSION(hyphen, hyphen.c, $ext_shared)
fi
