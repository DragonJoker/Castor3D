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
			, sdw::UIntField< "hasRefraction" >
			, sdw::UIntField< "hasReflection" >
			, sdw::FloatField< "bwAccumulationOperator" >
			, sdw::UVec4ArrayField< "textures", 2u >
			, sdw::UIntField< "texturesCount" >
			, sdw::UIntField< "passTypeIndex" >
			, sdw::UIntField< "lighting" >
			, sdw::UIntField< "passCount" > >
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

		C3D_API void getPassMultipliers( SubmeshFlags submeshFlags
			, sdw::UVec4 const & passMasks
			, sdw::Array< sdw::Vec4 > & passMultipliers )const;
		C3D_API sdw::UInt getTexture( uint32_t index )const;
		C3D_API sdw::UInt getTexture( sdw::UInt const & index )const;

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
		auto textures()const { return getMember< "textures" >(); }
		auto texturesCount()const { return getMember< "texturesCount" >(); }
		auto passTypeIndex()const { return getMember< "passTypeIndex" >(); }
		auto lighting()const { return getMember< "lighting" >(); }
		auto passCount()const { return getMember< "passCount" >(); }
	};

	template< typename T >
	struct BlendComponentT
	{
		BlendComponentT( T v, bool e = true )
			: value{ std::move( v ) }
			, enabled{ e }
		{
		}

		T value;
		bool enabled;
	};

	struct OpaResult
	{
		sdw::Float opa;
	};

	struct OpacityBlendComponents
	{
		C3D_API OpacityBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, BlendComponentT< sdw::Vec3 > t0
			, BlendComponentT< sdw::Float > opa );
		C3D_API OpacityBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, OpacityBlendComponents const & rhs );

		C3D_API OpaResult createResult( sdw::ShaderWriter & writer
			, OpacityBlendComponents const & rhs )const;
		C3D_API void apply( sdw::Float const & passMultiplier
			, OpaResult & res )const;
		C3D_API void set( OpaResult const & rhs );

		sdw::Vec3 & texCoord0() { return m_texCoord0; }
		sdw::Float & opacity() { return m_opacity; }

		sdw::Vec3 const & texCoord0()const { return m_texCoord0; }
		sdw::Float const & opacity()const { return m_opacity; }

	private:
		sdw::Vec3 m_texCoord0;
		sdw::Float m_opacity;
	};

	struct GeoResult
	{
		OpaResult opa;
		sdw::Vec3 nml;
		sdw::Vec3 tan;
		sdw::Vec3 bit;
		sdw::Vec3 tvp;
		sdw::Vec3 tfp;
	};

	struct GeometryBlendComponents
		: public OpacityBlendComponents
	{
		C3D_API GeometryBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, BlendComponentT< sdw::Vec3 > t0, BlendComponentT< sdw::Vec3 > t1, BlendComponentT< sdw::Vec3 > t2, BlendComponentT< sdw::Vec3 > t3
			, BlendComponentT< sdw::Float > opa
			, BlendComponentT< sdw::Vec3 > nml, BlendComponentT< sdw::Vec3 > tan, BlendComponentT< sdw::Vec3 > bit
			, BlendComponentT< sdw::Vec3 > tvp, BlendComponentT< sdw::Vec3 > tfp );
		C3D_API GeometryBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, GeometryBlendComponents const & rhs );

		C3D_API GeoResult createResult( sdw::ShaderWriter & writer
			, GeometryBlendComponents const & rhs )const;
		C3D_API void apply( sdw::Float const & passMultiplier
			, GeoResult & res )const;
		C3D_API void set( GeoResult const & rhs );

		sdw::Vec3 & texCoord1() { return m_texCoord1; }
		sdw::Vec3 & texCoord2() { return m_texCoord2; }
		sdw::Vec3 & texCoord3() { return m_texCoord3; }
		sdw::Vec3 & normal() { return m_normal; }
		sdw::Vec3 & tangent() { return m_tangent; }
		sdw::Vec3 & bitangent() { return m_bitangent; }
		sdw::Vec3 & tangentSpaceViewPosition() { return m_tangentSpaceViewPosition; }
		sdw::Vec3 & tangentSpaceFragPosition() { return m_tangentSpaceFragPosition; }

		sdw::Vec3 const & texCoord1()const { return m_texCoord1; }
		sdw::Vec3 const & texCoord2()const { return m_texCoord2; }
		sdw::Vec3 const & texCoord3()const { return m_texCoord3; }
		sdw::Vec3 const & normal()const { return m_normal; }
		sdw::Vec3 const & tangent()const { return m_tangent; }
		sdw::Vec3 const & bitangent()const { return m_bitangent; }
		sdw::Vec3 const & tangentSpaceViewPosition()const { return m_tangentSpaceViewPosition; }
		sdw::Vec3 const & tangentSpaceFragPosition()const { return m_tangentSpaceFragPosition; }

	private:
		sdw::Vec3 m_texCoord1;
		sdw::Vec3 m_texCoord2;
		sdw::Vec3 m_texCoord3;
		sdw::Vec3 m_normal;
		sdw::Vec3 m_tangent;
		sdw::Vec3 m_bitangent;
		sdw::Vec3 m_tangentSpaceViewPosition;
		sdw::Vec3 m_tangentSpaceFragPosition;
	};

	struct OpqResult
	{
		GeoResult geo;
		sdw::Float occ;
		sdw::Float trn;
		sdw::Vec3 ems;
	};

	struct OpaqueBlendComponents
		: public GeometryBlendComponents
	{
		C3D_API OpaqueBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, BlendComponentT< sdw::Vec3 > t0, BlendComponentT< sdw::Vec3 > t1, BlendComponentT< sdw::Vec3 > t2, BlendComponentT< sdw::Vec3 > t3
			, BlendComponentT< sdw::Float > opa
			, BlendComponentT< sdw::Vec3 > nml, BlendComponentT< sdw::Vec3 > tan, BlendComponentT< sdw::Vec3 > bit
			, BlendComponentT< sdw::Vec3 > tvp, BlendComponentT< sdw::Vec3 > tfp
			, BlendComponentT< sdw::Float > occ, BlendComponentT< sdw::Float > trn, BlendComponentT< sdw::Vec3 > ems );
		C3D_API OpaqueBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, OpaqueBlendComponents const & rhs );

		C3D_API OpqResult createResult( sdw::ShaderWriter & writer
			, OpaqueBlendComponents const & rhs )const;
		C3D_API void apply( sdw::Float const & passMultiplier
			, OpqResult & res )const;
		C3D_API void set( OpqResult const & rhs );

		sdw::Float & occlusion() { return m_occlusion; }
		sdw::Float & transmittance() { return m_transmittance; }
		sdw::Vec3 & emissive() { return m_emissive; }

		sdw::Float const & occlusion()const { return m_occlusion; }
		sdw::Float const & transmittance()const { return m_transmittance; }
		sdw::Vec3 const & emissive()const { return m_emissive; }

	private:
		sdw::Float m_occlusion;
		sdw::Float m_transmittance;
		sdw::Vec3 m_emissive;
	};

	struct LgtResult
	{
		OpqResult opq;
		sdw::Vec3 trs;
		sdw::Float rfr;
		sdw::UInt hrr;
		sdw::UInt hrl;
		sdw::Float acc;
	};

	struct LightingBlendComponents
		: public OpaqueBlendComponents
	{
		C3D_API LightingBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, BlendComponentT< sdw::Vec3 > t0, BlendComponentT< sdw::Vec3 > t1, BlendComponentT< sdw::Vec3 > t2, BlendComponentT< sdw::Vec3 > t3
			, BlendComponentT< sdw::Float > opa
			, BlendComponentT< sdw::Vec3 > nml, BlendComponentT< sdw::Vec3 > tan, BlendComponentT< sdw::Vec3 > bit
			, BlendComponentT< sdw::Vec3 > tvp, BlendComponentT< sdw::Vec3 > tfp
			, BlendComponentT< sdw::Float > occ, BlendComponentT< sdw::Float > trn, BlendComponentT< sdw::Vec3 > ems
			, BlendComponentT< sdw::Vec3 > trs
			, BlendComponentT< sdw::Float > rfr, BlendComponentT< sdw::UInt > hrr, BlendComponentT< sdw::UInt > hrl
			, BlendComponentT< sdw::Float > acc );
		C3D_API LightingBlendComponents( sdw::ShaderWriter & writer
			, std::string const & prefix
			, LightingBlendComponents const & rhs );

		C3D_API LgtResult createResult( sdw::ShaderWriter & writer
			, LightingBlendComponents const & rhs )const;
		C3D_API void apply( sdw::Float const & passMultiplier
			, LgtResult & res )const;
		C3D_API void set( LgtResult const & rhs );

		sdw::Vec3 & transmission() { return m_transmission; }
		sdw::Float & refractionRatio() { return m_refractionRatio; }
		sdw::UInt & hasRefraction() { return m_hasRefraction; }
		sdw::UInt & hasReflection() { return m_hasReflection; }
		sdw::Float & bwAccumulationOperator() { return m_bwAccumulationOperator; }

		sdw::Vec3 const & transmission()const { return m_transmission; }
		sdw::Float const & refractionRatio()const { return m_refractionRatio; }
		sdw::UInt const & hasRefraction()const { return m_hasRefraction; }
		sdw::UInt const & hasReflection()const { return m_hasReflection; }
		sdw::Float const & bwAccumulationOperator()const { return m_bwAccumulationOperator; }

	private:
		sdw::Vec3 m_transmission;
		sdw::Float m_refractionRatio;
		sdw::UInt m_hasRefraction;
		sdw::UInt m_hasReflection;
		sdw::Float m_bwAccumulationOperator;
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
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output )const;
		// Used by forward passes
		C3D_API std::pair< Material, std::unique_ptr< LightMaterial > > blendMaterials( Utils & utils
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
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, LightingBlendComponents & output )const;
		C3D_API std::pair< Material, std::unique_ptr< LightMaterial > > blendMaterials( Utils & utils
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
			, sdw::Array< sdw::Vec4 > const & passMultipliers
			, sdw::Vec3 const & vertexColour
			, LightingBlendComponents & output )const;

		C3D_API Material applyMaterial( std::string const & matName
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, OpacityBlendComponents & output
			, Utils & utils )const;
		C3D_API Material applyMaterial( std::string const & matName
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, GeometryBlendComponents & output
			, Utils & utils )const;
		C3D_API std::pair< Material, std::unique_ptr< LightMaterial > > applyMaterial( std::string const & matName
			, std::string const & lgtMatName
			, PassFlags const & passFlags
			, TextureFlags const & textures
			, bool hasTextures
			, shader::TextureConfigurations const & textureConfigs
			, shader::TextureAnimations const & textureAnims
			, shader::LightingModel & lightingModel
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UInt const & materialId
			, sdw::Vec3 const & vertexColour
			, OpaqueBlendComponents & output
			, Utils & utils
			, bool needsRsm )const;
		C3D_API std::pair< Material, std::unique_ptr< LightMaterial > > applyMaterial( std::string const & matName
			, std::string const & lgtMatName
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
		C3D_API std::pair< Material, std::unique_ptr< LightMaterial > > applyMaterial( std::string const & matName
			, std::string const & lgtMatName
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
