#include "py.h"
#include "trace.h"
#include "provider.h"
#include "kernel_trace.h"
#include "kernel_provider.h"

#define ETW_DOC "\
This module allows you to use subscribe to ETW providers using krabs.\n\
\n\
For complete documentation please visit:\n\
FIXME\n"

static PyMethodDef etw_methods[] = {
  { NULL, NULL }
};

/*
TODO:
* ETW specific exceptions
* Trace objects
  [X] UserTrace
  [ ] KernelTrace
* Provider objects
  [X] Provider
  [ ] KernelProvider
*/

MOD_INIT(etw)
{
	PyObject* m;

	MOD_DEF(m, "etw", ETW_DOC, etw_methods)

	if (m == NULL)
		return MOD_ERROR_VAL;

	PyModule_AddIntConstant(m, "version", 1);

	/* User mode */

	if (PyType_Ready(&UserTrace_Type) < 0)
		return MOD_ERROR_VAL;
	Py_INCREF(&UserTrace_Type);
	PyModule_AddObject(m, "UserTrace", (PyObject *)&UserTrace_Type);

	if (PyType_Ready(&Provider_Type) < 0)
		return MOD_ERROR_VAL;
	Py_INCREF(&Provider_Type);
	PyModule_AddObject(m, "Provider", (PyObject *)&Provider_Type);

	/* Kernel mode */

	if (PyType_Ready(&KernelProvider_Type) < 0)
		return MOD_ERROR_VAL;
	Py_INCREF(&KernelProvider_Type);
	PyModule_AddObject(m, "KernelProvider", (PyObject *)&KernelProvider_Type);

	if (PyType_Ready(&KernelTrace_Type) < 0)
		return MOD_ERROR_VAL;
	Py_INCREF(&KernelTrace_Type);
	PyModule_AddObject(m, "KernelTrace", (PyObject *)&KernelTrace_Type);

	return MOD_SUCCESS_VAL(m);
}
