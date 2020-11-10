#include "CastorTestParser/Database/DbStatement.hpp"

#include "CastorTestParser/Database/DbConnection.hpp"
#include "CastorTestParser/Database/DbParameter.hpp"
#include "CastorTestParser/Database/DbResult.hpp"
#include "CastorTestParser/Database/DbStatementParameter.hpp"
#include "CastorTestParser/Database/DbValuedObjectInfos.hpp"

#include <CastorUtils/Log/Logger.hpp>

namespace test_parser::db
{
	//*********************************************************************************************

	namespace
	{
		static const std::string ERROR_DB_STATEMENT_NOT_INITIALISED = "Statement is not initialised";
		static const std::string ERROR_STATEMENT_EXECUTION = "Statement execution failed: ";
		static const std::string ERROR_FIELD_RETRIEVAL = "Field retrieval error";
		static const std::string ERROR_SQLITE_MISSING_INITIALIZATION = "Method initialise must be called before calling method createParameter";
		static const std::string ERROR_SQLITE_CANT_PREPARE_STATEMENT = "Couldn't prepare the statement";
		static const std::string ERROR_SQLITE_QUERY_INCONSISTENCY = "Number of parameters doesn't match the sizes of parameter containers: ";

		static const char * INFO_SQLITE_STATEMENT_FINALISATION = "Statement finalisation";
		static const char * INFO_SQLITE_STMT_PARAMS_COUNT = "Bind Parameters count: ";
		static const char * INFO_SQLITE_STATEMENT_CLEAR_BINDINGS = "Statement bindings cleanup";
		static const char * INFO_SQLITE_STATEMENT_RESET = "Statement resetting";

		static const std::string WARNING_DB_STATEMENT_NULL_PARAMETER = "Trying to add a null parameter.";

		static const std::string SQLITE_SQL_DELIM = "?";
		static const std::string SQLITE_SQL_PARAM = "@";
		static const std::string SQLITE_SQL_SET = "SET @";
		static const std::string SQLITE_SQL_NULL = " = NULL;";
		static const std::string SQLITE_SQL_SELECT = "SELECT ";
		static const std::string SQLITE_SQL_AS = " AS ";
		static const std::string SQLITE_SQL_COMMA = ",";
	}

	//*********************************************************************************************

	Statement::ValueUpdater::ValueUpdater( Statement * stmt )
		: m_stmt{ stmt }
	{
	}

	void Statement::ValueUpdater::update( const Parameter & value )
	{
		m_stmt->m_paramsByPointer[value.getObjectValue().getPtrValue()] = &value;
	}

	//*********************************************************************************************

	Statement::Statement( Connection & connection, const std::string & query )
		: ParameteredObject()
		, m_connection{ connection }
		, m_query{ query }
	{
	}

	Statement::~Statement()
	{
		cleanup();
		if ( m_statement )
		{
			m_connection.check( sqlite3_finalize( m_statement )
				, INFO_SQLITE_STATEMENT_FINALISATION );
		}
	}

	bool Statement::executeUpdate()
	{
		if ( !m_initialised )
		{
			throw std::runtime_error{ ERROR_DB_STATEMENT_NOT_INITIALISED };
		}

		bool result = false;

		try
		{
			doPreExecute();
			result = m_connection.executeUpdate( m_statement );

			if ( result )
			{
				doPostExecute();
			}
		}
		catch ( std::exception & exc )
		{
			castor::Logger::logError( ERROR_STATEMENT_EXECUTION + exc.what() );
		}
		catch ( ... )
		{
			castor::Logger::logError( ERROR_STATEMENT_EXECUTION + "Unknown error" );
		}

		return result;
	}

	ResultPtr Statement::executeSelect()
	{
		if ( !m_initialised )
		{
			throw std::runtime_error{ ERROR_DB_STATEMENT_NOT_INITIALISED };
		}

		ResultPtr result;

		try
		{
			doPreExecute();
			result = m_connection.executeSelect( m_statement, m_infos );

			if ( result )
			{
				doPostExecute();
			}
		}
		catch ( std::exception & exc )
		{
			castor::Logger::logError( ERROR_STATEMENT_EXECUTION + exc.what() );
		}
		catch ( ... )
		{
			castor::Logger::logError( ERROR_STATEMENT_EXECUTION + "Unknown error" );
		}

		return result;
	}

	Parameter * Statement::createParameter( const std::string & name
		, FieldType fieldType
		, ParameterType parameterType )
	{
		return doCreateParameter( { name, fieldType }, parameterType );
	}

	Parameter * Statement::createParameter( const std::string & name
		, FieldType fieldType
		, uint32_t limits
		, ParameterType parameterType )
	{
		return doCreateParameter( { name, fieldType, limits }, parameterType );
	}

	Parameter * Statement::doCreateParameter( ValuedObjectInfos infos
		, ParameterType parameterType )
	{
		auto parameter = std::make_unique< StatementParameter >( m_connection
			, infos
			, uint16_t( m_inParams.size() + 1 )
			, parameterType
			, std::make_unique< ValueUpdater >( this ) );
		auto result = doAddParameter( std::move( parameter ) );

		if ( result && parameterType == ParameterType::eIn )
		{
			m_inParams.push_back( result );
		}

		return result;
	}

