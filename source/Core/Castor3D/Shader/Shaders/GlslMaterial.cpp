#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

CU_ImplementSmartPtr( castor3d::shader, Material )

namespace castor3d::shader
{
	//*********************************************************************************************

	namespace mtl
	{
		static void blendMaterials( Materials const & materials
			, bool opaque
			, VkCompareOp alphaFunc
			, PipelineFlags const & flags
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material & material
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, BlendComponents & output )
		{
			auto & writer = findWriterMandat( textureConfigs, textureAnims, maps, material, output );
			auto & passShaders = materials.getPassShaders();
			auto opacity = output.getMember< sdw::Float >( "opacity", true );

			if ( flags.enablePassMasks() )
			{
				auto result = writer.declLocale( "result"
					, BlendComponents{ materials, true } );
				auto passMultiplier = writer.declLocale( "passMultiplier"
					, passMultipliers[0_u][0_u] );

				IF( writer, passMultiplier != 0.0_f )
				{
					auto passComponents = writer.declLocale( "passComponents"
						, output );
					materials.applyMaterialMaps( flags
						, textureConfigs, textureAnims, maps
						, material
						, passComponents );
					passShaders.blendComponents( materials, passMultiplier, result, passComponents );
				}
				FI;

				FOR( writer, sdw::UInt, passIdx, 1_u, passIdx < material.passCount && passIdx < MaxPassLayers, ++passIdx )
				{
					passMultiplier = passMultipliers[passIdx / 4_u][passIdx % 4_u];

					IF( writer, passMultiplier != 0.0_f )
					{
						auto curMaterial = writer.declLocale( "passMaterial"
							, materials.getMaterial( materialId + passIdx ) );
						auto passComponents = writer.declLocale( "passComponents"
							, output );
						materials.applyMaterialMaps( flags
							, textureConfigs, textureAnims, maps
							, curMaterial
							, passComponents );
						material.lighting += curMaterial.lighting;
						passShaders.blendComponents( materials, passMultiplier, result, passComponents );
					}
					FI;
				}
				ROF;

				output = result;
				output.normal = normalize( result.normal );
			}
			else
			{
				materials.applyMaterialMaps( flags
					, textureConfigs, textureAnims, maps
					, material
					, output );
			}

			if ( opaque
				|| !output.hasMember( "transmission" ) )
			{
				Material::applyAlphaFunc( writer
					, alphaFunc
					, opacity
					, material.alphaRef
					, opaque );
			}
		}
	}

	//*********************************************************************************************

