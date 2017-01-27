#pragma once
#define WITH_THREAD
#include <Python.h>
#include <structmember.h>

#if PY_MAJOR_VERSION >= 3
#define MOD_ERROR_VAL NULL
#define MOD_SUCCESS_VAL(val) val
#define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
#define MOD_DEF(ob, name, doc, methods) \
      static struct PyModuleDef moduledef = { \
        PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
      ob = PyModule_Create(&moduledef);
#define PY_STRING(x) PyUnicode_FromString(x)
#define PY_STRING_TO_C(x) PyBytes_AsString(\
                            PyUnicode_AsEncodedString(x, "utf-8", "strict"))
#define PY_STRING_CHECK(x) PyUnicode_Check(x)
#else
#define MOD_ERROR_VAL
#define MOD_SUCCESS_VAL(val)
#define MOD_INIT(name) void init##name(void)
#define MOD_DEF(ob, name, doc, methods) \
      ob = Py_InitModule3(name, methods, doc);
#define PY_STRING(x) PyString_FromString(x)
#define PY_STRING_TO_C(x) PyString_AsString(x)
#define PY_STRING_CHECK(x) (PyString_Check(x) || PyUnicode_Check(x))
#endif
