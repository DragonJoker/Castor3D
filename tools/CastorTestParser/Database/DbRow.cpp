#include "CastorTestParser/Database/DbRow.hpp"

namespace test_parser::db
{
	namespace
	{
		static const std::string ERROR_DB_ROW_MISSING_FIELD_NAME = "Row misses field named: ";
		static const std::string ERROR_DB_ROW_MISSING_FIELD_INDEX = "Row misses field at index: ";
	}

	void Row::addField( FieldPtr field )
	{
		m_arrayFields.emplace_back( std::move( field ) );
	}

	Field const & Row::getField( const std::string & name ) const
	{
		auto it = std::find_if( m_arrayFields.begin()
			, m_arrayFields.end()
			, [&name]( FieldPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it == m_arrayFields.end() )
		{
			std::stringstream message;
			message << ERROR_DB_ROW_MISSING_FIELD_NAME << name;
			throw std::runtime_error{ message.str() };
		}

		return **it;
	}

	Field const & Row::getField( uint32_t index ) const
	{
		if ( index >= m_arrayFields.size() )
		{
			std::stringstream message;
			message << ERROR_DB_ROW_MISSING_FIELD_INDEX << index;
			throw std::runtime_error{ message.str() };
		}

		return *m_arrayFields[index];
	}

	Field & Row::getField( const std::string & name )
	{
		auto it = std::find_if( m_arrayFields.begin()
			, m_arrayFields.end()
			, [&name]( FieldPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it == m_arrayFields.end() )
		{
			std::stringstream message;
			message << ERROR_DB_ROW_MISSING_FIELD_NAME << name;
			throw std::runtime_error{ message.str() };
		}

		return **it;
	}

	Field & Row::getField( uint32_t index )
	{
		if ( index >= m_arrayFields.size() )
		{
			std::stringstream message;
			message << ERROR_DB_ROW_MISSING_FIELD_INDEX << index;
			throw std::runtime_error{ message.str() };
		}

		return *m_arrayFields[index];
	}

	bool Row::isNull( uint32_t index ) const
	{
		return getField( index ).isNull();
	}

	bool Row::isNull( const std::string & name ) const
	{
		return getField( name ).isNull();
	}
}
