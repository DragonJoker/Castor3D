#include "Castor3D/Castor3DPrerequisites.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"

CU_ImplementExportedOwnedBy( castor3d::Engine, Engine )
CU_ImplementExportedOwnedBy( castor3d::RenderSystem, RenderSystem )
CU_ImplementExportedOwnedBy( castor3d::Scene, Scene )

using namespace castor;

namespace castor3d
{
	ashes::Format convert( castor::PixelFormat format )
	{
		switch ( format )
		{
		case castor::PixelFormat::eR8_UNORM:
			return ashes::Format::eR8_UNORM;
		case castor::PixelFormat::eR16_SFLOAT:
			return ashes::Format::eR16_SFLOAT;
		case castor::PixelFormat::eR32_SFLOAT:
			return ashes::Format::eR32_SFLOAT;
		case castor::PixelFormat::eR8A8_UNORM:
			return ashes::Format::eR8G8_UNORM;
		case castor::PixelFormat::eR16A16_SFLOAT:
			return ashes::Format::eR16G16_SFLOAT;
		case castor::PixelFormat::eR32A32_SFLOAT:
			return ashes::Format::eR32G32_SFLOAT;
		case castor::PixelFormat::eR5G5B5A1_UNORM:
			return ashes::Format::eA1R5G5B5_UNORM_PACK16;
		case castor::PixelFormat::eR5G6B5_UNORM:
			return ashes::Format::eR5G5B5A1_UNORM_PACK16;
		case castor::PixelFormat::eR8G8B8_UNORM:
			return ashes::Format::eR8G8B8_UNORM;
		case castor::PixelFormat::eB8G8R8_UNORM:
			return ashes::Format::eB8G8R8_UNORM;
		case castor::PixelFormat::eR8G8B8_SRGB:
			return ashes::Format::eR8G8B8_SRGB;
		case castor::PixelFormat::eB8G8R8_SRGB:
			return ashes::Format::eB8G8R8_SRGB;
		case castor::PixelFormat::eR8G8B8A8_UNORM:
			return ashes::Format::eR8G8B8A8_UNORM;
		case castor::PixelFormat::eA8B8G8R8_UNORM:
			return ashes::Format::eA8B8G8R8_UNORM_PACK32;
		case castor::PixelFormat::eR8G8B8A8_SRGB:
			return ashes::Format::eR8G8B8A8_SRGB;
		case castor::PixelFormat::eA8B8G8R8_SRGB:
			return ashes::Format::eA8B8G8R8_SRGB_PACK32;
		case castor::PixelFormat::eR16G16B16_SFLOAT:
			return ashes::Format::eR16G16B16_SFLOAT;
		case castor::PixelFormat::eR16G16B16A16_SFLOAT:
			return ashes::Format::eR16G16B16A16_SFLOAT;
		case castor::PixelFormat::eR32G32B32_SFLOAT:
			return ashes::Format::eR32G32B32_SFLOAT;
		case castor::PixelFormat::eR32G32B32A32_SFLOAT:
			return ashes::Format::eR32G32B32A32_SFLOAT;
		case castor::PixelFormat::eBC1_RGB_UNORM_BLOCK:
			return ashes::Format::eBC1_RGB_UNORM_BLOCK;
		case castor::PixelFormat::eBC3_UNORM_BLOCK:
			return ashes::Format::eBC2_UNORM_BLOCK;
		case castor::PixelFormat::eBC5_UNORM_BLOCK:
			return ashes::Format::eBC3_UNORM_BLOCK;
		case castor::PixelFormat::eD16_UNORM:
			return ashes::Format::eD16_UNORM;
		case castor::PixelFormat::eD24_UNORM_S8_UINT:
			return ashes::Format::eD24_UNORM_S8_UINT;
		case castor::PixelFormat::eD32_UNORM:
			return ashes::Format::eX8_D24_UNORM_PACK32;
		case castor::PixelFormat::eD32_SFLOAT:
			return ashes::Format::eD32_SFLOAT;
		case castor::PixelFormat::eD32_SFLOAT_S8_UINT:
			return ashes::Format::eD32_SFLOAT_S8_UINT;
		case castor::PixelFormat::eS8_UINT:
			return ashes::Format::eS8_UINT;
		default:
			assert( false && "Unsupported castor::PixelFormat" );
			return ashes::Format::eR8G8B8A8_UNORM;
		}
	}

