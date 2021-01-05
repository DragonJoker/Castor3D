#include "Aria/Database/DbTransaction.hpp"

#include "Aria/Database/DbConnection.hpp"

namespace aria::db
{
	namespace
	{
		static const std::string SQLITE_SQL_NAMED_TRANSACTION_BEGIN = "SAVEPOINT ";
		static const std::string SQLITE_SQL_NAMED_TRANSACTION_COMMIT = "RELEASE ";
		static const std::string SQLITE_SQL_NAMED_TRANSACTION_ROLLBACK = "ROLLBACK TO ";
	}

	Transaction::Transaction( Connection & connection
		, std::string name )
		: m_connection{ connection }
		, m_name{ std::move( name ) }
		, m_valid{ m_connection.executeUpdate( SQLITE_SQL_NAMED_TRANSACTION_BEGIN + m_name ) }
	{
	}

	bool Transaction::commit()
	{
		if ( !m_valid )
		{
			return false;
		}

		return m_connection.executeUpdate( SQLITE_SQL_NAMED_TRANSACTION_COMMIT + m_name );
	}

	bool Transaction::rollback()
	{
		if ( !m_valid )
		{
			return false;
		}

		return m_connection.executeUpdate( SQLITE_SQL_NAMED_TRANSACTION_ROLLBACK + m_name );
	}
}
