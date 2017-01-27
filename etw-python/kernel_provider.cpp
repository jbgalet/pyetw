#include "kernel_provider.h"
#include <exception>

PyTypeObject KernelProvider_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"etw.KernelProvider",					/*tp_name*/
	sizeof(KernelProvider),				/*tp_basicsize*/
	0,								/*tp_itemsize*/
	(destructor)KernelProvider_dealloc,   /*tp_dealloc*/
	0,								/*tp_print*/
	0,								/*tp_getattr*/
	0,								/*tp_setattr*/
	0,								/*tp_compare*/
	0,				                /*tp_repr*/
	0,								/*tp_as_number*/
	0,								/*tp_as_sequence*/
	0,								/*tp_as_mapping*/
	0,								/*tp_hash */
	0,								/*tp_call*/
	0,								/*tp_str*/
	0,				                /*tp_getattro*/
	0,								/*tp_setattro*/
	0,								/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"KernelProvider class",				/* tp_doc */
	0,								/* tp_traverse */
	0,								/* tp_clear */
	0,					            /* tp_richcompare */
	0,								/* tp_weaklistoffset */
	0,								/* tp_iter */
	0,								/* tp_iternext */
	KernelProvider_methods,               /* tp_methods */
	KernelProvider_members,               /* tp_members */
	0,								/* tp_getset */
	0,								/* tp_base */
	0,								/* tp_dict */
	0,								/* tp_descr_get */
	0,								/* tp_descr_set */
	0,								/* tp_dictoffset */
	(initproc)KernelProvider_init,		/* tp_init */
	0,								/* tp_alloc */
	KernelProvider_new,					/* tp_new */
};

PyObject* KernelProvider_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds)
{
	KernelProvider* self;

	self = (KernelProvider*)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	self->guid = NULL;
	self->_provider = NULL;

	return (PyObject*)self;
}

int KernelProvider_init(
	KernelProvider *self,
	PyObject *args,
	PyObject *kwds
)
{
	static char *kwlist[] = { "flags", "guid", "callback", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|KZO", kwlist,
		&self->flags,
		&self->guid,
		&self->callback))
		return -1;

	if (!PyCallable_Check(self->callback))
		return -1;

	self->_provider = new (std::nothrow) krabs::kernel_provider(self->flags, krabs::guid(self->guid));
	if (!self->_provider)
		return -1;

	self->_provider->add_on_event_callback([self](const EVENT_RECORD &record) {
		event_callback(record, self->callback);
	});
	return 0;
}

void KernelProvider_dealloc(
	PyObject* self)
{
	KernelProvider* object = (KernelProvider*)self;
	delete object->_provider;
	object->_provider = NULL;

	PyObject_Del(self);
}
