/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbParameteredObject_HPP___
#define ___CTP_DbParameteredObject_HPP___

#include "DbPrerequisites.hpp"

namespace aria::db
{
	class ParameteredObject
	{
	public:
		ParameteredObject();
		virtual ~ParameteredObject() = default;

		bool initialise();
		void cleanup();

		inline uint16_t getParametersCount() const;
		Parameter * getParameter( uint32_t index )const;
		Parameter * getParameter( const std::string & name )const;
		FieldType getParameterType( uint32_t index );
		FieldType getParameterType( const std::string & name );

		void setParameterNull( uint32_t index );
		void setParameterNull( const std::string & name );
		void setParameterValue( uint32_t index, const ValuedObject & object );
		void setParameterValue( const std::string & name, const ValuedObject & object );

		template< typename T >
		inline void setParameterValue( uint32_t index, const T & value );
		template< typename T >
		inline void setParameterValue( const std::string & name, const T & value );

	protected:
		virtual bool doInitialise() = 0;
		virtual void doCleanup() = 0;
		virtual Parameter * doCreateParameter( ValuedObjectInfos infos, ParameterType parameterType ) = 0;
		Parameter * doAddParameter( ParameterPtr parameter );
		inline const ParameterArray & doGetParameters() const;

	protected:
		ParameterArray m_parameters;
		bool m_initialised;
	};
}

#include "DbParameteredObject.inl"

#endif
