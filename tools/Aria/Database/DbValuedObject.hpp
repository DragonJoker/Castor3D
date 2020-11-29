/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbValuedObject_HPP___
#define ___CTP_DbValuedObject_HPP___

#include "DbValueBase.hpp"
#include "DbValuedObjectInfos.hpp"

namespace aria::db
{
	class ValuedObject
	{
	public:
		ValuedObject( Connection & connection, ValuedObjectInfos infos );
		virtual ~ValuedObject() = default;

		ValueBase const & getObjectValue()const
		{
			return *m_value;
		}

		ValueBase & getObjectValue()
		{
			return *m_value;
		}

		FieldType getType() const
		{
			return m_infos.getType();
		}

		Connection & getConnection() const
		{
			return m_connection;
		}

		const std::string & getName() const
		{
			return m_infos.getName();
		}

		const uint32_t & getLimits() const
		{
			return m_infos.getLimits();
		}

	protected:
		virtual void doGetValue( bool & value ) const;
		virtual void doGetValue( int32_t & value ) const;
		virtual void doGetValue( uint32_t & value ) const;
		virtual void doGetValue( int64_t & value ) const;
		virtual void doGetValue( uint64_t & value ) const;
		virtual void doGetValue( float & value ) const;
		virtual void doGetValue( double & value ) const;
		virtual void doGetValue( std::string & value ) const;
		virtual void doGetValue( DateTime & value ) const;
		virtual void doGetValue( Date & value ) const;
		virtual void doGetValue( Time & value ) const;
		virtual void doGetValue( castor::ByteArray & value ) const;

		virtual void doSetValue( const bool & value );
		virtual void doSetValue( const int32_t & value );
		virtual void doSetValue( const uint32_t & value );
		virtual void doSetValue( const int64_t & value );
		virtual void doSetValue( const uint64_t & value );
		virtual void doSetValue( const float & value );
		virtual void doSetValue( const double & value );
		virtual void doSetValue( const std::string & value );
		virtual void doSetValue( const Date & value );
		virtual void doSetValue( const DateTime & value );
		virtual void doSetValue( const Time & value );
		virtual void doSetValue( const castor::ByteArray & value );

		virtual void doCreateValue();

	private:
		ValueBasePtr m_value;
		ValuedObjectInfos m_infos;
		Connection & m_connection;
	};
}

#endif
