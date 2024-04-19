/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMaterial_H___
#define ___C3D_GlslMaterial_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Shader/Shaders/GlslBuffer.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	struct Material
		: public sdw::StructInstance
	{
		friend class Materials;

		C3D_API Material( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, Material );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, PassShaders const & passShaders
			, sdw::expr::ExprList & inits );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, PassShaders const & passShaders )
		{
			sdw::expr::ExprList inits;
			return makeType( cache, passShaders, inits );
		}
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		writer			The writer.
		 *\param		alphaFunc		The alpha function.
		 *\param[in]	opacity			The opacity value.
		 *\param[in]	alphaRef		The alpha comparison reference value.
		 *\param[in]	opaque			\p true for opaque nodes, \p false for transparent ones.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		writer			Le writer.
		 *\param		alphaFunc		La fonction d'opacité.
		 *\param[in]	opacity			La valeur d'opacité.
		 *\param[in]	alphaRef		La valeur de référence pour la comparaison alpha.
		 *\param[in]	opaque			\p true pour les noeuds opaques, \p false pour les transparents.
		 */
		C3D_API static void applyAlphaFunc( sdw::ShaderWriter & writer
			, VkCompareOp alphaFunc
			, sdw::Float & opacity
			, sdw::Float const & alphaRef
			, bool opaque = true );

		C3D_API void getPassMultipliers( PipelineFlags const & flags
			, sdw::UVec4 const & passMasks
			, sdw::Array< sdw::Vec4 > const & passMultipliers )const;
		C3D_API sdw::UInt getTexture( uint32_t index )const;
		C3D_API sdw::UInt getTexture( sdw::UInt const & index )const;

	public:
		sdw::UInt passId;
		sdw::UInt index;
		sdw::UInt passCount;
		sdw::UInt lighting;
		sdw::UInt lightingModel;
		sdw::Vec3 colour;
		sdw::Vec3 specular;
		sdw::Float transmission;
		sdw::UInt hasTransmission;
		sdw::Float opacity;
		sdw::Float alphaRef;
		sdw::UInt hasReflection;
		sdw::Float refractionRatio;
		sdw::Vec3 attenuationColour;
		sdw::Float attenuationDistance;
		sdw::Float thicknessFactor;

	private:
		static sdw::expr::ExprPtr makeInit( sdw::ShaderWriter const & writer
			, PassShaders & passShaders );
	};

	class Materials
		: public BufferT< Material >
	{
	public:
		C3D_API Materials( sdw::ShaderWriter & writer
			, PassShaders & passShaders );
		C3D_API Materials( sdw::ShaderWriter & writer
			, PassShaders & passShaders
			, uint32_t binding
			, uint32_t set
			, bool enable = true );
		C3D_API Materials( Engine const & engine
			, sdw::ShaderWriter & writer
			, PassShaders & passShaders
			, uint32_t binding
			, uint32_t set
			, uint32_t & specifics
			, bool enable = true );

		C3D_API ComponentModeFlags getFilter()const;

		template< typename TypeT >
		bool hasSpecificsBuffer()const
		{
			return m_buffers.end() != m_buffers.find( TypeT::getName() );
		}

		template< typename TypeT >
		BufferT< TypeT > const & getSpecificsBuffer()const
		{
			auto it = m_buffers.find( TypeT::getName() );

			if ( it == m_buffers.end() )
			{
				CU_Exception( "Buffer not declared." );
			}

			return static_cast< BufferT< TypeT > const & >( *it->second );
		}

		sdw::UInt getPassTypesCount()const
		{
			return this->getSecondCount();
		}

		Material getMaterial( sdw::UInt const & index )const
		{
			return this->getData( index - 1_u, m_passShaders );
		}

		auto & getPassShaders()const
		{
			return m_passShaders;
		}

		// Used by:
		// - picking pass (opacity only)
		// - depth pass (opacity and tangent space only)
		// - shadow passes
		C3D_API void blendMaterials( PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material & material
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, BlendComponents & output )const;
		// Used by:
		// - visibility resolve pass
		// - opaque pass
		C3D_API void blendMaterials( VkCompareOp alphaFunc
			, PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material & material
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, BlendComponents & output )const;
		C3D_API void blendMaterials( DebugOutput & debugOutput
			, VkCompareOp alphaFunc
			, PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material & material
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, BlendComponents & output )const;
		// Used by forward passes
		C3D_API void blendMaterials( bool opaque
			, PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material & material
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, BlendComponents & output )const;
		C3D_API void blendMaterials( DebugOutput & debugOutput
			, bool opaque
			, PipelineFlags const & flags
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material & material
			, sdw::UInt const & materialId
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, BlendComponents & output )const;

		C3D_API void applyMaterialMaps( PipelineFlags const & flags
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, Material const & material
			, BlendComponents & components )const;

	private:
		void doDisplayDebug( DebugOutput & debugOutput
			, BlendComponents const & components )const;

	private:
		PassShaders & m_passShaders;
		castor::StringMap< BufferBaseUPtr > m_buffers;
	};
}

#endif
