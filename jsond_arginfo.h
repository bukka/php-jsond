/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 19c6712092a1fd8fda7de83d775b09fe3f5747df */

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
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, schema, JsondSchema, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_validate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, schema, JsondSchema, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_last_error_msg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsondSerializable_jsonSerialize, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_JsondSchema___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_JsondSchema_fromString, 0, 1, JsondSchema, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(jsond_encode);
ZEND_FUNCTION(jsond_decode);
ZEND_FUNCTION(jsond_validate);
ZEND_FUNCTION(jsond_last_error);
ZEND_FUNCTION(jsond_last_error_msg);
ZEND_METHOD(JsondSchema, __construct);
ZEND_METHOD(JsondSchema, fromString);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(jsond_encode, arginfo_jsond_encode)
	ZEND_FE(jsond_decode, arginfo_jsond_decode)
	ZEND_FE(jsond_validate, arginfo_jsond_validate)
	ZEND_FE(jsond_last_error, arginfo_jsond_last_error)
	ZEND_FE(jsond_last_error_msg, arginfo_jsond_last_error_msg)
	ZEND_FE_END
};

static const zend_function_entry class_JsondSerializable_methods[] = {
	ZEND_RAW_FENTRY("jsonSerialize", NULL, arginfo_class_JsondSerializable_jsonSerialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_JsondSchema_methods[] = {
	ZEND_ME(JsondSchema, __construct, arginfo_class_JsondSchema___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(JsondSchema, fromString, arginfo_class_JsondSchema_fromString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
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

	INIT_CLASS_ENTRY(ce, "JsondException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_JsondSchemaException(zend_class_entry *class_entry_JsondException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "JsondSchemaException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_JsondException, 0);

	return class_entry;
}

static zend_class_entry *register_class_JsondSchema(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "JsondSchema", class_JsondSchema_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
