/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbValueBase_HPP___
#define ___CTP_DbValueBase_HPP___

#include "DbPrerequisites.hpp"

namespace aria::db
{
	class ValueBase
	{
	public:
		explicit ValueBase( Connection & connection );
		virtual ~ValueBase() = default;

		virtual void setValue( ValueBase const & value ) = 0;
		virtual std::string getQueryValue()const = 0;
		virtual void * getPtrValue() = 0;
		virtual const void * getPtrValue() const = 0;

		unsigned long & getPtrSize()
		{
			return m_valueSize;
		}

		const unsigned long & getPtrSize()const
		{
			return m_valueSize;
		}

		bool isNull() const
		{
			return m_isNull;
		}

		void setNull( bool null = true )
		{
			m_isNull = null;
		}

	protected:
		virtual void doSetNull() = 0;

	protected:
		Connection & m_connection;
		unsigned long m_valueSize{};
		bool m_isNull{ true };
	};
}

#endif
