#include "DbConnection.hpp"

namespace test_parser::db
{
	//*********************************************************************************************

	/** FieldTypeNeedsLimitsT for FieldType::eCHAR
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eCHAR > : std::true_type {};

	/** FieldTypeNeedsLimitsT for FieldType::eVARCHAR
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eVARCHAR > : std::true_type {};

	/** FieldTypeNeedsLimitsT for FieldType::eBINARY
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eBINARY > : std::true_type {};

	/** FieldTypeNeedsLimitsT for FieldType::eVARBINARY
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eVARBINARY > : std::true_type {};

	//*********************************************************************************************

	/** FieldTypeDataTyperT specialization for FieldType::eBIT
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eBIT >
	{
		static const size_t size = 1;
		using value_type = bool;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eSINT32
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eSINT32 >
	{
		static const size_t size = 32;
		using value_type = int32_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eSINT64
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eSINT64 >
	{
		static const size_t size = 64;
		using value_type = int64_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eUINT32
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eUINT32 >
	{
		static const size_t size = 32;
		using value_type = uint32_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eUINT64
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eUINT64 >
	{
		static const size_t size = 64;
		using value_type = uint64_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eFLOAT32
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eFLOAT32 >
	{
		static const size_t size = 32;
		using value_type = float;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eFLOAT64
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eFLOAT64 >
	{
		static const size_t size = 64;
		using value_type = double;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eDATE
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eDATE >
	{
		static const size_t size = 0;
		using value_type = Date;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eDATETIME
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eDATETIME >
	{
		static const size_t size = 0;
		using value_type = DateTime;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eTIME
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eTIME >
	{
		static const size_t size = 0;
		using value_type = Time;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eCHAR
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eCHAR >
	{
		static const size_t size = 0;
		using value_type = std::string;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eVARCHAR
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eVARCHAR >
	{
		static const size_t size = 0;
		using value_type = std::string;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eTEXT
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eTEXT >
	{
		static const size_t size = 0;
		using value_type = std::string;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eBINARY
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eBINARY >
	{
		static const size_t size = 0;
		using value_type = castor::ByteArray;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eVARBINARY
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eVARBINARY >
	{
		static const size_t size = 0;
		using value_type = castor::ByteArray;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eBLOB
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eBLOB >
	{
		static const size_t size = 0;
		using value_type = castor::ByteArray;
	};

	//*********************************************************************************************

	/** ValuePolicyT specialization for FieldType::eFLOAT32 type
	*/
	template<> struct ValuePolicyT< FieldType::eFLOAT32 >
	{
		using value_type = FieldTypeDataTyperT< FieldType::eFLOAT32 >::value_type;

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
				return connection.writeFloat( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eFLOAT64 type
	*/
	template<> struct ValuePolicyT< FieldType::eFLOAT64 >
	{
		using value_type = FieldTypeDataTyperT< FieldType::eFLOAT64 >::value_type;

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
				return connection.writeDouble( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eBIT type
	*/
	template<> struct ValuePolicyT< FieldType::eBIT >
	{
		using value_type = FieldTypeDataTyperT< FieldType::eBIT >::value_type;

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
				return connection.writeBool( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** Policy used for text, char and nvarchar fields
	*/
	struct TextValuePolicyT
	{
		typedef std::string value_type;

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
			size = ( unsigned long )( in.size() );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value[0];
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value[0];
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
				return connection.writeText( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eVARCHAR type
	*/
	template<> struct ValuePolicyT< FieldType::eCHAR >
		: public TextValuePolicyT
	{
		using value_type = TextValuePolicyT::value_type;
	};

	/** ValuePolicyT specialization for FieldType::eVARCHAR type
	*/
	template<> struct ValuePolicyT< FieldType::eVARCHAR >
		: public TextValuePolicyT
	{
		using value_type = TextValuePolicyT::value_type;
	};

	/** ValuePolicyT specialization for FieldType::eTEXT type
	*/
	template<> struct ValuePolicyT< FieldType::eTEXT >
		: public TextValuePolicyT
	{
		using value_type = TextValuePolicyT::value_type;
	};

	/** ByteArray value policy (shared amongst FieldType::eBINARY, FieldType::eVARBINARY, and FieldType::eBLOB)
	*/
	struct ByteArrayValuePolicyT
	{
		using value_type = castor::ByteArray;

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
			out.clear();

			if ( !in.empty() )
			{
				out.reserve( in.size() );
				out.insert( out.end(), in.begin(), in.end() );
				size = ( unsigned long )( in.size() );
			}

			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			void * result = NULL;

			if ( !value.empty() )
			{
				result = value.data();
			}

			return result;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			void const * result = NULL;

			if ( !value.empty() )
			{
				result = value.data();
			}

			return result;
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
				return connection.writeBinary( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eBINARY type
	*/
	template<> struct ValuePolicyT< FieldType::eBINARY >
		: public ByteArrayValuePolicyT
	{
		typedef ByteArrayValuePolicyT::value_type value_type;
	};

	/** ValuePolicyT specialization for FieldType::eVARBINARY type
	*/
	template<> struct ValuePolicyT< FieldType::eVARBINARY >
		: public ByteArrayValuePolicyT
	{
		typedef ByteArrayValuePolicyT::value_type value_type;
	};

	/** ValuePolicyT specialization for FieldType::eBLOB type
	*/
	template<> struct ValuePolicyT< FieldType::eBLOB >
		: public ByteArrayValuePolicyT
	{
		typedef ByteArrayValuePolicyT::value_type value_type;
	};

	/** ValuePolicyT specialization for FieldType::eDATE type
	*/
	template<> struct ValuePolicyT< FieldType::eDATE >
	{
		typedef FieldTypeDataTyperT< FieldType::eDATE >::value_type value_type;

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
			size = ( unsigned long )( connection.getStmtDateSize() );
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
				return connection.writeDate( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eTIME type
	*/
	template<> struct ValuePolicyT< FieldType::eTIME >
	{
		typedef FieldTypeDataTyperT< FieldType::eTIME >::value_type value_type;

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
			size = ( unsigned long )( connection.getStmtTimeSize() );
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
				return connection.writeTime( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eDATETIME type
	*/
	template<> struct ValuePolicyT< FieldType::eDATETIME >
	{
		typedef FieldTypeDataTyperT< FieldType::eDATETIME >::value_type value_type;

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
			size = ( unsigned long )( connection.getStmtDateTimeSize() );
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
				return connection.writeDateTime( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	//*********************************************************************************************
}