	Material::Material( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, passId{ getMember< sdw::UInt >( "passId" ) }
		, index{ getMember < sdw::UInt >( "index" ) }
		, passCount{ getMember < sdw::UInt >( "passCount" ) }
		, lighting{ getMember < sdw::UInt >( "lighting" ) }
		, lightingModel{ getMember < sdw::UInt >( "lightingModel" ) }
		, textures{ getMember < sdw::Array< sdw::UInt > >( "textures" ) }
		, texturesCount{ getMember < sdw::UInt >( "textureCount" ) }
		, colour{ getMember< sdw::Vec3 >( "colour" ) }
		, specular{ getMember< sdw::Vec3 >( "specular" ) }
		, transmission{ getMember< sdw::Float >( "transmission" ) }
		, hasTransmission{ getMember< sdw::UInt >( "hasTransmission" ) }
		, opacity{ getMember< sdw::Float >( "opacity" ) }
		, alphaRef{ getMember< sdw::Float >( "alphaRef" ) }
		, hasReflection{ getMember< sdw::UInt >( "hasReflection" ) }
		, refractionRatio{ getMember< sdw::Float >( "refractionRatio" ) }
		, attenuationColour{ getMember< sdw::Vec3 >( "attenuationColour" ) }
		, attenuationDistance{ getMember< sdw::Float >( "attenuationDistance" ) }
		, thicknessFactor{ getMember< sdw::Float >( "thicknessFactor" ) }
	{
	}

	ast::type::BaseStructPtr Material::makeType( ast::type::TypesCache & cache
		, PassShaders & passShaders
		, sdw::expr::ExprList & inits )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430, "C3D_Material" );
		passShaders.fillMaterial( *result, inits );
		return result;
	}

	void Material::getPassMultipliers( PipelineFlags const & flags
		, sdw::UVec4 const & passMasks
		, sdw::Array< sdw::Vec4 > & passMultipliers )const
	{
		if ( passMultipliers.isEnabled() )
		{
			if ( flags.enablePassMasks()
				&& passMasks.isEnabled() )
			{
				FOR( *m_writer, sdw::UInt, passIdx, 0_u, passIdx < passCount && passIdx < MaxPassLayers, ++passIdx )
				{
					auto mask32 = passMasks[passIdx / 4_u];
					auto mask8 = ( mask32 >> ( ( passIdx % 4_u ) * 8_u ) ) & 0xFF_u;
					passMultipliers[passIdx / 4_u][passIdx % 4_u] = m_writer->cast< sdw::Float >( mask8 ) / 255.0_f;
				}
				ROF;
			}
		}
	}

	void Material::applyAlphaFunc( sdw::ShaderWriter & writer
		, VkCompareOp alphaFunc
		, sdw::Float & alpha
		, sdw::Float const & ref
		, bool opaque )
	{
		if ( alphaFunc != VK_COMPARE_OP_ALWAYS
			&& alpha.isEnabled()
			&& ref.isEnabled() )
		{
			switch ( alphaFunc )
			{
			case VK_COMPARE_OP_NEVER:
				writer.demote();
				break;
			case VK_COMPARE_OP_LESS:
				IF( writer, alpha >= ref )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_EQUAL:
				IF( writer, alpha != ref )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_LESS_OR_EQUAL:
				IF( writer, alpha > ref )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_GREATER:
				IF( writer, alpha <= ref )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_NOT_EQUAL:
				IF( writer, alpha == ref )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_GREATER_OR_EQUAL:
				IF( writer, alpha < ref )
				{
					writer.demote();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_ALWAYS:
				break;

			default:
				break;
			}
		}
	}

	sdw::UInt Material::getTexture( uint32_t idx )const
	{
		return textures[idx % MaxPassTextures];
	}

	sdw::UInt Material::getTexture( sdw::UInt const & idx )const
	{
		return textures[idx % MaxPassTextures];
	}

	sdw::expr::ExprPtr Material::makeInit( sdw::ShaderWriter & writer
		, PassShaders & passShaders )
	{
		sdw::expr::ExprList initializers;
		auto type = Material::makeType( writer.getTypesCache(), passShaders, initializers );
		return sdw::makeAggrInit( type, std::move( initializers ) );
	}

	//*********************************************************************************************

	Materials::Materials( sdw::ShaderWriter & writer
		, PassShaders & passShaders )
		: BufferT< Material >{ writer
		, "C3D_Materials"
		, "c3d_materials"
		, 0u
		, 0u
		, false
		, passShaders }
		, m_passShaders{ passShaders }
	{
	}

	Materials::Materials( sdw::ShaderWriter & writer
		, PassShaders & passShaders
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< Material >{ writer
		, "C3D_Materials"
		, "c3d_materials"
		, binding
		, set
		, enable
		, passShaders }
		, m_passShaders{ passShaders }
	{
	}

	Materials::Materials( Engine const & engine
		, sdw::ShaderWriter & writer
		, PassShaders & passShaders
		, uint32_t binding
		, uint32_t set
		, uint32_t & specifics
		, bool enable )
		: Materials{ writer
		, passShaders
		, binding
		, set
		, enable }
	{
		engine.declareSpecificsShaderBuffers( writer, m_buffers, specifics, set );
	}

	ComponentModeFlags Materials::getFilter()const
	{
		return m_passShaders.getFilter();
	}

	void Materials::blendMaterials( PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material & material
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, BlendComponents & output )const
	{
		mtl::blendMaterials( *this, true, flags.alphaFunc, flags
			, textureConfigs, textureAnims, maps
			, material, materialId, passMultipliers
			, output );
	}

	void Materials::blendMaterials( VkCompareOp alphaFunc
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material & material
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, BlendComponents & output )const
	{
		mtl::blendMaterials( *this, true, alphaFunc, flags
			, textureConfigs, textureAnims, maps
			, material, materialId, passMultipliers
			, output );
	}

	void Materials::blendMaterials( DebugOutput & debugOutput
		, VkCompareOp alphaFunc
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material & material
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, BlendComponents & output )const
	{
		blendMaterials( alphaFunc, flags
			, textureConfigs, textureAnims, maps
			, material, materialId, passMultipliers
			, output );
		doDisplayDebug( debugOutput, output );
	}

	void Materials::blendMaterials( bool opaque
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material & material
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, BlendComponents & output )const
	{
		mtl::blendMaterials( *this, opaque, flags.alphaFunc, flags
			, textureConfigs, textureAnims, maps
			, material, materialId, passMultipliers
			, output );
	}

	void Materials::blendMaterials( DebugOutput & debugOutput
		, bool opaque
		, PipelineFlags const & flags
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material & material
		, sdw::UInt const & materialId
		, sdw::Array< sdw::Vec4 > const & passMultipliers
		, BlendComponents & output )const
	{
		blendMaterials( opaque, flags
			, textureConfigs, textureAnims, maps
			, material, materialId, passMultipliers
			, output );
		doDisplayDebug( debugOutput, output );
	}

	void Materials::applyMaterialMaps( PipelineFlags const & flags
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, Material const & material
		, BlendComponents & components )const
	{
		textureConfigs.computeMapsContributions( m_passShaders
			, flags
			, textureAnims
			, maps
			, material
			, components );
	}

	void Materials::doDisplayDebug( DebugOutput & debugOutput
		, BlendComponents const & components )const
	{
		debugOutput.registerOutput( cuT( "Material Base Colour" ), components.colour.rgb() );
		debugOutput.registerOutput( cuT( "Material Opacity" ), components.opacity );
		debugOutput.registerOutput( cuT( "Material Metalness" ), components.metalness );
		debugOutput.registerOutput( cuT( "Material Roughness" ), components.roughness );
		debugOutput.registerOutput( cuT( "Material Specular Colour" ), components.specular );
		debugOutput.registerOutput( cuT( "Material Specular Factor" ), components.getMember< sdw::Float >( "specularFactor", 0.0_f ) );
		debugOutput.registerOutput( cuT( "Material Occlusion" ), components.occlusion );
		debugOutput.registerOutput( cuT( "Material Emissive Colour" ), components.emissiveColour );
		debugOutput.registerOutput( cuT( "Material Emissive Factor" ), components.emissiveFactor );
		debugOutput.registerOutput( cuT( "Material Ambient Colour" ), components.ambientColour );
		debugOutput.registerOutput( cuT( "Material Ambient Factor" ), components.ambientFactor );
	}

	//*********************************************************************************************
}
