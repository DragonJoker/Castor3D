#include "CastorUtilsPixelBufferExtractTest.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

namespace Testing
{
	namespace details
	{
		template< castor::PixelFormat PfT >
		castor::ByteArray createPixelDataT()
		{
			castor::ByteArray result;
			result.resize( 16u * getBytesPerPixel( PfT ) );
			auto count = getComponentsCount( PfT );
			auto data = result.data();

			for ( auto i = 0u; i < 16u; ++i )
			{
				if constexpr ( castor::is8UComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, uint8_t( ( count * i ) + 0 ) );
					castor::PixelComponentsT< PfT >::G( data, uint8_t( ( count * i ) + 1 ) );
					castor::PixelComponentsT< PfT >::B( data, uint8_t( ( count * i ) + 2 ) );
					castor::PixelComponentsT< PfT >::A( data, uint8_t( ( count * i ) + 3 ) );
				}
				else if constexpr ( castor::is8SComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, int8_t( ( count * i ) + 0 ) - 8 );
					castor::PixelComponentsT< PfT >::G( data, int8_t( ( count * i ) + 1 ) - 8 );
					castor::PixelComponentsT< PfT >::B( data, int8_t( ( count * i ) + 2 ) - 8 );
					castor::PixelComponentsT< PfT >::A( data, int8_t( ( count * i ) + 3 ) - 8 );
				}
				else if constexpr ( castor::is16UComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, uint16_t( ( count * i ) + 0 ) );
					castor::PixelComponentsT< PfT >::G( data, uint16_t( ( count * i ) + 1 ) );
					castor::PixelComponentsT< PfT >::B( data, uint16_t( ( count * i ) + 2 ) );
					castor::PixelComponentsT< PfT >::A( data, uint16_t( ( count * i ) + 3 ) );
				}
				else if constexpr ( castor::is16SComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, int16_t( ( count * i ) + 0 ) - 8 );
					castor::PixelComponentsT< PfT >::G( data, int16_t( ( count * i ) + 1 ) - 8 );
					castor::PixelComponentsT< PfT >::B( data, int16_t( ( count * i ) + 2 ) - 8 );
					castor::PixelComponentsT< PfT >::A( data, int16_t( ( count * i ) + 3 ) - 8 );
				}
				else if constexpr ( castor::is16FComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, int16_t( ( count * i ) + 0 ) - 8 );
					castor::PixelComponentsT< PfT >::G( data, int16_t( ( count * i ) + 1 ) - 8 );
					castor::PixelComponentsT< PfT >::B( data, int16_t( ( count * i ) + 2 ) - 8 );
					castor::PixelComponentsT< PfT >::A( data, int16_t( ( count * i ) + 3 ) - 8 );
				}
				else if constexpr ( castor::is32UComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, uint32_t( ( count * i ) + 0 ) );
					castor::PixelComponentsT< PfT >::G( data, uint32_t( ( count * i ) + 1 ) );
					castor::PixelComponentsT< PfT >::B( data, uint32_t( ( count * i ) + 2 ) );
					castor::PixelComponentsT< PfT >::A( data, uint32_t( ( count * i ) + 3 ) );
				}
				else if constexpr ( castor::is32SComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, int32_t( ( count * i ) + 0 ) - 8 );
					castor::PixelComponentsT< PfT >::G( data, int32_t( ( count * i ) + 1 ) - 8 );
					castor::PixelComponentsT< PfT >::B( data, int32_t( ( count * i ) + 2 ) - 8 );
					castor::PixelComponentsT< PfT >::A( data, int32_t( ( count * i ) + 3 ) - 8 );
				}
				else if constexpr ( castor::is32FComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, float( ( count * i ) + 0 ) );
					castor::PixelComponentsT< PfT >::G( data, float( ( count * i ) + 1 ) );
					castor::PixelComponentsT< PfT >::B( data, float( ( count * i ) + 2 ) );
					castor::PixelComponentsT< PfT >::A( data, float( ( count * i ) + 3 ) );
				}
				else if constexpr ( castor::is64UComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, uint64_t( ( count * i ) + 0 ) );
					castor::PixelComponentsT< PfT >::G( data, uint64_t( ( count * i ) + 1 ) );
					castor::PixelComponentsT< PfT >::B( data, uint64_t( ( count * i ) + 2 ) );
					castor::PixelComponentsT< PfT >::A( data, uint64_t( ( count * i ) + 3 ) );
				}
				else if constexpr ( castor::is64SComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, int64_t( ( count * i ) + 0 ) - 8 );
					castor::PixelComponentsT< PfT >::G( data, int64_t( ( count * i ) + 1 ) - 8 );
					castor::PixelComponentsT< PfT >::B( data, int64_t( ( count * i ) + 2 ) - 8 );
					castor::PixelComponentsT< PfT >::A( data, int64_t( ( count * i ) + 3 ) - 8 );
				}
				else if constexpr ( castor::is64FComponentsV< PfT > )
				{
					castor::PixelComponentsT< PfT >::R( data, double( ( count * i ) + 0 ) );
					castor::PixelComponentsT< PfT >::G( data, double( ( count * i ) + 1 ) );
					castor::PixelComponentsT< PfT >::B( data, double( ( count * i ) + 2 ) );
					castor::PixelComponentsT< PfT >::A( data, double( ( count * i ) + 3 ) );
				}

				data += castor::PixelDefinitionsT< PfT >::Size;
			}

			return result;
		}

		template< castor::PixelFormat PfT >
		castor::PxBufferBaseUPtr createPixelBufferT()
		{
			auto data = createPixelDataT< PfT >();
			return castor::PxBufferBase::create( { 4u, 4u }
				, PfT
				, data.data()
				, PfT );
		}

		template< castor::PixelFormat PfT >
		void checkPixelExtractCompT( Testing::TestCase & test
			, castor::PixelComponents components )
		{
			CT_WHEN_EX( test, "components == " + castor::toUtf8( getName( components ) ) );
			auto src = createPixelBufferT< PfT >();
			auto buffer = castor::extractComponents( src.get(), components );
			auto srcData = src->getConstPtr();
			auto dstData = buffer->getConstPtr();
			auto constexpr componentFormat = castor::singleComponentV< PfT >;
			auto constexpr componentSize = castor::PixelDefinitionsT< componentFormat >::Size;

			for ( uint32_t i = 0u; i < buffer->getCount(); ++i )
			{
				auto index = 0u;

				for ( auto component : components )
				{
					auto srcComponent = srcData + componentSize * getComponentIndex( component, PfT );
					auto dstComponent = dstData + componentSize * index;

					if ( hasComponent( PfT, component ) )
					{
						if constexpr ( castor::is8UComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< uint8_t const * >( srcComponent )
								, *reinterpret_cast< uint8_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is8SComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< int8_t const * >( srcComponent )
								, *reinterpret_cast< int8_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is16UComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< uint16_t const * >( srcComponent )
								, *reinterpret_cast< uint16_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is16SComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< int16_t const * >( srcComponent )
								, *reinterpret_cast< int16_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is16FComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< int16_t const * >( srcComponent )
								, *reinterpret_cast< int16_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is32UComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< uint32_t const * >( srcComponent )
								, *reinterpret_cast< uint32_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is32SComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< int32_t const * >( srcComponent )
								, *reinterpret_cast< int32_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is32FComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< float const * >( srcComponent )
								, *reinterpret_cast< float const * >( dstComponent ) );
						}
						else if constexpr ( castor::is64UComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< uint64_t const * >( srcComponent )
								, *reinterpret_cast< uint64_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is64SComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< int64_t const * >( srcComponent )
								, *reinterpret_cast< int64_t const * >( dstComponent ) );
						}
						else if constexpr ( castor::is64FComponentsV< PfT > )
						{
							CT_EQUAL_EX( test
								, *reinterpret_cast< double const * >( srcComponent )
								, *reinterpret_cast< double const * >( dstComponent ) );
						}
					}

					++index;
				}

				dstData += getBytesPerPixel( buffer->getFormat() );
				srcData += castor::PixelDefinitionsT< PfT >::Size;
			}
		}

		template< castor::PixelFormat PfT >
		struct CheckPixelExtract1T
		{
			void operator()( Testing::TestCase & test )const
			{
				CT_ON_EX( test, "PfT == " + castor::toUtf8( getFormatName( PfT ) ) );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eGreen );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eBlue );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eAlpha );
			}
		};

		template< castor::PixelFormat PfT >
		struct CheckPixelExtract2ContigT
		{
			void operator()( Testing::TestCase & test )const
			{
				CT_ON_EX( test, "PfT == " + castor::toUtf8( getFormatName( PfT ) ) );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed | castor::PixelComponent::eGreen );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eGreen | castor::PixelComponent::eBlue );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eBlue | castor::PixelComponent::eAlpha );
			}
		};

		template< castor::PixelFormat PfT >
		struct CheckPixelExtract3ContigT
		{
			void operator()( Testing::TestCase & test )const
			{
				CT_ON_EX( test, "PfT == " + castor::toUtf8( getFormatName( PfT ) ) );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed | castor::PixelComponent::eGreen | castor::PixelComponent::eBlue );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eGreen | castor::PixelComponent::eBlue | castor::PixelComponent::eAlpha );
			}
		};

		template< castor::PixelFormat PfT >
		struct CheckPixelExtract2DisjointT
		{
			void operator()( Testing::TestCase & test )const
			{
				CT_ON_EX( test, "PfT == " + castor::toUtf8( getFormatName( PfT ) ) );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed | castor::PixelComponent::eBlue );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed | castor::PixelComponent::eAlpha );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eGreen | castor::PixelComponent::eAlpha );
			}
		};

		template< castor::PixelFormat PfT >
		struct CheckPixelExtract3DisjointT
		{
			void operator()( Testing::TestCase & test )const
			{
				CT_ON_EX( test, "PfT == " + castor::toUtf8( getFormatName( PfT ) ) );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed | castor::PixelComponent::eGreen | castor::PixelComponent::eAlpha );
				checkPixelExtractCompT< PfT >( test, castor::PixelComponent::eRed | castor::PixelComponent::eBlue | castor::PixelComponent::eAlpha );
			}
		};

		template< template< castor::PixelFormat PfT > typename FunctorT >
		void checkPixelExtractT( Testing::TestCase & test )
		{
			FunctorT< castor::PixelFormat::eR8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eR8G8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eR8G8B8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eB8G8R8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eR8G8B8A8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eB8G8R8A8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eA8B8G8R8_UNORM >{}( test );
			FunctorT< castor::PixelFormat::eR8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eR8G8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eR8G8B8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eB8G8R8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eR8G8B8A8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eB8G8R8A8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eA8B8G8R8_SRGB >{}( test );
			FunctorT< castor::PixelFormat::eR16_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16B16_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16B16A16_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR16_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16B16_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16B16A16_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR16_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16B16_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR16G16B16A16_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR32_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32B32_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32B32A32_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR32_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32B32_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32B32A32_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR32_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32B32_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR32G32B32A32_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR64_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64B64_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64B64A64_UINT >{}( test );
			FunctorT< castor::PixelFormat::eR64_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64B64_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64B64A64_SINT >{}( test );
			FunctorT< castor::PixelFormat::eR64_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64B64_SFLOAT >{}( test );
			FunctorT< castor::PixelFormat::eR64G64B64A64_SFLOAT >{}( test );
		}
	}

	CastorUtilsPixelBufferExtractTest::CastorUtilsPixelBufferExtractTest()
		:	TestCase( "CastorUtilsPixelBufferExtractTest" )
	{
	}

	void CastorUtilsPixelBufferExtractTest::doRegisterTests()
	{
		doRegisterTest( "TestExtract1Component", std::bind( &CastorUtilsPixelBufferExtractTest::TestExtract1Component, this ) );
		doRegisterTest( "TestExtract2ContigComponents", std::bind( &CastorUtilsPixelBufferExtractTest::TestExtract2ContigComponents, this ) );
		doRegisterTest( "TestExtract3ContigComponents", std::bind( &CastorUtilsPixelBufferExtractTest::TestExtract3ContigComponents, this ) );
		doRegisterTest( "TestExtract2DisjointComponents", std::bind( &CastorUtilsPixelBufferExtractTest::TestExtract2DisjointComponents, this ) );
		doRegisterTest( "TestExtract3DisjointComponents", std::bind( &CastorUtilsPixelBufferExtractTest::TestExtract3DisjointComponents, this ) );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract1Component()
	{
		details::checkPixelExtractT< details::CheckPixelExtract1T >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract2ContigComponents()
	{
		details::checkPixelExtractT< details::CheckPixelExtract2ContigT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract3ContigComponents()
	{
		details::checkPixelExtractT< details::CheckPixelExtract3ContigT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract2DisjointComponents()
	{
		details::checkPixelExtractT< details::CheckPixelExtract2DisjointT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract3DisjointComponents()
	{
		details::checkPixelExtractT< details::CheckPixelExtract3DisjointT >( *this );
	}
}
