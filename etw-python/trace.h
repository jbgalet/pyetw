#pragma once
#include "py.h"
#include <krabs.hpp>
#include "provider.h"

typedef struct
{
	PyObject_HEAD
	Py_UNICODE* name;
	krabs::user_trace *_trace;
} UserTrace;

static PyMemberDef UserTrace_members[] = {
	{
		"name",
		T_OBJECT_EX,
		offsetof(UserTrace, name),
		READONLY,
		"Name of the trace"
	},
	{ NULL } // End marker
};

PyObject* UserTrace_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds);

int UserTrace_init(
	UserTrace *self,
	PyObject *args,
	PyObject *kwds
);

void UserTrace_dealloc(
	PyObject* self);


PyObject *UserTrace_start(
	UserTrace* self);

PyObject *UserTrace_stop(
	UserTrace* self);

PyObject *UserTrace_enable(
	UserTrace* self,
	PyObject* obj);

static PyMethodDef UserTrace_methods[] =
{
	{ "start", (PyCFunction)UserTrace_start, METH_NOARGS, "Start the user trace"},
	{ "stop", (PyCFunction)UserTrace_stop, METH_NOARGS, "Stop the user trace" },
	{ "enable", (PyCFunction)UserTrace_enable, METH_O, "Enable the specified provider for the trace" },
	{ NULL },
};

extern PyTypeObject UserTrace_Type;
