#include "Aria/Database/DbValuedObject.hpp"

#include "Aria/Database/DbValue.hpp"

namespace aria::db
{
	namespace
	{
		static const std::string ERROR_DB_FIELD_CREATION_TYPE = "Type error while creating the object: ";
	}

	//*********************************************************************************************

	ValuedObject::ValuedObject( Connection & connection, ValuedObjectInfos infos )
		: m_connection{ connection }
		, m_infos{ std::move( infos ) }
	{
	}

	void ValuedObject::doGetValue( bool & value ) const
	{
		assert( getType() == FieldType::eBIT );
		value = static_cast< ValueT< FieldType::eBIT > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( int32_t & value ) const
	{
		assert( getType() == FieldType::eSINT32 || getType() == FieldType::eUINT32 );
		value = static_cast< ValueT< FieldType::eSINT32 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( uint32_t & value ) const
	{
		assert( getType() == FieldType::eSINT32 || getType() == FieldType::eUINT32 );
		value = static_cast< ValueT< FieldType::eUINT32 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( int64_t & value ) const
	{
		assert( getType() == FieldType::eSINT64 || getType() == FieldType::eUINT64 );
		value = static_cast< ValueT< FieldType::eSINT64 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( uint64_t & value ) const
	{
		assert( getType() == FieldType::eSINT64 || getType() == FieldType::eUINT64 );
		value = static_cast< ValueT< FieldType::eUINT64 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( float & value ) const
	{
		assert( getType() == FieldType::eFLOAT32 );
		value = static_cast< ValueT< FieldType::eFLOAT32 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( double & value ) const
	{
		assert( getType() == FieldType::eFLOAT64 );
		value = static_cast< ValueT< FieldType::eFLOAT64 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( std::string & value ) const
	{
		assert( getType() == FieldType::eTEXT || getType() == FieldType::eVARCHAR || getType() == FieldType::eCHAR );

		if ( m_value->getPtrValue() )
		{
			if ( getType() == FieldType::eTEXT )
			{
				value = static_cast< ValueT< FieldType::eTEXT > & >( *m_value ).getValue();
			}
			else if ( getType() == FieldType::eVARCHAR )
			{
				value = static_cast< ValueT< FieldType::eVARCHAR > & >( *m_value ).getValue();
			}
			else
			{
				value = static_cast< ValueT< FieldType::eCHAR > & >( *m_value ).getValue();
			}
		}
		else
		{
			value.clear();
		}
	}

	void ValuedObject::doGetValue( Date & value ) const
	{
		assert( getType() == FieldType::eDATE );
		value = static_cast< ValueT< FieldType::eDATE > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( DateTime & value ) const
	{
		assert( getType() == FieldType::eDATETIME );
		value = static_cast< ValueT< FieldType::eDATETIME > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( Time & value ) const
	{
		assert( getType() == FieldType::eTIME );
		value = static_cast< ValueT< FieldType::eTIME > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( castor::ByteArray & value ) const
	{
		assert( getType() == FieldType::eBINARY || getType() == FieldType::eVARBINARY || getType() == FieldType::eBLOB );
		value = static_cast< ValueT< FieldType::eBINARY > & >( *m_value ).getValue();
	}

	void ValuedObject::doSetValue( const bool & value )
	{
		assert( getType() == FieldType::eBIT );
		static_cast< ValueT< FieldType::eBIT > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const int32_t & value )
	{
		assert( getType() == FieldType::eSINT32 || getType() == FieldType::eUINT32 );
		static_cast< ValueT< FieldType::eSINT32 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const uint32_t & value )
	{
		assert( getType() == FieldType::eSINT32 || getType() == FieldType::eUINT32 );
		static_cast< ValueT< FieldType::eUINT32 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const int64_t & value )
	{
		assert( getType() == FieldType::eSINT64 || getType() == FieldType::eUINT64 );
		static_cast< ValueT< FieldType::eSINT64 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const uint64_t & value )
	{
		assert( getType() == FieldType::eSINT64 || getType() == FieldType::eUINT64 );
		static_cast< ValueT< FieldType::eUINT64 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const float & value )
	{
		assert( getType() == FieldType::eFLOAT32 );
		static_cast< ValueT< FieldType::eFLOAT32 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const double & value )
	{
		assert( getType() == FieldType::eFLOAT64 );
		static_cast< ValueT< FieldType::eFLOAT64 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const std::string & value )
	{
		assert( getType() == FieldType::eTEXT || getType() == FieldType::eVARCHAR || getType() == FieldType::eCHAR );

		if ( getType() == FieldType::eCHAR )
		{
			static_cast< ValueT< FieldType::eCHAR > & >( *m_value ).setValue( value.c_str(), getLimits() );
		}
		else
		{
			static_cast< ValueT< FieldType::eTEXT > & >( *m_value ).setValue( value.c_str(), std::min( getLimits(), uint32_t( value.size() ) ) );
		}
	}

	void ValuedObject::doSetValue( const Date & value )
	{
		assert( getType() == FieldType::eDATE );
		static_cast< ValueT< FieldType::eDATE > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const DateTime & value )
	{
		assert( getType() == FieldType::eDATETIME );
		static_cast< ValueT< FieldType::eDATETIME > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const Time & value )
	{
		assert( getType() == FieldType::eTIME );
		static_cast< ValueT< FieldType::eTIME > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const castor::ByteArray & value )
	{
		assert( getType() == FieldType::eBINARY || getType() == FieldType::eVARBINARY || getType() == FieldType::eBLOB );
		static_cast< ValueT< FieldType::eBINARY > & >( *m_value ).setValue( value.data(), std::min( getLimits(), uint32_t( value.size() ) ) );
	}

	void ValuedObject::doCreateValue()
	{
		switch ( getType() )
		{
		case FieldType::eBIT:
			m_value = std::make_unique< ValueT< FieldType::eBIT > >( m_connection );
			break;

		case FieldType::eSINT32:
			m_value = std::make_unique< ValueT< FieldType::eSINT32 > >( m_connection );
			break;

		case FieldType::eSINT64:
			m_value = std::make_unique< ValueT< FieldType::eSINT64 > >( m_connection );
			break;

		case FieldType::eUINT32:
			m_value = std::make_unique< ValueT< FieldType::eUINT32 > >( m_connection );
			break;

		case FieldType::eUINT64:
			m_value = std::make_unique< ValueT< FieldType::eUINT64 > >( m_connection );
			break;

		case FieldType::eFLOAT32:
			m_value = std::make_unique< ValueT< FieldType::eFLOAT32 > >( m_connection );
			break;

		case FieldType::eFLOAT64:
			m_value = std::make_unique< ValueT< FieldType::eFLOAT64 > >( m_connection );
			break;

		case FieldType::eCHAR:
			m_value = std::make_unique< ValueT< FieldType::eCHAR > >( m_connection );
			break;

		case FieldType::eVARCHAR:
			m_value = std::make_unique< ValueT< FieldType::eVARCHAR > >( m_connection );
			break;

		case FieldType::eTEXT:
			m_value = std::make_unique< ValueT< FieldType::eTEXT > >( m_connection );
			break;

		case FieldType::eDATE:
			m_value = std::make_unique< ValueT< FieldType::eDATE > >( m_connection );
			break;

		case FieldType::eDATETIME:
			m_value = std::make_unique< ValueT< FieldType::eDATETIME > >( m_connection );
			break;

		case FieldType::eTIME:
			m_value = std::make_unique< ValueT< FieldType::eTIME > >( m_connection );
			break;

		case FieldType::eBINARY:
			m_value = std::make_unique< ValueT< FieldType::eBINARY > >( m_connection );
			break;

		case FieldType::eVARBINARY:
			m_value = std::make_unique< ValueT< FieldType::eVARBINARY > >( m_connection );
			break;

		case FieldType::eBLOB:
			m_value = std::make_unique< ValueT< FieldType::eBLOB > >( m_connection );
			break;

		default:
			std::string errMsg = ERROR_DB_FIELD_CREATION_TYPE + getName();
			throw std::runtime_error{ errMsg };
			break;
		}
	}

	//*********************************************************************************************
}
