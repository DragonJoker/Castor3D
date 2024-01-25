/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslAppendBuffer_H___
#define ___C3D_GlslAppendBuffer_H___

#include "SdwModule.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	template< typename DataT >
	class AppendArrayT
		: public sdw::StructInstance
	{
	public:
		SDW_DeclStructInstance( , AppendArrayT );

		AppendArrayT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, castor::move( expr ), enabled }
			, m_count{ getMember< sdw::UInt >( "count" ) }
			, m_data{ this->template getMemberArray< DataT >( "data" ) }
		{
		}

		template< typename ... ParamsT >
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, castor::MbString const & name
			, uint32_t arraySize
			, ParamsT && ... params )
		{
			auto result = cache.getStruct( sdw::type::MemoryLayout::eC
				, "C3D_AppendArray" + name );

			if ( result->empty() )
			{
				result->declMember( "count"
					, sdw::type::Kind::eUInt
					, sdw::type::NotArray );
				result->declMember( "data"
					, DataT::makeType( cache, castor::forward< ParamsT >( params )... )
					, arraySize );
			}

			return result;
		}

		template< sdw::type::Kind KindT >
		DataT operator[]( sdw::IntegerValue< KindT > const & offset )const
		{
			return m_data[offset];
		}

		DataT operator[]( int32_t offset )const
		{
			return m_data[offset];
		}

		DataT operator[]( uint32_t offset )const
		{
			return m_data[offset];
		}

		void resetCount()
		{
			m_count = 0_u32;
		}

		sdw::UInt getCount()const
		{
			return m_count;
		}

		sdw::UInt incrementCount()const
		{
			return sdw::atomicAdd( m_count, 1_u32 );
		}

		void appendData( DataT const data )
		{
			sdw::ShaderWriter & writer = *this->getWriter();
			auto idx = writer.declLocale( "appendIndex", this->incrementCount() );
			m_data[idx] = data;
		}

		void appendData( DataT const data
			, sdw::UInt maxCount )
		{
			sdw::ShaderWriter & writer = *this->getWriter();
			auto idx = writer.declLocale( "appendIndex", this->incrementCount() );

			IF( *m_writer, idx < maxCount )
			{
				m_data[idx] = data;
			}
			FI;
		}

		void appendData( DataT const data
			, sdw::Int maxCount )
		{
			sdw::ShaderWriter & writer = *this->getWriter();
			this->appendData( data, writer.cast< sdw::UInt >( maxCount ) );
		}

		void appendData( DataT const data
			, u32 maxCount )
		{
			this->appendData( data, sdw::UInt{ maxCount } );
		}

	private:
		sdw::UInt m_count;
		sdw::Array< DataT > m_data;
	};

	class AppendBufferBase
	{
	public:
		C3D_API AppendBufferBase( sdw::ShaderWriter & writer
			, castor::MbString blockName
			, castor::MbString variableName
			, uint32_t binding
			, uint32_t set
			, bool enabled = true );

		C3D_API sdw::UInt incrementCount()const;

		sdw::UInt32 getCount()const
		{
			return *m_count;
		}

		bool isEnabled()const noexcept
		{
			return m_ssbo != nullptr;
		}

		sdw::ShaderWriter * getWriter()const noexcept
		{
			return &m_writer;
		}

	protected:
		sdw::ShaderWriter & m_writer;
		castor::RawUniquePtr< sdw::StorageBuffer > m_ssbo;
		castor::RawUniquePtr< sdw::UInt32 > m_count;
		castor::MbString m_variableName;
	};

	template< typename DataT >
	class AppendBufferT
		: public AppendBufferBase
	{
	public:
		template< typename ... ParamsT >
		AppendBufferT( sdw::ShaderWriter & writer
			, castor::String blockName
			, castor::String variableName
			, uint32_t binding
			, uint32_t set
			, bool enabled = true
			, ParamsT && ... params )
			: AppendBufferBase{ writer
				, castor::move( blockName )
				, castor::move( variableName )
				, binding
				, set
				, enabled }
		{
			if ( isEnabled() )
			{
				m_data = castor::make_unique< sdw::Array< DataT > >( m_ssbo->declMemberArray< DataT >( "d"
					, true
					, castor::forward< ParamsT >( params )... ) );
				m_ssbo->end();
			}
		}

		template< typename ... ParamsT >
		DataT getData( sdw::UInt const & index
			, ParamsT && ... params )const
		{
			return ( m_ssbo
				? ( *m_data )[index]
				: m_writer.declLocale< DataT >( "disabled_" + m_variableName + "_data", false, castor::forward< ParamsT >( params )... ) );
		}

		void appendData( DataT const data )
		{
			if ( m_ssbo )
			{
				auto idx = m_writer.declLocale( m_variableName + "_idx", this->incrementCount() );
				( *m_data )[idx] = data;
			}
		}

		void appendData( DataT const data
			, sdw::UInt const maxCount )
		{
			if ( m_ssbo )
			{
				auto idx = m_writer.declLocale( m_variableName + "_idx", this->incrementCount() );

				IF( m_writer, idx < maxCount )
				{
					( *m_data )[idx] = data;
				}
				FI;
			}
		}

		void appendData( DataT const data
			, uint32_t maxCount )
		{
			if ( m_ssbo )
			{
				auto idx = m_writer.declLocale( m_variableName + "_idx", this->incrementCount() );

				IF( m_writer, idx < maxCount )
				{
					( *m_data )[idx] = data;
				}
				FI;
			}
		}

	private:
		castor::RawUniquePtr< sdw::Array< DataT > > m_data;
	};
}

#endif
