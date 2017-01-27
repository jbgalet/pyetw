#include "trace.h"

PyTypeObject UserTrace_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"etw.UserTrace",				/*tp_name*/
	sizeof(UserTrace),				/*tp_basicsize*/
	0,								/*tp_itemsize*/
	(destructor)UserTrace_dealloc,  /*tp_dealloc*/
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
	"UserTrace class",					/* tp_doc */
	0,								/* tp_traverse */
	0,								/* tp_clear */
	0,					            /* tp_richcompare */
	0,								/* tp_weaklistoffset */
	0,								/* tp_iter */
	0,								/* tp_iternext */
	UserTrace_methods,              /* tp_methods */
	UserTrace_members,              /* tp_members */
	0,								/* tp_getset */
	0,								/* tp_base */
	0,								/* tp_dict */
	0,								/* tp_descr_get */
	0,								/* tp_descr_set */
	0,								/* tp_dictoffset */
	(initproc)UserTrace_init,		/* tp_init */
	0,								/* tp_alloc */
	UserTrace_new,					/* tp_new */
};

PyObject* UserTrace_new(
	PyTypeObject *type,
	PyObject *args,
	PyObject *kwds)
{
	UserTrace* self;

	self = (UserTrace*)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	self->name = NULL;
	self->_trace = NULL;

	return (PyObject*)self;
}

int UserTrace_init(
	UserTrace *self,
	PyObject *args,
	PyObject *kwds
)
{
	static char *kwlist[] = { "name", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Z", kwlist,
		&self->name))
		return -1;
	self->_trace = new (std::nothrow) krabs::user_trace(self->name);
	if (!self->_trace)
		return -1;
	return 0;
}

void UserTrace_dealloc(
	PyObject* self)
{
	UserTrace* object = (UserTrace*)self;
	delete object->_trace;
	object->_trace = NULL;

	PyObject_Del(self);
}

PyObject *UserTrace_start(
	UserTrace* self)
{
	assert(self->_trace != NULL);

	Py_BEGIN_ALLOW_THREADS
	self->_trace->start();
	Py_END_ALLOW_THREADS
	return PyBool_FromLong(true);
}

PyObject *UserTrace_stop(
	UserTrace* self)
{
	assert(self->_trace != NULL);

	self->_trace->stop();
	// thread join
	return PyBool_FromLong(true);
}

PyObject *UserTrace_enable(
	UserTrace* self,
	PyObject* obj)
{
	assert(self->_trace != NULL);

	if (!PyObject_TypeCheck(obj, &Provider_Type))
	{
		// FIXME : set error
		return NULL;
	}
	Provider* provider = (Provider*)obj;
	assert(provider->_provider != NULL);

	self->_trace->enable(*(provider->_provider));

	Py_RETURN_NONE;
}
