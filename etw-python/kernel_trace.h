#pragma once
#include "py.h"
#include <krabs.hpp>
#include "kernel_provider.h"

typedef struct
{
	PyObject_HEAD
	Py_UNICODE* name;
	krabs::kernel_trace *_trace;
} KernelTrace;

static PyMemberDef KernelTrace_members[] = {
	{
		"name",
		T_OBJECT_EX,
		offsetof(KernelTrace, name),
		READONLY,
		"Name of the trace"
	},
	{ NULL } // End marker
};

PyObject* KernelTrace_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds);

int KernelTrace_init(
	KernelTrace *self,
	PyObject *args,
	PyObject *kwds
);

void KernelTrace_dealloc(
	PyObject* self);


PyObject *KernelTrace_start(
	KernelTrace* self);

PyObject *KernelTrace_stop(
	KernelTrace* self);

PyObject *KernelTrace_enable(
	KernelTrace* self,
	PyObject* obj);

static PyMethodDef KernelTrace_methods[] =
{
	{ "start", (PyCFunction)KernelTrace_start, METH_NOARGS, "Start the kernel trace" },
	{ "stop", (PyCFunction)KernelTrace_stop, METH_NOARGS, "Stop the kernel trace" },
	{ "enable", (PyCFunction)KernelTrace_enable, METH_O, "Enable the specified provider for the trace" },
	{ NULL },
};

extern PyTypeObject KernelTrace_Type;
