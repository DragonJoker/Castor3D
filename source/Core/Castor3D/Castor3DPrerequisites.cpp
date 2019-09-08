#include "Castor3D/Castor3DPrerequisites.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"

CU_ImplementExportedOwnedBy( castor3d::Engine, Engine )
CU_ImplementExportedOwnedBy( castor3d::RenderSystem, RenderSystem )
CU_ImplementExportedOwnedBy( castor3d::Scene, Scene )
CU_ImplementExportedOwnedBy( castor3d::RenderDevice, RenderDevice )

using namespace castor;

namespace castor3d
{
	VkFormat convert( castor::PixelFormat format )
	{
		switch ( format )
		{
		case castor::PixelFormat::eR8_UNORM:
			return VK_FORMAT_R8_UNORM;
		case castor::PixelFormat::eR16_SFLOAT:
			return VK_FORMAT_R16_SFLOAT;
		case castor::PixelFormat::eR32_SFLOAT:
			return VK_FORMAT_R32_SFLOAT;
		case castor::PixelFormat::eR8A8_UNORM:
			return VK_FORMAT_R8G8_UNORM;
		case castor::PixelFormat::eR16A16_SFLOAT:
			return VK_FORMAT_R16G16_SFLOAT;
		case castor::PixelFormat::eR32A32_SFLOAT:
			return VK_FORMAT_R32G32_SFLOAT;
		case castor::PixelFormat::eR5G5B5A1_UNORM:
			return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
		case castor::PixelFormat::eR5G6B5_UNORM:
			return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
		case castor::PixelFormat::eR8G8B8_UNORM:
			return VK_FORMAT_R8G8B8_UNORM;
		case castor::PixelFormat::eB8G8R8_UNORM:
			return VK_FORMAT_B8G8R8_UNORM;
		case castor::PixelFormat::eR8G8B8_SRGB:
			return VK_FORMAT_R8G8B8_SRGB;
		case castor::PixelFormat::eB8G8R8_SRGB:
			return VK_FORMAT_B8G8R8_SRGB;
		case castor::PixelFormat::eR8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case castor::PixelFormat::eA8B8G8R8_UNORM:
			return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
		case castor::PixelFormat::eR8G8B8A8_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case castor::PixelFormat::eA8B8G8R8_SRGB:
			return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
		case castor::PixelFormat::eR16G16B16_SFLOAT:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case castor::PixelFormat::eR16G16B16A16_SFLOAT:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case castor::PixelFormat::eR32G32B32_SFLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case castor::PixelFormat::eR32G32B32A32_SFLOAT:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case castor::PixelFormat::eBC1_RGB_UNORM_BLOCK:
			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case castor::PixelFormat::eBC3_UNORM_BLOCK:
			return VK_FORMAT_BC2_UNORM_BLOCK;
		case castor::PixelFormat::eBC5_UNORM_BLOCK:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case castor::PixelFormat::eD16_UNORM:
			return VK_FORMAT_D16_UNORM;
		case castor::PixelFormat::eD24_UNORM_S8_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case castor::PixelFormat::eD32_UNORM:
			return VK_FORMAT_X8_D24_UNORM_PACK32;
		case castor::PixelFormat::eD32_SFLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case castor::PixelFormat::eD32_SFLOAT_S8_UINT:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case castor::PixelFormat::eS8_UINT:
			return VK_FORMAT_S8_UINT;
		default:
			assert( false && "Unsupported castor::PixelFormat" );
			return VK_FORMAT_R8G8B8A8_UNORM;
		}
	}

