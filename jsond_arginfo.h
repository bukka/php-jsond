/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 771a3d2fde31a30afc98ab9e275f09838839027b */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_jsond_encode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_decode, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_validate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_last_error_msg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsondSerializable_jsonSerialize, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(jsond_encode);
ZEND_FUNCTION(jsond_decode);
ZEND_FUNCTION(jsond_validate);
ZEND_FUNCTION(jsond_last_error);
ZEND_FUNCTION(jsond_last_error_msg);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(jsond_encode, arginfo_jsond_encode)
	ZEND_FE(jsond_decode, arginfo_jsond_decode)
	ZEND_FE(jsond_validate, arginfo_jsond_validate)
	ZEND_FE(jsond_last_error, arginfo_jsond_last_error)
	ZEND_FE(jsond_last_error_msg, arginfo_jsond_last_error_msg)
	ZEND_FE_END
};


static const zend_function_entry class_JsondSerializable_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(JsondSerializable, jsonSerialize, arginfo_class_JsondSerializable_jsonSerialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_JsondException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_JsondSerializable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "JsondSerializable", class_JsondSerializable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_JsondException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "JsondException", class_JsondException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return class_entry;
}
