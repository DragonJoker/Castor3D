/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbPrerequisites_HPP___
#define ___CTP_DbPrerequisites_HPP___

#include <CastorUtils/Data/Path.hpp>

#include <memory>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "DbFieldType.hpp"
#include "DbParameterType.hpp"

namespace aria::db
{
#	include "sqlite3.h"

	using castor::ByteArray;

	using StringArray = std::vector< std::string >;

	//YYYY-mm-dd
	static constexpr unsigned long SQLITE_STMT_DATE_SIZE = 10;
	//YYYY-mm-dd HH:MM:SS
	static constexpr unsigned long SQLITE_STMT_DATETIME_SIZE = 19;
	//HH:MM:SS
	static constexpr unsigned long SQLITE_STMT_TIME_SIZE = 8;

	static const std::string NULL_VALUE = "NULL";

	typedef boost::gregorian::date Date;
	typedef boost::posix_time::time_duration Time;
	typedef boost::posix_time::ptime DateTime;

	class Connection;
	class Field;
	class Parameter;
	class ParameteredObject;
	class Result;
	class Row;
	class Statement;
	class Transaction;
	class ValueBase;
	class ValuedObject;
	class ValuedObjectInfos;

	template< FieldType FieldTypeT >
	struct FieldTypeNeedsLimitsT : std::false_type{};
	template< FieldType FieldTypeT >
	struct FieldTypeDataTyperT;
	template< FieldType FieldTypeT >
	struct ValuePolicyT;
	template< FieldType FieldTypeT, typename PolicyT = ValuePolicyT< FieldTypeT > >
	class ValueT;

	using FieldPtr = std::unique_ptr< Field >;
	using ResultPtr = std::unique_ptr< Result >;
	using ValueBasePtr = std::unique_ptr< ValueBase >;
	using ParameterPtr = std::unique_ptr< Parameter >;
	using StatementPtr = std::unique_ptr< Statement >;

	using FieldArray = std::vector< FieldPtr >;
	using ParameterArray = std::vector< ParameterPtr >;
	using ValuedObjectInfosArray = std::vector< ValuedObjectInfos >;
	using RowArray = std::vector< Row >;

	void sqliteCheck( int code, castor::xchar const * msg, sqlite3 * connection );
	void sqliteCheck( int code, std::string const & msg, sqlite3 * connection );
	void sqliteCheck( int code, std::ostream const & stream, sqlite3 * connection );
}

#endif