	castor::PixelFormat convert( ashes::Format format )
	{
		switch ( format )
		{
		case ashes::Format::eR8_UNORM:
			return castor::PixelFormat::eR8_UNORM;
		case ashes::Format::eR16_SFLOAT:
			return castor::PixelFormat::eR16_SFLOAT;
		case ashes::Format::eR32_SFLOAT:
			return castor::PixelFormat::eR32_SFLOAT;
		case ashes::Format::eR8G8_UNORM:
			return castor::PixelFormat::eR8A8_UNORM;
		case ashes::Format::eR16G16_SFLOAT:
			return castor::PixelFormat::eR16A16_SFLOAT;
		case ashes::Format::eR32G32_SFLOAT:
			return castor::PixelFormat::eR32A32_SFLOAT;
		case ashes::Format::eA1R5G5B5_UNORM_PACK16:
			return castor::PixelFormat::eR5G5B5A1_UNORM;
		case ashes::Format::eR5G5B5A1_UNORM_PACK16:
			return castor::PixelFormat::eR5G6B5_UNORM;
		case ashes::Format::eR8G8B8_UNORM:
			return castor::PixelFormat::eR8G8B8_UNORM;
		case ashes::Format::eB8G8R8_UNORM:
			return castor::PixelFormat::eB8G8R8_UNORM;
		case ashes::Format::eR8G8B8_SRGB:
			return castor::PixelFormat::eR8G8B8_SRGB;
		case ashes::Format::eB8G8R8_SRGB:
			return castor::PixelFormat::eB8G8R8_SRGB;
		case ashes::Format::eR8G8B8A8_UNORM:
			return castor::PixelFormat::eR8G8B8A8_UNORM;
		case ashes::Format::eA8B8G8R8_UNORM_PACK32:
			return castor::PixelFormat::eA8B8G8R8_UNORM;
		case ashes::Format::eR8G8B8A8_SRGB:
			return castor::PixelFormat::eR8G8B8A8_SRGB;
		case ashes::Format::eA8B8G8R8_SRGB_PACK32:
			return castor::PixelFormat::eA8B8G8R8_SRGB;
		case ashes::Format::eR16G16B16_SFLOAT:
			return castor::PixelFormat::eR16G16B16_SFLOAT;
		case ashes::Format::eR16G16B16A16_SFLOAT:
			return castor::PixelFormat::eR16G16B16A16_SFLOAT;
		case ashes::Format::eR32G32B32_SFLOAT:
			return castor::PixelFormat::eR32G32B32_SFLOAT;
		case ashes::Format::eR32G32B32A32_SFLOAT:
			return castor::PixelFormat::eR32G32B32A32_SFLOAT;
		case ashes::Format::eBC1_RGB_UNORM_BLOCK:
			return castor::PixelFormat::eBC1_RGB_UNORM_BLOCK;
		case ashes::Format::eBC2_UNORM_BLOCK:
			return castor::PixelFormat::eBC3_UNORM_BLOCK;
		case ashes::Format::eBC3_UNORM_BLOCK:
			return castor::PixelFormat::eBC5_UNORM_BLOCK;
		case ashes::Format::eD16_UNORM:
			return castor::PixelFormat::eD16_UNORM;
		case ashes::Format::eD24_UNORM_S8_UINT:
			return castor::PixelFormat::eD24_UNORM_S8_UINT;
		case ashes::Format::eX8_D24_UNORM_PACK32:
			return castor::PixelFormat::eD32_UNORM;
		case ashes::Format::eD32_SFLOAT:
			return castor::PixelFormat::eD32_SFLOAT;
		case ashes::Format::eD32_SFLOAT_S8_UINT:
			return castor::PixelFormat::eD32_SFLOAT_S8_UINT;
		case ashes::Format::eS8_UINT:
			return castor::PixelFormat::eS8_UINT;
		default:
			assert( false && "Unsupported ashes::Format" );
			return castor::PixelFormat::eR8G8B8A8_UNORM;
		}
	}

	castor::Matrix4x4f convert( std::array< float, 16 > const & value )
	{
		castor::Matrix4x4f result;
		std::memcpy( result.ptr(), value.data(), 16 * sizeof( float ) );
		return result;
	}

	ashes::ClearColorValue convert( castor::RgbaColour const & value )
	{
		ashes::ClearColorValue result;
		std::memcpy( result.float32.data(), value.constPtr(), 4 * sizeof( float ) );
		return result;
	}

	castor::RgbaColour convert( ashes::ClearColorValue const & value )
	{
		castor::RgbaColour result;
		std::memcpy( result.ptr(), value.float32.data(), 4 * sizeof( float ) );
		return result;
	}

	bool isShadowMapProgram( ProgramFlags const & flags )
	{
		return checkFlag( flags, ProgramFlag::eShadowMapDirectional )
			|| checkFlag( flags, ProgramFlag::eShadowMapSpot )
			|| checkFlag( flags, ProgramFlag::eShadowMapPoint );
	}

	FogType getFogType( SceneFlags const & flags )
	{
		FogType result = FogType::eDisabled;

		if ( checkFlag( flags, SceneFlag::eFogLinear) )
		{
			result = FogType::eLinear;
		}
		else if ( checkFlag( flags, SceneFlag::eFogExponential) )
		{
			result = FogType::eExponential;
		}
		else if ( checkFlag( flags, SceneFlag::eFogSquaredExponential ) )
		{
			result = FogType::eSquaredExponential;
		}

		return result;
	}

	ashes::Device const & getCurrentDevice( RenderSystem const & obj )
	{
		return *obj.getCurrentDevice();
	}

	ashes::Device const & getCurrentDevice( Engine const & obj )
	{
		return getCurrentDevice( *obj.getRenderSystem() );
	}

	ashes::Device const & getCurrentDevice( Scene const & obj )
	{
		return getCurrentDevice( *obj.getEngine() );
	}
}
