
dnl
dnl LMAP_MESSAGE(MESSAGE)
dnl

AC_DEFUN(LMAP_MESSAGE,[
   AC_MSG_RESULT()
   AC_MSG_RESULT(${SB}$1...${EB})
   AC_MSG_RESULT()
])

dnl
dnl LMAP_CHECK_OPTION(STRING, VAR)
dnl

AC_DEFUN(LMAP_CHECK_OPTION,[
   echo "$1 ${SB}$2${EB}"
])


dnl vim:ts=3:expandtab
