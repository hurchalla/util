All files in this experimental directory are expected to be good and immediately usable if desired.  However, I consider them to be experiments that don't seem useful enough to belong in the include directory.
<br/><br/><br/>
The file "exception_checking.h" provides exception checking macros:<br/>
  * PBC_VERIFY_THROWS(EXPRESSION_BODY, ...) <br/>
  This macro requires an EXPRESSION_BODY argument, followed by a variable number of exception types.  It will run EXPRESSION_BODY, and if any exceptions are thrown that have a type not listed in the variable aguments, the macro will call pbcAssertHandler (typically resulting in abort).
  * PBC_VERIFY_NOTHROW(EXPRESSION_BODY) <br/>
  This macro will run EXPRESSION_BODY, and if any exceptions are thrown, the macro will call pbcAssertHandler (typically resulting in abort).<br/>
<a/>
Both macros always evaluate EXPRESSION_BODY, regardless of whether or not NDEBUG is defined.
