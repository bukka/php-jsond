/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6cb161069f30ade9bfe22f65a0dd369b8521b3a2 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_jsond_encode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_jsond_decode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, assoc, _IS_BOOL, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_jsond_last_error_msg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsondSerializable_jsonSerialize, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(jsond_encode);
ZEND_FUNCTION(jsond_decode);
ZEND_FUNCTION(jsond_last_error);
ZEND_FUNCTION(jsond_last_error_msg);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(jsond_encode, arginfo_jsond_encode)
	ZEND_FE(jsond_decode, arginfo_jsond_decode)
	ZEND_FE(jsond_last_error, arginfo_jsond_last_error)
	ZEND_FE(jsond_last_error_msg, arginfo_jsond_last_error_msg)
	ZEND_FE_END
};

static const zend_function_entry class_JsondSerializable_methods[] = {
	ZEND_RAW_FENTRY("jsonSerialize", NULL, arginfo_class_JsondSerializable_jsonSerialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};
