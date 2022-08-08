/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMaterials_H___
#define ___C3D_GlslMaterials_H___

#include "GlslBuffer.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	castor::String const PassBufferName = cuT( "Materials" );

	struct Material
		: public sdw::StructInstanceHelperT< "C3D_Material"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec4Field< "colourDiv" >
			, sdw::Vec4Field< "specDiv" >
			, sdw::FloatField< "edgeWidth" >
			, sdw::FloatField< "depthFactor" >
			, sdw::FloatField< "normalFactor" >
			, sdw::FloatField< "objectFactor" >
			, sdw::Vec4Field< "edgeColour" >
			, sdw::Vec4Field< "specific" >
			, sdw::UIntField< "index" >
			, sdw::FloatField< "emissive" >
			, sdw::FloatField< "alphaRef" >
			, sdw::UIntField< "sssProfileIndex" >
			, sdw::Vec3Field< "transmission" >
			, sdw::FloatField< "opacity" >
			, sdw::FloatField< "refractionRatio" >
			, sdw::IntField< "hasRefraction" >
			, sdw::IntField< "hasReflection" >
			, sdw::FloatField< "bwAccumulationOperator" >
			, sdw::UVec4Field< "textures0" >
			, sdw::UVec4Field< "textures1" >
			, sdw::IntField< "textures" >
			, sdw::UIntField< "passTypeIndex" > >
	{
		friend class Materials;

		C3D_API Material( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API sdw::Vec3 colour()const;
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	opacity		The alpha TypeEnum.
		 *\param[in]	passMasks	The subpasses masks.
		 *\param[in]	opaque		\p true for opaque nodes, \p false for transparent ones.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	opacity		La valeur d'opacité.
		 *\param[in]	passMasks	Les masques de subpasses.
		 *\param[in]	opaque		\p true pour les noeuds opaques, \p false pour les transparents.
		 */
		C3D_API void applyAlphaFunc( VkCompareOp alphaFunc
			, sdw::Float & opacity
			, sdw::Float const & passMultiplier
			, bool opaque = true );
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	opacity		The opacity value.
		 *\param[in]	alphaRef	The alpha comparison reference value.
		 *\param[in]	passMasks	The subpasses masks.
		 *\param[in]	opaque		\p true for opaque nodes, \p false for transparent ones.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	opacity		La valeur d'opacité.
		 *\param[in]	alphaRef	La valeur de référence pour la comparaison alpha.
		 *\param[in]	passMasks	Les masques de subpasses.
		 *\param[in]	opaque		\p true pour les noeuds opaques, \p false pour les transparents.
		 */
		C3D_API void applyAlphaFunc( VkCompareOp alphaFunc
			, sdw::Float & opacity
			, sdw::Float const & alphaRef
			, sdw::Float const & passMultiplier
			, bool opaque = true );
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	opacity		The opacity value.
		 *\param[in]	alphaRef	The alpha comparison reference value.
		 *\param[in]	passMasks	The subpasses masks.
		 *\param[in]	opaque		\p true for opaque nodes, \p false for transparent ones.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	opacity		La valeur d'opacité.
		 *\param[in]	alphaRef	La valeur de référence pour la comparaison alpha.
		 *\param[in]	passMasks	Les masques de subpasses.
		 *\param[in]	opaque		\p true pour les noeuds opaques, \p false pour les transparents.
		 */
		C3D_API static void applyAlphaFunc( sdw::ShaderWriter & writer
			, VkCompareOp alphaFunc
			, sdw::Float & opacity
			, sdw::Float const & alphaRef
			, sdw::Float const & passMultiplier
			, bool opaque = true );
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	opacity		The alpha TypeEnum.
		 *\param[in]	passMasks	The subpasses masks.
		 *\param[in]	opaque		\p true for opaque nodes, \p false for transparent ones.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	opacity		La valeur d'opacité.
		 *\param[in]	passMasks	Les masques de subpasses.
		 *\param[in]	opaque		\p true pour les noeuds opaques, \p false pour les transparents.
		 */
		C3D_API void getPassMultipliers( SubmeshFlags submeshFlags
			, sdw::UInt const & passCount
			, sdw::UVec4 const & passMasks
			, sdw::Array< sdw::Vec4 > & passMultipliers )const;
		C3D_API sdw::UInt getTexture( uint32_t index )const;

	public:
		auto colourDiv()const { return getMember< "colourDiv" >(); }
		auto specDiv()const { return getMember< "specDiv" >(); }
		auto edgeWidth()const { return getMember< "edgeWidth" >(); }
		auto depthFactor()const { return getMember< "depthFactor" >(); }
		auto normalFactor()const { return getMember< "normalFactor" >(); }
		auto objectFactor()const { return getMember< "objectFactor" >(); }
		auto edgeColour()const { return getMember< "edgeColour" >(); }
		auto specific()const { return getMember< "specific" >(); }
		auto index()const { return getMember< "index" >(); }
		auto emissive()const { return getMember< "emissive" >(); }
		auto alphaRef()const { return getMember< "alphaRef" >(); }
		auto sssProfileIndex()const { return getMember< "sssProfileIndex" >(); }
		auto transmission()const { return getMember< "transmission" >(); }
		auto opacity()const { return getMember< "opacity" >(); }
		auto refractionRatio()const { return getMember< "refractionRatio" >(); }
		auto hasRefraction()const { return getMember< "hasRefraction" >(); }
		auto hasReflection()const { return getMember< "hasReflection" >(); }
		auto bwAccumulationOperator()const { return getMember< "bwAccumulationOperator" >(); }
		auto textures0()const { return getMember< "textures0" >(); }
		auto textures1()const { return getMember< "textures1" >(); }
		auto textures()const { return getMember< "textures" >(); }
		auto passTypeIndex()const { return getMember< "passTypeIndex" >(); }
	};

	struct OpacityBlendComponents
	{
		sdw::Vec3 texCoord0;
		sdw::Float opacity;
	};

	struct GeometryBlendComponents
	{
		sdw::Vec3 texCoord0;
		sdw::Vec3 texCoord1;
		sdw::Vec3 texCoord2;
		sdw::Vec3 texCoord3;
		sdw::Float opacity;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 bitangent;
		sdw::Vec3 tangentSpaceViewPosition;
		sdw::Vec3 tangentSpaceFragPosition;
	};

	struct OpaqueBlendComponents
	{
		sdw::Vec3 texCoord0;
		sdw::Vec3 texCoord1;
		sdw::Vec3 texCoord2;
		sdw::Vec3 texCoord3;
		sdw::Float opacity;
		sdw::Float occlusion;
		sdw::Float transmittance;
		sdw::Vec3 emissive;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 bitangent;
		sdw::Vec3 tangentSpaceViewPosition;
		sdw::Vec3 tangentSpaceFragPosition;
	};

	struct LightingBlendComponents
	{
		sdw::Vec3 texCoord0;
		sdw::Vec3 texCoord1;
		sdw::Vec3 texCoord2;
		sdw::Vec3 texCoord3;
		sdw::Float opacity;
		sdw::Float occlusion;
		sdw::Float transmittance;
		sdw::Vec3 transmission;
		sdw::Vec3 emissive;
		sdw::Float refractionRatio;
		sdw::Int hasRefraction;
		sdw::Int hasReflection;
		sdw::Float bwAccumulationOperator;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 bitangent;
		sdw::Vec3 tangentSpaceViewPosition;
		sdw::Vec3 tangentSpaceFragPosition;
	};

	CU_DeclareSmartPtr( Material );

	class Materials
		: public BufferT< Material >
	{
	public:
		C3D_API explicit Materials( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		sdw::UInt getPassTypesCount()const
		{
			return this->getSecondaryCount();
		}

		Material getMaterial( sdw::UInt const & index )const
		{
			return this->getData( index );
		}

		// Used by picking pass (opacity only)
		C3D_API void blendMaterials( Utils & utils
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, OpacityBlendComponents & output )const;
		C3D_API void blendMaterials( Utils & utils
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlagsArray const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, OpacityBlendComponents & output )const;
		// Used by depth pass (opacity and tangent space only)
		C3D_API void blendMaterials( Utils & utils
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, GeometryBlendComponents & output )const;
		C3D_API void blendMaterials( Utils & utils
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlagsArray const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, GeometryBlendComponents & output )const;
		// Used by shadow passes
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, bool needsRsm
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, bool needsRsm
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlagsArray const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		// Used by visibility resolve pass
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		// Used by opaque pass
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlagsArray const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		// Used by forward passes
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, bool opaque
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, LightingBlendComponents & output )const;
		C3D_API std::unique_ptr< LightMaterial > blendMaterials( Utils & utils
			, bool opaque
			, VkCompareOp alphaFunc
			, PassFlags const & passFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlagsArray const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::UInt const & passCount
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, LightingBlendComponents & output )const;

	private:
		Material applyMaterial( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, OpacityBlendComponents & output )const;
		Material applyMaterial( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, GeometryBlendComponents & output )const;
		std::pair< Material, std::unique_ptr< LightMaterial > > applyMaterial( Utils & utils
			, bool needsRsm
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		std::pair< Material, std::unique_ptr< LightMaterial > > applyMaterial( PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		std::pair< Material, std::unique_ptr< LightMaterial > > applyMaterial( Utils & utils
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::Vec3 const & vertexColour
			, LightingBlendComponents & output )const;
	};

	struct SssProfile
		: public sdw::StructInstance
	{
		friend class SssProfiles;
		C3D_API SssProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, SssProfile );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		sdw::Int transmittanceProfileSize;
		sdw::Float gaussianWidth;
		sdw::Float subsurfaceScatteringStrength;
		sdw::Float pad;
		sdw::Array< sdw::Vec4 > transmittanceProfile;
	};

	CU_DeclareSmartPtr( SssProfile );

	class SssProfiles
		: public BufferT< SssProfile >
	{
	public:
		C3D_API explicit SssProfiles( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		SssProfile getProfile( sdw::UInt const & index )const
		{
			return BufferT< SssProfile >::getData( index );
		}
	};
}

#endif
