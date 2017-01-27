#include "event.h"

static PyObject *getPythonProperty(krabs::parser &parser, krabs::property &property)
{
	switch (property.type())
	{
	case TDH_INTYPE_NULL:
		Py_RETURN_NONE;

	case TDH_INTYPE_UNICODESTRING:
	{
		std::wstring prop_value = parser.parse<std::wstring>(property.name());
		return PyUnicode_FromUnicode(prop_value.c_str(), prop_value.length());
	}

	case TDH_INTYPE_ANSISTRING:
	{
		std::string prop_value = parser.parse<std::string>(property.name());
		return PyUnicode_DecodeFSDefaultAndSize(prop_value.c_str(), prop_value.length());
	}

	case TDH_INTYPE_INT8:
	{
		uint8_t prop_value = parser.parse<uint8_t>(property.name());
		return PyLong_FromLong(prop_value);
	}

	case TDH_INTYPE_UINT8:
	{
		uint8_t prop_value = parser.parse<uint8_t>(property.name());
		return PyLong_FromUnsignedLong(prop_value);
	}

	case TDH_INTYPE_INT16:
	{
		int16_t prop_value = parser.parse<int16_t>(property.name());
		return PyLong_FromLong(prop_value);
	}

	case TDH_INTYPE_UINT16:
	{
		uint16_t prop_value = parser.parse<uint16_t>(property.name());
		return PyLong_FromUnsignedLong(prop_value);
	}

	case TDH_INTYPE_INT32:
	{
		int32_t prop_value = parser.parse<int32_t>(property.name());
		return PyLong_FromLong(prop_value);
	}

	case TDH_INTYPE_UINT32:
	{
		uint32_t prop_value = parser.parse<uint32_t>(property.name());
		return PyLong_FromUnsignedLong(prop_value);
	}

	case TDH_INTYPE_INT64:
	{
		int64_t prop_value = parser.parse<int64_t>(property.name());
#ifdef HAVE_LONG_LONG
		return PyLong_FromLongLong(prop_value);
#else
		return PyLong_FromLong(prop_value);
#endif
	}

	case TDH_INTYPE_UINT64:
	{
		uint64_t prop_value = parser.parse<uint64_t>(property.name());
#ifdef HAVE_LONG_LONG
		return PyLong_FromUnsignedLongLong(prop_value);
#else
		return PyLong_FromUnsignedLong(prop_value);
#endif
	}

	case TDH_INTYPE_FLOAT:
	{
		float prop_value = parser.parse<float>(property.name());
		return PyFloat_FromDouble(prop_value);
	}

	case TDH_INTYPE_DOUBLE:
	{
		double prop_value = parser.parse<double>(property.name());
		return PyFloat_FromDouble(prop_value);
	}

	case TDH_INTYPE_BOOLEAN:
	{
		bool prop_value = parser.parse<bool>(property.name());
		return PyBool_FromLong(prop_value);
	}

	case TDH_INTYPE_BINARY:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		return PyBytes_FromStringAndSize((char *)prop_value.bytes().data(), prop_value.bytes().size());
	}

	case TDH_INTYPE_GUID:
	{
		GUID guid = parser.parse<GUID>(property.name());

		WCHAR szGuid[50];

		StringFromGUID2(guid, szGuid, sizeof(szGuid) - 1);
		return PyUnicode_FromUnicode(szGuid, wcsnlen_s(szGuid, sizeof(szGuid)));
	}

	case TDH_INTYPE_POINTER:
	{
		uint64_t prop_value = parser.parse<uint64_t>(property.name());
#ifdef HAVE_LONG_LONG
		return PyLong_FromLongLong(prop_value);
#else
		return PyLong_FromLong(prop_value);
#endif
	}

	case TDH_INTYPE_FILETIME:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		const BYTE *data = prop_value.bytes().data();
		ULARGE_INTEGER value;
		value.HighPart = ((PFILETIME)data)->dwHighDateTime;
		value.LowPart = ((PFILETIME)data)->dwLowDateTime;
#ifdef HAVE_LONG_LONG
		return PyLong_FromUnsignedLongLong(value.QuadPart);
#else
		return PyLong_FromUnsignedLong(value.QuadPart);
#endif
	}

	case TDH_INTYPE_SYSTEMTIME:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		const BYTE *data = prop_value.bytes().data();
		
		FILETIME ftime;
		SystemTimeToFileTime((PSYSTEMTIME)data, &ftime);
		ULARGE_INTEGER value;
		value.HighPart = ftime.dwHighDateTime;
		value.LowPart = ftime.dwLowDateTime;
#ifdef HAVE_LONG_LONG
		return PyLong_FromUnsignedLongLong(value.QuadPart);
#else
		return PyLong_FromUnsignedLong(value.QuadPart);
