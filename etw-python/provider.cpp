#include "provider.h"
#include <exception>

PyTypeObject Provider_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"etw.Provider",					/*tp_name*/
	sizeof(Provider),				/*tp_basicsize*/
	0,								/*tp_itemsize*/
	(destructor)Provider_dealloc,   /*tp_dealloc*/
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
	"Provider class",				/* tp_doc */
	0,								/* tp_traverse */
	0,								/* tp_clear */
	0,					            /* tp_richcompare */
	0,								/* tp_weaklistoffset */
	0,								/* tp_iter */
	0,								/* tp_iternext */
	Provider_methods,               /* tp_methods */
	Provider_members,               /* tp_members */
	0,								/* tp_getset */
	0,								/* tp_base */
	0,								/* tp_dict */
	0,								/* tp_descr_get */
	0,								/* tp_descr_set */
	0,								/* tp_dictoffset */
	(initproc)Provider_init,		/* tp_init */
	0,								/* tp_alloc */
	Provider_new,					/* tp_new */
};

PyObject* Provider_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds)
{
	Provider* self;

	self = (Provider*)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	self->guid = NULL;
	self->_provider = NULL;

	return (PyObject*)self;
}

int Provider_init(
	Provider *self,
	PyObject *args,
	PyObject *kwds
)
{
	static char *kwlist[] = { "guid", "callback", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ZO", kwlist,
		&self->guid,
		&self->callback))
		return -1;

	if (!PyCallable_Check(self->callback))
		return -1;

	self->_provider = new (std::nothrow) krabs::provider<>(krabs::guid(self->guid));
	if (!self->_provider)
		return -1;

	self->_provider->add_on_event_callback([self](const EVENT_RECORD &record) {
		event_callback(record, self->callback);
	});
	return 0;
}

void Provider_dealloc(
	PyObject* self)
{
	Provider* object = (Provider*)self;
	delete object->_provider;
	object->_provider = NULL;

	PyObject_Del(self);
}

PyObject *Provider_any(
	Provider* self,
	PyObject *args)
{
	ULONGLONG any;

	if (!PyArg_ParseTuple(args, "K", &any))
	{
		// FIXME: throw custom exception
		return NULL;
	}
	assert(self->_provider != NULL);
	self->_provider->any(any);
	Py_RETURN_NONE;
}

PyObject *Provider_all(
	Provider* self,
	PyObject *args)
{
	ULONGLONG all;

	if (!PyArg_ParseTuple(args, "K", &all))
	{
		// FIXME: throw custom exception
		return NULL;
	}
	assert(self->_provider != NULL);
	self->_provider->all(all);
	Py_RETURN_NONE;
}

PyObject *Provider_level(
	Provider* self,
	PyObject *args)
{
	ULONGLONG level;

	if (!PyArg_ParseTuple(args, "K", &level))
	{
		// FIXME: throw custom exception
		return NULL;
	}
	assert(self->_provider != NULL);
	self->_provider->level(level);
	Py_RETURN_NONE;
}
