All files in this experimental directory are expected to be good and immediately usable if desired.  However, I consider them to be experiments that aren't useful enough to belong in the "official" include directory.
<br/><br/><br/>
The file "exception_checking.h" provides exception checking macros:<br/>
  * HPBC_VERIFY_THROWS(EXPRESSION_BODY, ...) <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; This macro requires an EXPRESSION_BODY argument, followed by a variable number of exception types.  It will run EXPRESSION_BODY, and if any exceptions are thrown that have a type not listed in the variable arguments, the macro will print a failure message and abort.
  * HPBC_VERIFY_NOTHROW(EXPRESSION_BODY) <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; This macro will run EXPRESSION_BODY, and if any exceptions are thrown, the macro will print a failure message and abort.<br/><br/>
<a/>
Both macros will always evaluate EXPRESSION_BODY.
