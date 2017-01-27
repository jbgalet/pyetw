#pragma once
#include "py.h"
#include <krabs.hpp>
#include "event.h"

typedef struct
{
	PyObject_HEAD
	Py_UNICODE* guid;
	PyObject *callback;
	krabs::provider<> *_provider;
} Provider;

static PyMemberDef Provider_members[] = {
	{
		"guid",
		T_OBJECT_EX,
		offsetof(Provider, guid),
		READONLY,
		"Name of the trace"
	},
	{ NULL } // End marker
};

PyObject* Provider_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds);

int Provider_init(
	Provider *self,
	PyObject *args,
	PyObject *kwds
);

void Provider_dealloc(
	PyObject* self);

PyObject *Provider_any(
	Provider* self,
	PyObject *args);

PyObject *Provider_all(
	Provider* self,
	PyObject *args);

PyObject *Provider_level(
	Provider* self,
	PyObject *args);

static PyMethodDef Provider_methods[] =
{
	{ "any", (PyCFunction)Provider_any, METH_VARARGS, "Call the any method of the provider" },
	{ "all", (PyCFunction)Provider_all, METH_VARARGS, "Call the all method of the provider" },
	{ "level", (PyCFunction)Provider_level, METH_VARARGS, "Call the level method of the provider" },
	{ NULL },
};

extern PyTypeObject Provider_Type;