	bool Statement::doInitialise()
	{
		bool result{ false };

		if ( !m_query.empty() )
		{
			m_paramsCount = uint32_t( std::count( m_query.begin(), m_query.end(), '?' ) );
			m_queries = castor::string::split( m_query, "?", m_paramsCount + 1 );
		}

		assert( m_paramsCount == getParametersCount() );

		std::stringstream query;
		unsigned short i = 0;
		auto && itQueries = m_queries.begin();
		auto && itParams = doGetParameters().begin();
		auto && itParamsEnd = doGetParameters().end();

		m_outInitialisers.clear();
		m_outParams.clear();

		m_outInitialisers.reserve( getParametersCount() );
		m_outParams.reserve( getParametersCount() );

		while ( itQueries != m_queries.end() && itParams != itParamsEnd )
		{
			query << ( *itQueries );
			auto parameter = itParams->get();

			if ( parameter->getParamType() == ParameterType::eIn )
			{
				query << SQLITE_SQL_DELIM;
			}
			else if ( parameter->getParamType() == ParameterType::eInOut )
			{
				query << SQLITE_SQL_PARAM + parameter->getName();
				StatementPtr stmt = m_connection.createStatement( SQLITE_SQL_SET + parameter->getName() + " = " + SQLITE_SQL_DELIM );
				stmt->createParameter( parameter->getName(), parameter->getType(), parameter->getLimits(), ParameterType::eIn );
				stmt->initialise();
				m_outParams.push_back( parameter );
				m_inOutInitialisers.emplace_back( std::move( stmt ), parameter );
			}
			else if ( parameter->getParamType() == ParameterType::eOut )
			{
				query << SQLITE_SQL_PARAM + parameter->getName();
				StatementPtr stmt = m_connection.createStatement( SQLITE_SQL_SET + parameter->getName() + SQLITE_SQL_NULL );
				stmt->initialise();
				m_outParams.push_back( parameter );
				m_outInitialisers.emplace_back( std::move( stmt ) );
			}

			++i;
			++itQueries;
			++itParams;
		}

		while ( itQueries != m_queries.end() )
		{
			query << ( *itQueries );
			++itQueries;
		}

		m_query = query.str();

		if ( !m_outParams.empty() )
		{
			std::string sep;
			std::stringstream queryInOutParam;
			queryInOutParam << SQLITE_SQL_SELECT;

			for ( auto && parameter : m_outParams )
			{
				queryInOutParam << sep << SQLITE_SQL_PARAM << parameter->getName() << SQLITE_SQL_AS << parameter->getName();
				sep = SQLITE_SQL_COMMA;
			}

			m_stmtOutParams = m_connection.createStatement( queryInOutParam.str() );
			m_stmtOutParams->initialise();
		}

		m_statement = m_connection.prepareStatement( m_query );
		int count = sqlite3_bind_parameter_count( m_statement );

		if ( count == getParametersCount() )
		{
			castor::Logger::logDebug( INFO_SQLITE_STMT_PARAMS_COUNT + castor::string::toString( count ) );
			result = true;
		}
		else
		{
			std::stringstream error;
			error << ERROR_SQLITE_QUERY_INCONSISTENCY << getParametersCount() << ", Expected: " << count;
			throw std::runtime_error{ error.str() };
		}

		for ( auto && parameter : m_inParams )
		{
			static_cast< StatementParameter & >( *parameter ).setStatement( m_statement );
		}

		return result;
	}

	void Statement::doCleanup()
	{
		if ( m_statement )
		{
			m_connection.check( sqlite3_finalize( m_statement )
				, INFO_SQLITE_STATEMENT_FINALISATION );
			m_statement = nullptr;
		}

		m_inParams.clear();
		m_outParams.clear();
		m_outInitialisers.clear();
		m_queries.clear();
		m_paramsCount = 0;
		m_stmtOutParams.reset();
	}

	void Statement::doPreExecute()
	{
		for ( auto & it : m_inOutInitialisers )
		{
			auto parameter = it.second;

			if ( parameter->getObjectValue().isNull() )
			{
				it.first->setParameterNull( 0 );
			}
			else
			{
				it.first->setParameterValue( 0, static_cast< const ValuedObject & >( *parameter ) );
			}

			it.first->executeUpdate();
		}

		for ( auto & statement : m_outInitialisers )
		{
			statement->executeUpdate();
		}
	}

	void Statement::doPostExecute()
	{
		if ( !m_outParams.empty() )
		{
			ResultPtr result = m_stmtOutParams->executeSelect();

			if ( result && !result->empty() )
			{
				auto & row = *result->begin();

				for ( auto && parameter : m_outParams )
				{
					if ( parameter->getParamType() == ParameterType::eInOut
						|| parameter->getParamType() == ParameterType::eOut )
					{
						Field const * field{};

						try
						{
							field = &row.getField( parameter->getName() );
						}
						catch ( std::exception & exc )
						{
							castor::Logger::logError( ERROR_FIELD_RETRIEVAL + exc.what() );
						}
						catch ( ... )
						{
							castor::Logger::logError( ERROR_FIELD_RETRIEVAL + "Unknown error" );
						}

						if ( field )
						{
							parameter->setValue( static_cast< const ValuedObject & >( *field ) );
						}
					}
				}
			}
		}

		m_connection.check( sqlite3_clear_bindings( m_statement )
			, INFO_SQLITE_STATEMENT_CLEAR_BINDINGS );
		m_connection.check( sqlite3_reset( m_statement )
			, INFO_SQLITE_STATEMENT_RESET );
	}

	//*********************************************************************************************
}
