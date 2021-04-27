#include "CastorUtilsPixelBufferExtractTest.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

using namespace castor;

namespace
{
	template< PixelFormat PfT >
	ByteArray createPixelDataT()
	{
		ByteArray result;
		result.resize( 16u * getBytesPerPixel( PfT ) );
		auto count = getComponentsCount( PfT );
		auto data = result.data();

		for ( auto i = 0u; i < 16u; ++i )
		{
			if constexpr ( is8UComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, uint8_t( ( count * i ) + 0 ) );
				PixelComponentsT< PfT >::G( data, uint8_t( ( count * i ) + 1 ) );
				PixelComponentsT< PfT >::B( data, uint8_t( ( count * i ) + 2 ) );
				PixelComponentsT< PfT >::A( data, uint8_t( ( count * i ) + 3 ) );
			}
			else if constexpr ( is8SComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, int8_t( ( count * i ) + 0 ) - 8 );
				PixelComponentsT< PfT >::G( data, int8_t( ( count * i ) + 1 ) - 8 );
				PixelComponentsT< PfT >::B( data, int8_t( ( count * i ) + 2 ) - 8 );
				PixelComponentsT< PfT >::A( data, int8_t( ( count * i ) + 3 ) - 8 );
			}
			else if constexpr ( is16UComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, uint16_t( ( count * i ) + 0 ) );
				PixelComponentsT< PfT >::G( data, uint16_t( ( count * i ) + 1 ) );
				PixelComponentsT< PfT >::B( data, uint16_t( ( count * i ) + 2 ) );
				PixelComponentsT< PfT >::A( data, uint16_t( ( count * i ) + 3 ) );
			}
			else if constexpr ( is16SComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, int16_t( ( count * i ) + 0 ) - 8 );
				PixelComponentsT< PfT >::G( data, int16_t( ( count * i ) + 1 ) - 8 );
				PixelComponentsT< PfT >::B( data, int16_t( ( count * i ) + 2 ) - 8 );
				PixelComponentsT< PfT >::A( data, int16_t( ( count * i ) + 3 ) - 8 );
			}
			else if constexpr ( is16FComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, int16_t( ( count * i ) + 0 ) - 8 );
				PixelComponentsT< PfT >::G( data, int16_t( ( count * i ) + 1 ) - 8 );
				PixelComponentsT< PfT >::B( data, int16_t( ( count * i ) + 2 ) - 8 );
				PixelComponentsT< PfT >::A( data, int16_t( ( count * i ) + 3 ) - 8 );
			}
			else if constexpr ( is32UComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, uint32_t( ( count * i ) + 0 ) );
				PixelComponentsT< PfT >::G( data, uint32_t( ( count * i ) + 1 ) );
				PixelComponentsT< PfT >::B( data, uint32_t( ( count * i ) + 2 ) );
				PixelComponentsT< PfT >::A( data, uint32_t( ( count * i ) + 3 ) );
			}
			else if constexpr ( is32SComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, int32_t( ( count * i ) + 0 ) - 8 );
				PixelComponentsT< PfT >::G( data, int32_t( ( count * i ) + 1 ) - 8 );
				PixelComponentsT< PfT >::B( data, int32_t( ( count * i ) + 2 ) - 8 );
				PixelComponentsT< PfT >::A( data, int32_t( ( count * i ) + 3 ) - 8 );
			}
			else if constexpr ( is32FComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, float( ( count * i ) + 0 ) );
				PixelComponentsT< PfT >::G( data, float( ( count * i ) + 1 ) );
				PixelComponentsT< PfT >::B( data, float( ( count * i ) + 2 ) );
				PixelComponentsT< PfT >::A( data, float( ( count * i ) + 3 ) );
			}
			else if constexpr ( is64UComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, uint64_t( ( count * i ) + 0 ) );
				PixelComponentsT< PfT >::G( data, uint64_t( ( count * i ) + 1 ) );
				PixelComponentsT< PfT >::B( data, uint64_t( ( count * i ) + 2 ) );
				PixelComponentsT< PfT >::A( data, uint64_t( ( count * i ) + 3 ) );
			}
			else if constexpr ( is64SComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, int64_t( ( count * i ) + 0 ) - 8 );
				PixelComponentsT< PfT >::G( data, int64_t( ( count * i ) + 1 ) - 8 );
				PixelComponentsT< PfT >::B( data, int64_t( ( count * i ) + 2 ) - 8 );
				PixelComponentsT< PfT >::A( data, int64_t( ( count * i ) + 3 ) - 8 );
			}
			else if constexpr ( is64FComponentsV< PfT > )
			{
				PixelComponentsT< PfT >::R( data, double( ( count * i ) + 0 ) );
				PixelComponentsT< PfT >::G( data, double( ( count * i ) + 1 ) );
				PixelComponentsT< PfT >::B( data, double( ( count * i ) + 2 ) );
				PixelComponentsT< PfT >::A( data, double( ( count * i ) + 3 ) );
			}

			data += PixelDefinitionsT< PfT >::Size;
		}

		return result;
	}

	template< PixelFormat PfT >
	PxBufferBaseSPtr createPixelBufferT()
	{
		auto data = createPixelDataT< PfT >();
		return PxBufferBase::create( { 4u, 4u }
			, PfT
			, data.data()
			, PfT );
	}

	template< PixelFormat PfT >
	void checkPixelExtractCompT( Testing::TestCase & test
		, PixelComponents components )
	{
		PxBufferBaseSPtr src = createPixelBufferT< PfT >();
		PxBufferBaseSPtr buffer = extractComponents( src, components );
		auto srcData = src->getConstPtr();
		auto dstData = buffer->getConstPtr();
		auto constexpr componentFormat = singleComponentV< PfT >;
		auto constexpr componentSize = PixelDefinitionsT< componentFormat >::Size;

		for ( uint32_t i = 0u; i < buffer->getCount(); ++i )
		{
			auto index = 0u;

			for ( auto component : components )
			{
				auto srcComponent = srcData + componentSize * getComponentIndex( component );
				auto dstComponent = dstData + componentSize * index;

				if ( hasComponent( PfT, component ) )
				{
					if constexpr ( is8UComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< uint8_t const * >( srcComponent )
							, *reinterpret_cast< uint8_t const * >( dstComponent ) );
					}
					else if constexpr ( is8SComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< int8_t const * >( srcComponent )
							, *reinterpret_cast< int8_t const * >( dstComponent ) );
					}
					else if constexpr ( is16UComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< uint16_t const * >( srcComponent )
							, *reinterpret_cast< uint16_t const * >( dstComponent ) );
					}
					else if constexpr ( is16SComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< int16_t const * >( srcComponent )
							, *reinterpret_cast< int16_t const * >( dstComponent ) );
					}
					else if constexpr ( is16FComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< int16_t const * >( srcComponent )
							, *reinterpret_cast< int16_t const * >( dstComponent ) );
					}
					else if constexpr ( is32UComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< uint32_t const * >( srcComponent )
							, *reinterpret_cast< uint32_t const * >( dstComponent ) );
					}
					else if constexpr ( is32SComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< int32_t const * >( srcComponent )
							, *reinterpret_cast< int32_t const * >( dstComponent ) );
					}
					else if constexpr ( is32FComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< float const * >( srcComponent )
							, *reinterpret_cast< float const * >( dstComponent ) );
					}
					else if constexpr ( is64UComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< uint64_t const * >( srcComponent )
							, *reinterpret_cast< uint64_t const * >( dstComponent ) );
					}
					else if constexpr ( is64SComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< int64_t const * >( srcComponent )
							, *reinterpret_cast< int64_t const * >( dstComponent ) );
					}
					else if constexpr ( is64FComponentsV< PfT > )
					{
						CT_EQUAL_EX( test
							, *reinterpret_cast< double const * >( srcComponent )
							, *reinterpret_cast< double const * >( dstComponent ) );
					}
				}
				else
				{
					if constexpr ( is8UComponentsV< PfT > )
					{

					}
					else if constexpr ( is32FComponentsV< PfT > )
					{
					}
				}

				++index;
			}

			dstData += getBytesPerPixel( buffer->getFormat() );
			srcData += PixelDefinitionsT< PfT >::Size;
		}
	}

	template< PixelFormat PfT >
	void checkPixelExtract1T( Testing::TestCase & test )
	{
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eGreen );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eBlue );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eAlpha );
	}

	template< PixelFormat PfT >
	void checkPixelExtract2ContigT( Testing::TestCase & test )
	{
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed | PixelComponent::eGreen );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eGreen | PixelComponent::eBlue );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eBlue | PixelComponent::eAlpha );
	}

	template< PixelFormat PfT >
	void checkPixelExtract3ContigT( Testing::TestCase & test )
	{
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed | PixelComponent::eGreen | PixelComponent::eBlue );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eGreen | PixelComponent::eBlue | PixelComponent::eAlpha );
	}

	template< PixelFormat PfT >
	void checkPixelExtract2DisjointT( Testing::TestCase & test )
	{
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed | PixelComponent::eBlue );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed | PixelComponent::eAlpha );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eGreen | PixelComponent::eAlpha );
	}

	template< PixelFormat PfT >
	void checkPixelExtract3DisjointT( Testing::TestCase & test )
	{
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed | PixelComponent::eGreen | PixelComponent::eAlpha );
		checkPixelExtractCompT< PfT >( test, PixelComponent::eRed | PixelComponent::eBlue | PixelComponent::eAlpha );
	}
}

