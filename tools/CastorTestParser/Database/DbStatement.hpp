/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbStatement_HPP___
#define ___CTP_DbStatement_HPP___

#include "DbParameteredObject.hpp"

namespace test_parser::db
{
	class Statement
		: public ParameteredObject
	{
	public:
		Statement( Connection & connection, const std::string & query );
		~Statement();

		bool executeUpdate();
		ResultPtr executeSelect();

		Parameter * createParameter( const std::string & name
			, FieldType fieldType
			, ParameterType parameterType = ParameterType::eIn );
		Parameter * createParameter( const std::string & name
			, FieldType fieldType
			, uint32_t limits
			, ParameterType parameterType = ParameterType::eIn );

		template< typename T >
		T const & getOutputValue( uint32_t index );
		template< typename T >
		T const & getOutputValue( const std::string & name );
		template< typename T >
		void getOutputValue( uint32_t index, T & value );
		template< typename T >
		void getOutputValue( const std::string & name, T & value );

	private:
		Parameter * doCreateParameter( ValuedObjectInfos infos, ParameterType parameterType ) override;
		bool doInitialise() override;
		void doCleanup() override;
		void doPreExecute();
		void doPostExecute();

	private:
		std::map< const void *, Parameter const * > m_paramsByPointer;
		std::string m_query;
		Connection & m_connection;
		ValuedObjectInfosArray m_infos;
		sqlite3_stmt * m_statement{};
		StringArray m_queries;
		uint32_t m_paramsCount{};
		std::vector< Parameter * > m_inParams;
		std::vector< Parameter * > m_outParams;
		std::vector< std::pair< StatementPtr, Parameter * > > m_inOutInitialisers;
		std::vector< StatementPtr > m_outInitialisers;
		StatementPtr m_stmtOutParams;

		struct ValueUpdater
			: public Parameter::ValueUpdater
		{
			explicit ValueUpdater( Statement * stmt );

			void update( const Parameter & value ) override;

			Statement * m_stmt;
		};
	};
}

#include "DbStatement.inl"

#endif
