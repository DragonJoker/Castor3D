/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbFieldType_HPP___
#define ___CTP_DbFieldType_HPP___

namespace test_parser::db
{
	enum class FieldType
	{
		eNull,
		eNULL = eNull,
		eBit,
		eBIT = eBit,
		eSint32,
		eSINT32 = eSint32,
		eSint64,
		eSINT64 = eSint64,
		eUint32,
		eUINT32 = eUint32,
		eUint64,
		eUINT64 = eUint64,
		eFloat32,
		eFLOAT32 = eFloat32,
		eFloat64,
		eFLOAT64 = eFloat64,
		eChar,
		eCHAR = eChar,
		eVarchar,
		eVARCHAR = eVarchar,
		eText,
		eTEXT = eText,
		eDate,
		eDATE = eDate,
		eDatetime,
		eDATETIME = eDatetime,
		eTime,
		eTIME = eTime,
		eBinary,
		eBINARY = eBinary,
		eVarbinary,
		eVARBINARY = eVarbinary,
		eBlob,
		eBLOB = eBlob,
		eCount
	};
}

#endif
