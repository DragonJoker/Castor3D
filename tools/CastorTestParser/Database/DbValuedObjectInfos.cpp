#include "CastorTestParser/Database/DbValuedObjectInfos.hpp"

namespace test_parser::db
{
	//*********************************************************************************************

	namespace
	{
		static const std::string ERROR_DB_MISSING_LIMITS = "Missing limits for field ";

		bool needsLimits( FieldType type )
		{
			bool result = false;

			switch ( type )
			{
			case FieldType::eChar:
			case FieldType::eVarchar:
			case FieldType::eBinary:
			case FieldType::eVarbinary:
				result = true;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}
	}

	//*********************************************************************************************

	ValuedObjectInfos::ValuedObjectInfos( const std::string & name )
		: ValuedObjectInfos( name, FieldType::eNull, -1 )
	{
	}

	ValuedObjectInfos::ValuedObjectInfos( const std::string & name
		, FieldType type )
		: ValuedObjectInfos( name, type, -1 )
	{
		if ( needsLimits( type ) )
		{
			throw std::runtime_error{ ERROR_DB_MISSING_LIMITS + getName() };
		}
	}

	ValuedObjectInfos::ValuedObjectInfos( const std::string & name
		, FieldType type
		, uint32_t limits )
		: m_name( name )
		, m_type( type )
		, m_limits( limits )
	{
	}

	void ValuedObjectInfos::setType( FieldType type )
	{
		if ( needsLimits( type ) )
		{
			throw std::runtime_error{ ERROR_DB_MISSING_LIMITS + getName() };
		}

		m_type = type;
	}

	void ValuedObjectInfos::setType( FieldType type, uint32_t limits )
	{
		m_type = type;
		m_limits = limits;
	}

	//*********************************************************************************************
}
