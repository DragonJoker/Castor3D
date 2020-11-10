/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbValuePolicy_HPP___
#define ___CTP_DbValuePolicy_HPP___

#include "DbPrerequisites.hpp"

namespace test_parser::db
{
	template< FieldType FieldTypeT >
	struct ValuePolicyT
	{
		using value_type = typename FieldTypeDataTyperT< FieldTypeT >::value_type;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out = in;
			size = sizeof( value_type );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value;
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return castor::string::toString( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};
}

#include "DbValuePolicy.inl"

#endif