	castor::PixelFormat convert( VkFormat format )
	{
		switch ( format )
		{
		case VK_FORMAT_R8_UNORM:
			return castor::PixelFormat::eR8_UNORM;
		case VK_FORMAT_R16_SFLOAT:
			return castor::PixelFormat::eR16_SFLOAT;
		case VK_FORMAT_R32_SFLOAT:
			return castor::PixelFormat::eR32_SFLOAT;
		case VK_FORMAT_R8G8_UNORM:
			return castor::PixelFormat::eR8A8_UNORM;
		case VK_FORMAT_R16G16_SFLOAT:
			return castor::PixelFormat::eR16A16_SFLOAT;
		case VK_FORMAT_R32G32_SFLOAT:
			return castor::PixelFormat::eR32A32_SFLOAT;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
			return castor::PixelFormat::eR5G5B5A1_UNORM;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
			return castor::PixelFormat::eR5G6B5_UNORM;
		case VK_FORMAT_R8G8B8_UNORM:
			return castor::PixelFormat::eR8G8B8_UNORM;
		case VK_FORMAT_B8G8R8_UNORM:
			return castor::PixelFormat::eB8G8R8_UNORM;
		case VK_FORMAT_R8G8B8_SRGB:
			return castor::PixelFormat::eR8G8B8_SRGB;
		case VK_FORMAT_B8G8R8_SRGB:
			return castor::PixelFormat::eB8G8R8_SRGB;
		case VK_FORMAT_R8G8B8A8_UNORM:
			return castor::PixelFormat::eR8G8B8A8_UNORM;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
			return castor::PixelFormat::eA8B8G8R8_UNORM;
		case VK_FORMAT_R8G8B8A8_SRGB:
			return castor::PixelFormat::eR8G8B8A8_SRGB;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
			return castor::PixelFormat::eA8B8G8R8_SRGB;
		case VK_FORMAT_R16G16B16_SFLOAT:
			return castor::PixelFormat::eR16G16B16_SFLOAT;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			return castor::PixelFormat::eR16G16B16A16_SFLOAT;
		case VK_FORMAT_R32G32B32_SFLOAT:
			return castor::PixelFormat::eR32G32B32_SFLOAT;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return castor::PixelFormat::eR32G32B32A32_SFLOAT;
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
			return castor::PixelFormat::eBC1_RGB_UNORM_BLOCK;
		case VK_FORMAT_BC2_UNORM_BLOCK:
			return castor::PixelFormat::eBC3_UNORM_BLOCK;
		case VK_FORMAT_BC3_UNORM_BLOCK:
			return castor::PixelFormat::eBC5_UNORM_BLOCK;
		case VK_FORMAT_D16_UNORM:
			return castor::PixelFormat::eD16_UNORM;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			return castor::PixelFormat::eD24_UNORM_S8_UINT;
		case VK_FORMAT_X8_D24_UNORM_PACK32:
			return castor::PixelFormat::eD32_UNORM;
		case VK_FORMAT_D32_SFLOAT:
			return castor::PixelFormat::eD32_SFLOAT;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return castor::PixelFormat::eD32_SFLOAT_S8_UINT;
		case VK_FORMAT_S8_UINT:
			return castor::PixelFormat::eS8_UINT;
		default:
			assert( false && "Unsupported VkFormat" );
			return castor::PixelFormat::eR8G8B8A8_UNORM;
		}
	}

	castor::Matrix4x4f convert( std::array< float, 16 > const & value )
	{
		castor::Matrix4x4f result;
		std::memcpy( result.ptr(), value.data(), 16 * sizeof( float ) );
		return result;
	}

	VkClearColorValue convert( castor::RgbaColour const & value )
	{
		VkClearColorValue result;
		std::memcpy( result.float32, value.constPtr(), 4 * sizeof( float ) );
		return result;
	}

	castor::RgbaColour convert( VkClearColorValue const & value )
	{
		castor::RgbaColour result;
		std::memcpy( result.ptr(), value.float32, 4 * sizeof( float ) );
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

	RenderDevice const & getCurrentRenderDevice( RenderDevice const & obj )
	{
		return getCurrentRenderDevice( obj.renderSystem );
	}

	RenderDevice const & getCurrentRenderDevice( RenderSystem const & obj )
	{
		return *obj.getCurrentRenderDevice();
	}

	RenderDevice const & getCurrentRenderDevice( Engine const & obj )
	{
		return getCurrentRenderDevice( *obj.getRenderSystem() );
	}

	RenderDevice const & getCurrentRenderDevice( Scene const & obj )
	{
		return getCurrentRenderDevice( *obj.getEngine() );
	}

	bool operator<( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.alphaFunc < rhs.alphaFunc
			|| ( lhs.alphaFunc == rhs.alphaFunc
				&& ( lhs.topology < rhs.topology
					|| ( lhs.topology == rhs.topology
						&& ( lhs.colourBlendMode < rhs.colourBlendMode
							|| ( lhs.colourBlendMode == rhs.colourBlendMode
								&& ( lhs.alphaBlendMode < rhs.alphaBlendMode
									|| ( lhs.alphaBlendMode == rhs.alphaBlendMode
										&& ( lhs.textures < rhs.textures
											|| ( lhs.textures == rhs.textures
												&& ( lhs.texturesCount < rhs.texturesCount
													|| ( lhs.texturesCount == rhs.texturesCount
														&& ( lhs.heightMapIndex < rhs.heightMapIndex
															|| ( lhs.heightMapIndex == rhs.heightMapIndex
																&& ( lhs.programFlags < rhs.programFlags
																	|| ( lhs.programFlags == rhs.programFlags
																		&& ( lhs.passFlags < rhs.passFlags
																			|| ( lhs.passFlags == rhs.passFlags
																				&& lhs.sceneFlags < rhs.sceneFlags )
																			)
																		)
																	)
																)
															)
														)
													)
												)
											)
										)
									)
								)
							)
						)
					)
				);
	}
}
