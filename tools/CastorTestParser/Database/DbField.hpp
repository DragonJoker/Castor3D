/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbField_HPP___
#define ___CTP_DbField_HPP___

#include "DbValuedObject.hpp"

namespace test_parser::db
{
	class Field
		: public ValuedObject
	{
	public:
		Field( Connection & connection, ValuedObjectInfos infos );

		bool isNull() const
		{
			return getObjectValue().isNull();
		}

		template< typename T >
		T getValue() const
		{
			T result;
			getValue( result );
			return result;
		}

		template< typename T >
		void getValue( T & value ) const
		{
			doGetValue( value );
		}
	};
}

#endif
