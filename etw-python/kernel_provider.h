#pragma once
#include "py.h"
#include <krabs.hpp>
#include "event.h"

typedef struct
{
	PyObject_HEAD
	ULONGLONG flags;
	Py_UNICODE* guid;
	PyObject *callback;
	krabs::kernel_provider *_provider;
} KernelProvider;

static PyMemberDef KernelProvider_members[] = {
	{
		"guid",
		T_OBJECT_EX,
		offsetof(KernelProvider, guid),
		READONLY,
		"Name of the trace"
	},
	{ NULL } // End marker
};

PyObject* KernelProvider_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds);

int KernelProvider_init(
	KernelProvider *self,
	PyObject *args,
	PyObject *kwds
);

void KernelProvider_dealloc(
	PyObject* self);

static PyMethodDef KernelProvider_methods[] =
{
	{ NULL },
};

extern PyTypeObject KernelProvider_Type;