#endif
	}

	case TDH_INTYPE_SID:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		const BYTE *data = prop_value.bytes().data();
		WCHAR *szSID;
		ConvertSidToStringSidW((PSID)data, &szSID);
		PyObject *ret = PyUnicode_FromUnicode(szSID, wcslen(szSID));
		LocalFree(szSID);
		return ret;
	}

	case TDH_INTYPE_HEXINT32:
	{
		krabs::hexint32 prop_value = parser.parse<krabs::hexint32>(property.name());
		return PyLong_FromUnsignedLong(prop_value.value);
	}
	case TDH_INTYPE_HEXINT64:
	{
		krabs::hexint64 prop_value = parser.parse<krabs::hexint64>(property.name());
#ifdef HAVE_LONG_LONG
		return PyLong_FromUnsignedLongLong(prop_value.value);
#else
		return PyLong_FromUnsignedLong(prop_value.value);
#endif
	}
	
	// End of winmeta intypes.
	// Start of TDH intypes for WBEM.

	case TDH_INTYPE_COUNTEDSTRING:
	case TDH_INTYPE_REVERSEDCOUNTEDSTRING:
	case TDH_INTYPE_NONNULLTERMINATEDSTRING:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		const BYTE *pData = prop_value.bytes().data();
		size_t StringLength = 0;

		if (TDH_INTYPE_COUNTEDSTRING == property.type())
		{
			StringLength = *(PUSHORT)pData;
		}
		else if (TDH_INTYPE_REVERSEDCOUNTEDSTRING == property.type())
		{
			StringLength = MAKEWORD(HIBYTE((PUSHORT)pData), LOBYTE((PUSHORT)pData));
		}
		else if (TDH_INTYPE_NONNULLTERMINATEDSTRING == property.type())
		{
			StringLength = prop_value.bytes().size();
		}
		return PyUnicode_FromUnicode((const WCHAR *)pData, StringLength);
	}

	case TDH_INTYPE_COUNTEDANSISTRING:
	case TDH_INTYPE_REVERSEDCOUNTEDANSISTRING:
	case TDH_INTYPE_NONNULLTERMINATEDANSISTRING:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		const BYTE *pData = prop_value.bytes().data();
		size_t StringLength = 0;

		if (TDH_INTYPE_COUNTEDSTRING == property.type())
		{
			StringLength = *(PUSHORT)pData;
		}
		else if (TDH_INTYPE_REVERSEDCOUNTEDSTRING == property.type())
		{
			StringLength = MAKEWORD(HIBYTE((PUSHORT)pData), LOBYTE((PUSHORT)pData));
		}
		else if (TDH_INTYPE_NONNULLTERMINATEDSTRING == property.type())
		{
			StringLength = prop_value.bytes().size();
		}
		return PyUnicode_DecodeFSDefaultAndSize((const CHAR *)pData, StringLength);
	}

	case TDH_INTYPE_UNICODECHAR:
	case TDH_INTYPE_ANSICHAR:
	{
		uint8_t prop_value = parser.parse<uint8_t>(property.name());
		return PyLong_FromUnsignedLong(prop_value);
	}
	case TDH_INTYPE_SIZET:
	{
		uint64_t prop_value = parser.parse<uint64_t>(property.name());
#ifdef HAVE_LONG_LONG
		return PyLong_FromLongLong(prop_value);
#else
		return PyLong_FromLong(prop_value);
#endif
	}
	case TDH_INTYPE_HEXDUMP:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		return PyBytes_FromStringAndSize((char *)prop_value.bytes().data(), prop_value.bytes().size());
	}

	case TDH_INTYPE_WBEMSID:
	{
		krabs::binary prop_value = parser.parse<krabs::binary>(property.name());
		const BYTE *data = prop_value.bytes().data();

		// A WBEM SID is actually a TOKEN_USER structure followed 
		// by the SID. The size of the TOKEN_USER structure differs 
		// depending on whether the events were generated on a 32-bit 
		// or 64-bit architecture. Also the structure is aligned
		// on an 8-byte boundary, so its size is 8 bytes on a
		// 32-bit computer and 16 bytes on a 64-bit computer.
		// Doubling the pointer size handles both cases.
		data += sizeof(PVOID) * 2;
		WCHAR *szSID;
		ConvertSidToStringSidW((PSID)data, &szSID);
		PyObject *ret = PyUnicode_FromUnicode(szSID, wcslen(szSID));
		LocalFree(szSID);
		return ret;
	}

	default:
		throw std::runtime_error("Unknown property type");
	}
}

static PyObject *get_property_value(krabs::parser &parser, krabs::property &property)
{
	PyObject *val = NULL;

	if (property.flags() & PropertyStruct)
	{
		val = PyDict_New();
		for (krabs::property &subproperty : property.get_subproperties())
		{
			std::wstring prop_name = subproperty.name();
			PyObject *key = PyUnicode_FromUnicode(prop_name.c_str(), prop_name.length());
			PyObject *subval = get_property_value(parser, subproperty);
			PyDict_SetItem(val, key, subval);

			// No need to maintain reference here
			Py_DECREF(key);
			Py_DECREF(subval);
		}
	}
	else
	{
		try
		{
			val = getPythonProperty(parser, property);
		}
		catch (const std::exception &exc)
		{
			val = Py_None;
			Py_INCREF(val);
		}
	}

	return val;
}

void event_callback(const EVENT_RECORD &record, PyObject *callback)
{
	krabs::schema schema(record);
	krabs::parser parser(schema);

	PyObject *propDict = PyDict_New();
	for (krabs::property &property : parser.properties())
	{
		std::wstring prop_name = property.name();
		PyObject *key = PyUnicode_FromUnicode(prop_name.c_str(), prop_name.length());

		PyObject *val = get_property_value(parser, property);

		PyDict_SetItem(propDict, key, val);

		// No need to maintain reference here
		Py_DECREF(key);
		Py_DECREF(val);
	}

	PyGILState_STATE state = PyGILState_Ensure();
	PyObject *result = PyObject_CallFunction(
		callback,
		"uiuO",
		schema.event_name(),
		schema.event_id(),
		schema.provider_name(),
		propDict,
		NULL
	);
	PyGILState_Release(state);
}