#include "CastorTestParser/Database/DbParameteredObject.hpp"

#include "CastorTestParser/Database/DbParameter.hpp"

#include <CastorUtils/Log/Logger.hpp>

#include <algorithm>

namespace test_parser::db
{
	static const std::string ERROR_DB_PARAMETER_INDEX = "No query parameter at index: ";
	static const std::string ERROR_DB_PARAMETER_NAME = "No query parameter named: ";
	static const std::string ERROR_DB_PARAMETER_ALREADY_ADDED = "Parameter with givent name already exists: ";
	static const std::string ERROR_DB_PARAMETER_INITIALISATION = "Parametered object initialisation failed: ";
	static const std::string ERROR_DB_PARAMETER_CLEANUP = "Statement cleanup failed: ";

	static const std::string WARNING_DB_PARAMETER_NULL = "Trying to add a null parameter.";

	ParameteredObject::ParameteredObject()
		: m_initialised( false )
	{
	}

	bool ParameteredObject::initialise()
	{
		bool ret = false;

		try
		{
			ret = doInitialise();
			m_initialised = true;
		}
		catch ( std::exception & exc )
		{
			castor::Logger::logError( ERROR_DB_PARAMETER_INITIALISATION + exc.what() );
		}
		catch ( ... )
		{
			castor::Logger::logError( ERROR_DB_PARAMETER_INITIALISATION + "Unknown error" );
		}

		return ret;
	}

	void ParameteredObject::cleanup()
	{
		try
		{
			doCleanup();
		}
		catch ( std::exception & exc )
		{
			castor::Logger::logError( ERROR_DB_PARAMETER_CLEANUP + exc.what() );
		}
		catch ( ... )
		{
			castor::Logger::logError( ERROR_DB_PARAMETER_CLEANUP + "Unknown error" );
		}
	}

	Parameter * ParameteredObject::getParameter( uint32_t index )const
	{
		if ( index >= m_parameters.size() )
		{
			std::stringstream message;
			message << ERROR_DB_PARAMETER_INDEX << index;
			throw std::runtime_error{ message.str() };
		}

		return m_parameters[index].get();
	}

	Parameter * ParameteredObject::getParameter( const std::string & name )const
	{
		auto it = std::find_if( m_parameters.begin()
			, m_parameters.end()
			, [&name]( ParameterPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it == m_parameters.end() )
		{
			std::stringstream message;
			message << ERROR_DB_PARAMETER_NAME << name;
			throw std::runtime_error{ message.str() };
		}

		return it->get();
	}

	FieldType ParameteredObject::getParameterType( uint32_t index )
	{
		return getParameter( index )->getType();
	}

	FieldType ParameteredObject::getParameterType( const std::string & name )
	{
		return getParameter( name )->getType();
	}

	void ParameteredObject::setParameterNull( uint32_t index )
	{
		getParameter( index )->setNull();
	}

	void ParameteredObject::setParameterNull( const std::string & name )
	{
		getParameter( name )->setNull();
	}

	void ParameteredObject::setParameterValue( uint32_t index
		, const ValuedObject & object )
	{
		getParameter( index )->setValue( object );
	}

	void ParameteredObject::setParameterValue( const std::string & name
		, const ValuedObject & object )
	{
		getParameter( name )->setValue( object );
	}

	Parameter * ParameteredObject::doAddParameter( ParameterPtr parameter )
	{
		Parameter * ret;

		if ( parameter )
		{
			auto it = std::find_if( m_parameters.begin()
				, m_parameters.end()
				, [&parameter]( ParameterPtr const & lookup )
				{
					return lookup->getName() == parameter->getName();
				} );

			if ( it == m_parameters.end() )
			{
				ret = parameter.get();
				m_parameters.emplace_back( std::move( parameter ) );
			}
			else
			{
				throw std::runtime_error{ ERROR_DB_PARAMETER_ALREADY_ADDED + parameter->getName() };
			}
		}
		else
		{
			castor::Logger::logWarning( WARNING_DB_PARAMETER_NULL );
		}

		return ret;
	}
}