namespace Testing
{
	CastorUtilsPixelBufferExtractTest::CastorUtilsPixelBufferExtractTest()
		:	TestCase( "CastorUtilsPixelBufferExtractTest" )
	{
	}

	CastorUtilsPixelBufferExtractTest::~CastorUtilsPixelBufferExtractTest()
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
		checkPixelExtract1T< PixelFormat::eR8_UNORM >( *this );
		checkPixelExtract1T< PixelFormat::eR8G8_UNORM >( *this );
		checkPixelExtract1T< PixelFormat::eR8G8B8_UNORM >( *this );
		checkPixelExtract1T< PixelFormat::eR8G8B8A8_UNORM >( *this );
		checkPixelExtract1T< PixelFormat::eR16_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16B16_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16B16A16_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16B16_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16B16A16_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR16_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16B16_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR16G16B16A16_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR32_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32B32_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32B32A32_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32B32_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32B32A32_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR32_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32B32_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR32G32B32A32_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR64_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64B64_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64B64A64_UINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64B64_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64B64A64_SINT >( *this );
		checkPixelExtract1T< PixelFormat::eR64_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64B64_SFLOAT >( *this );
		checkPixelExtract1T< PixelFormat::eR64G64B64A64_SFLOAT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract2ContigComponents()
	{
		checkPixelExtract2ContigT< PixelFormat::eR8_UNORM >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR8G8_UNORM >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR8G8B8_UNORM >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR8G8B8A8_UNORM >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16B16_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16B16A16_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16B16_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16B16A16_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16B16_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR16G16B16A16_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32B32_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32B32A32_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32B32_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32B32A32_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32B32_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR32G32B32A32_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64B64_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64B64A64_UINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64B64_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64B64A64_SINT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64B64_SFLOAT >( *this );
		checkPixelExtract2ContigT< PixelFormat::eR64G64B64A64_SFLOAT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract3ContigComponents()
	{
		checkPixelExtract3ContigT< PixelFormat::eR8_UNORM >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR8G8_UNORM >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR8G8B8_UNORM >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR8G8B8A8_UNORM >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16B16_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16B16A16_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16B16_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16B16A16_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16B16_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR16G16B16A16_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32B32_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32B32A32_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32B32_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32B32A32_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32B32_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR32G32B32A32_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64B64_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64B64A64_UINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64B64_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64B64A64_SINT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64B64_SFLOAT >( *this );
		checkPixelExtract3ContigT< PixelFormat::eR64G64B64A64_SFLOAT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract2DisjointComponents()
	{
		checkPixelExtract2DisjointT< PixelFormat::eR8_UNORM >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR8G8_UNORM >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR8G8B8_UNORM >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR8G8B8A8_UNORM >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16B16_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16B16A16_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16B16_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16B16A16_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16B16_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR16G16B16A16_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32B32_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32B32A32_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32B32_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32B32A32_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32B32_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR32G32B32A32_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64B64_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64B64A64_UINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64B64_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64B64A64_SINT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64B64_SFLOAT >( *this );
		checkPixelExtract2DisjointT< PixelFormat::eR64G64B64A64_SFLOAT >( *this );
	}

	void CastorUtilsPixelBufferExtractTest::TestExtract3DisjointComponents()
	{
		checkPixelExtract3DisjointT< PixelFormat::eR8_UNORM >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR8G8_UNORM >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR8G8B8_UNORM >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR8G8B8A8_UNORM >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16B16_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16B16A16_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16B16_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16B16A16_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16B16_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR16G16B16A16_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32B32_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32B32A32_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32B32_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32B32A32_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32B32_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR32G32B32A32_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64B64_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64B64A64_UINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64B64_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64B64A64_SINT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64B64_SFLOAT >( *this );
		checkPixelExtract3DisjointT< PixelFormat::eR64G64B64A64_SFLOAT >( *this );
	}
}
