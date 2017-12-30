/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_SHADER_H___
#define ___C3D_PREREQUISITES_SHADER_H___

namespace castor3d
{
	static uint32_t constexpr PassBufferIndex = 0u;
	static uint32_t constexpr LightBufferIndex = 1u;
	static uint32_t constexpr MinTextureIndex = 2u;

	/**@name Shader */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Enumeration of existing shader models
	\~french
	\brief		Enumération des modèles de shader existants
	\remark
	*/
	enum class ShaderModel
		: uint8_t
	{
		//!\~english	Vertex shaders.
		//!\~french		Vertex shaders.
		eModel1,
		//!\~english	Pixel shaders.
		//!\~french		Pixel shaders.
		eModel2,
		//!\~english	Geometry shaders.
		//!\~french		Geometry shaders.
		eModel3,
		//!\~english	Tessellation shaders.
		//!\~french		Tessellation shaders.
		eModel4,
		//!\~english	Compute shaders.
		//!\~french		Compute shaders.
		eModel5,
		CASTOR_SCOPED_ENUM_BOUNDS( eModel1 )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Shader status enueration
	\~french
	\brief		Enumération des statuts de shader
	*/
	enum class ShaderStatus
		: uint8_t
	{
		edontExist,
		eNotCompiled,
		eError,
		eCompiled,
		CASTOR_SCOPED_ENUM_BOUNDS( edontExist )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Program status enumeration
	\~french
	\brief		Enumération des statuts de programme
	*/
	enum class ProgramStatus
		: uint8_t
	{
		eNotLinked,
		eError,
		eLinked,
		CASTOR_SCOPED_ENUM_BOUNDS( eNotLinked )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types enumeration
	\~french
	\brief		Enumération des types de shader object
	*/
	enum class ShaderType
		: int8_t
	{
		eNone = -1,
		eVertex,
		eHull,
		eDomain,
		eGeometry,
		ePixel,
		eCompute,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types masks
	\~french
	\brief		Masques pour les types de shader object
	*/
	enum class ShaderTypeFlag
		: uint8_t
	{
		eVertex = uint8_t( 0x1 ) << int( ShaderType::eVertex ),
		eHull = uint8_t( 0x1 ) << int( ShaderType::eHull ),
		eDomain = uint8_t( 0x1 ) << int( ShaderType::eDomain ),
		eGeometry = uint8_t( 0x1 ) << int( ShaderType::eGeometry ),
		ePixel = uint8_t( 0x1 ) << int( ShaderType::ePixel ),
		eCompute = uint8_t( 0x1 ) << int( ShaderType::eCompute ),
	};
	IMPLEMENT_FLAGS( ShaderTypeFlag )
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/11/13
	\version	0.7.0.0
	\~english
	\brief		Flags to use when looking for an automatically generated program
	\~french
	\brief		Indicateurs à utiliser lors de la recherche de programme généré automatiquement
	*/
	enum class ProgramFlag
		: uint16_t
	{
		//!\~english	Program using instanciation.
		//!\~french		Programme utilisant l'instanciation.
		eInstantiation = 0x0001,
		//!\~english	Program using skeleton animations.
		//!\~french		Programme utilisant les animations par squelette.
		eSkinning = 0x0002,
		//!\~english	Program used by billboards.
		//!\~french		Programme utilisé par des billboards.
		eBillboards = 0x0004,
		//!\~english	Program using per-vertex animations.
		//!\~french		Programme utilisant les animations par sommet.
		eMorphing = 0x0008,
		//!\~english	Picking pass program.
		//\~french		Programme de passe de picking.
		ePicking = 0x0010,
		//!\~english	Shader supporting lighting.
		//\~french		Shader supportant les éclairages.
		eLighting = 0x0020,
		//!\~english	Shader for spherical billboards.
		//\~french		Shader pour les billboards sphériques.
		eSpherical = 0x0040,
		//!\~english	Shader for fixed size billboards.
		//\~french		Shader pour les billboards à dimensions fixes.
		eFixedSize = 0x0080,
		//!\~english	Shader used to render a shadow map for directional light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière directionnalle.
		eShadowMapDirectional = 0x0100,
		//!\~english	Shader used to render a shadow map for spot light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière projecteur.
		eShadowMapSpot = 0x0200,
		//!\~english	Shader used to render a shadow map for point light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière omnidirectionnelle.
		eShadowMapPoint = 0x0400,
		//!\~english	Shader used to render an environment map.
		//\~french		Shader utilisé pour dessiner une texture d'environnement.
		eEnvironmentMapping = 0x0800,
		//!\~english	Shader for the depth pre-pass.
		//\~french		Shader pour la pré-passe de profondeur.
		eDepthPass = 0x1000,
	};
	IMPLEMENT_FLAGS( ProgramFlag )
	/**
	 *\~english
	 *\brief		Tells if the given flags contain a shadow map flag.
	 *\param[in]	p_flags	The flags to check.
	 *\return		\p true if p_flags contain one of ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, or ProgramFlag::eShadowMapPoint.
	 *\~french
	 *\brief		Dit si les indicateurs donnés contiennent un indicateur de shadow map.
	 *\param[in]	p_flags	Les indicateurs à vérifier.
	 *\return		\p true si p_flags contient l'un de ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, ou ProgramFlag::eShadowMapPoint.
	 */
	bool isShadowMapProgram( ProgramFlags const & p_flags );
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables dimensions enumeration
	\~french
	\brief		Enumération des dimensions de variable supportées
	*/
	enum class VariableType
		: uint8_t
	{
		//!\~english	One variable.
		//!\~french		Une variable.
		eOne,
		//!\~english	1 variable array.
		//!\~french		Tableau de 1 variable.
		eVec1,
		//!\~english	2 variables array.
		//!\~french		Tableau de 2 variables.
		eVec2,
		//!\~english	3 variables array.
		//!\~french		Tableau de 3 variables.
		eVec3,
		//!\~english	4 variables array.
		//!\~french		Tableau de 4 variables.
		eVec4,
		//!\~english	1x1 matrix.
		//!\~french		Matrice 1x1.
		eMat1,
		//!\~english	2x2 matrix.
		//!\~french		Matrice 2x2.
		eMat2x2,
		//!\~english	2x3 matrix.
		//!\~french		Matrice 2x3.
		eMat2x3,
		//!\~english	2x4 matrix.
		//!\~french		Matrice 2x4.
		eMat2x4,
		//!\~english	3x2 matrix.
		//!\~french		Matrice 3x2.
		eMat3x2,
		//!\~english	3x3 matrix.
		//!\~french		Matrice 3x3.
		eMat3x3,
		//!\~english	3x4 matrix.
		//!\~french		Matrice 3x4.
		eMat3x4,
		//!\~english	4x2 matrix.
		//!\~french		Matrice 4x2.
		eMat4x2,
		//!\~english	4x3 matrix.
		//!\~french		Matrice 4x3
		eMat4x3,
		//!\~english	4x4 matrix.
		//!\~french		Matrice 4x4.
		eMat4x4,
		CASTOR_SCOPED_ENUM_BOUNDS( eOne )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables full type enumeration
	\~french
	\brief		Enumération des types complets de variable supportées
	*/
	enum class UniformType
		: uint8_t
	{
		eBool,
		eInt,
		eUInt,
		eFloat,
		eDouble,
		eSampler,
		eVec2b,
		eVec3b,
		eVec4b,
		eVec2i,
		eVec3i,
		eVec4i,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eVec2f,
		eVec3f,
		eVec4f,
		eVec2d,
		eVec3d,
		eVec4d,
		eMat2x2b,
		eMat2x3b,
		eMat2x4b,
		eMat3x2b,
		eMat3x3b,
		eMat3x4b,
		eMat4x2b,
		eMat4x3b,
		eMat4x4b,
		eMat2x2i,
		eMat2x3i,
		eMat2x4i,
		eMat3x2i,
		eMat3x3i,
		eMat3x4i,
		eMat4x2i,
		eMat4x3i,
		eMat4x4i,
		eMat2x2ui,
		eMat2x3ui,
		eMat2x4ui,
		eMat3x2ui,
		eMat3x3ui,
		eMat3x4ui,
		eMat4x2ui,
		eMat4x3ui,
		eMat4x4ui,
		eMat2x2f,
		eMat2x3f,
		eMat2x4f,
		eMat3x2f,
		eMat3x3f,
		eMat3x4f,
		eMat4x2f,
		eMat4x3f,
		eMat4x4f,
		eMat2x2d,
		eMat2x3d,
		eMat2x4d,
		eMat3x2d,
		eMat3x3d,
		eMat3x4d,
		eMat4x2d,
		eMat4x3d,
		eMat4x4d,
#if CASTOR_USE_DOUBLE
		eReal = eDouble,
		eVec2r = eVec2d,
		eVec3r = eVec3d,
		eVec4r = eVec4d,
		eMat2x2r = eMat2x2d,
		eMat2x3r = eMat2x3d,
		eMat2x4r = eMat2x4d,
		eMat3x2r = eMat3x2d,
		eMat3x3r = eMat3x3d,
		eMat3x4r = eMat3x4d,
		eMat4x2r = eMat4x2d,
		eMat4x3r = eMat4x3d,
		eMat4x4r = eMat4x4d,
#else
		eReal = eFloat,
		eVec2r = eVec2f,
		eVec3r = eVec3f,
		eVec4r = eVec4f,
		eMat2x2r = eMat2x2f,
		eMat2x3r = eMat2x3f,
		eMat2x4r = eMat2x4f,
		eMat3x2r = eMat3x2f,
		eMat3x3r = eMat3x3f,
		eMat3x4r = eMat3x4f,
		eMat4x2r = eMat4x2f,
		eMat4x3r = eMat4x3f,
		eMat4x4r = eMat4x4f,
#endif
		CASTOR_SCOPED_ENUM_BOUNDS( eBool )
	};

	class ShaderProgram;
	class ShaderObject;
	class ProgramInputLayout;
	class Uniform;
	class PushUniform;
	class UniformBuffer;
	class UniformBufferBinding;
	class ShaderStorageBuffer;
	class AtomicCounterBuffer;
	class BillboardUbo;
	class MatrixUbo;
	class ModelUbo;
	class ModelMatrixUbo;
	class MorphingUbo;
	class SceneUbo;
	class SkinningUbo;
	class PassBuffer;
	template< UniformType Type >
	class TUniform;
	template< UniformType Type >
	class TPushUniform;

	using Uniform1b = TUniform< UniformType::eBool >;
	using Uniform1i = TUniform< UniformType::eInt >;
	using Uniform1s = TUniform< UniformType::eSampler >;
	using Uniform1ui = TUniform< UniformType::eUInt >;
	using Uniform1f = TUniform< UniformType::eFloat >;
	using Uniform1d = TUniform< UniformType::eDouble >;
	using Uniform1r = TUniform< UniformType::eReal >;
	using Uniform2b = TUniform< UniformType::eVec2b >;
	using Uniform3b = TUniform< UniformType::eVec3b >;
	using Uniform4b = TUniform< UniformType::eVec4b >;
	using Uniform2i = TUniform< UniformType::eVec2i >;
	using Uniform3i = TUniform< UniformType::eVec3i >;
	using Uniform4i = TUniform< UniformType::eVec4i >;
	using Uniform2ui = TUniform< UniformType::eVec2ui >;
	using Uniform3ui = TUniform< UniformType::eVec3ui >;
	using Uniform4ui = TUniform< UniformType::eVec4ui >;
	using Uniform2f = TUniform< UniformType::eVec2f >;
	using Uniform3f = TUniform< UniformType::eVec3f >;
	using Uniform4f = TUniform< UniformType::eVec4f >;
	using Uniform2d = TUniform< UniformType::eVec2d >;
	using Uniform3d = TUniform< UniformType::eVec3d >;
	using Uniform4d = TUniform< UniformType::eVec4d >;
	using Uniform2r = TUniform< UniformType::eVec2r >;
	using Uniform3r = TUniform< UniformType::eVec3r >;
	using Uniform4r = TUniform< UniformType::eVec4r >;
	using Uniform2x2b = TUniform< UniformType::eMat2x2b >;
	using Uniform2x3b = TUniform< UniformType::eMat2x3b >;
	using Uniform2x4b = TUniform< UniformType::eMat2x4b >;
	using Uniform3x2b = TUniform< UniformType::eMat3x2b >;
	using Uniform3x3b = TUniform< UniformType::eMat3x3b >;
	using Uniform3x4b = TUniform< UniformType::eMat3x4b >;
	using Uniform4x2b = TUniform< UniformType::eMat4x2b >;
	using Uniform4x3b = TUniform< UniformType::eMat4x3b >;
	using Uniform4x4b = TUniform< UniformType::eMat4x4b >;
	using Uniform2x2i = TUniform< UniformType::eMat2x2i >;
	using Uniform2x3i = TUniform< UniformType::eMat2x3i >;
	using Uniform2x4i = TUniform< UniformType::eMat2x4i >;
	using Uniform3x2i = TUniform< UniformType::eMat3x2i >;
	using Uniform3x3i = TUniform< UniformType::eMat3x3i >;
	using Uniform3x4i = TUniform< UniformType::eMat3x4i >;
	using Uniform4x2i = TUniform< UniformType::eMat4x2i >;
	using Uniform4x3i = TUniform< UniformType::eMat4x3i >;
	using Uniform4x4i = TUniform< UniformType::eMat4x4i >;
	using Uniform2x2ui = TUniform< UniformType::eMat2x2ui >;
	using Uniform2x3ui = TUniform< UniformType::eMat2x3ui >;
	using Uniform2x4ui = TUniform< UniformType::eMat2x4ui >;
	using Uniform3x2ui = TUniform< UniformType::eMat3x2ui >;
	using Uniform3x3ui = TUniform< UniformType::eMat3x3ui >;
	using Uniform3x4ui = TUniform< UniformType::eMat3x4ui >;
	using Uniform4x2ui = TUniform< UniformType::eMat4x2ui >;
	using Uniform4x3ui = TUniform< UniformType::eMat4x3ui >;
	using Uniform4x4ui = TUniform< UniformType::eMat4x4ui >;
	using Uniform2x2f = TUniform< UniformType::eMat2x2f >;
	using Uniform2x3f = TUniform< UniformType::eMat2x3f >;
	using Uniform2x4f = TUniform< UniformType::eMat2x4f >;
	using Uniform3x2f = TUniform< UniformType::eMat3x2f >;
	using Uniform3x3f = TUniform< UniformType::eMat3x3f >;
	using Uniform3x4f = TUniform< UniformType::eMat3x4f >;
	using Uniform4x2f = TUniform< UniformType::eMat4x2f >;
	using Uniform4x3f = TUniform< UniformType::eMat4x3f >;
	using Uniform4x4f = TUniform< UniformType::eMat4x4f >;
	using Uniform2x2d = TUniform< UniformType::eMat2x2d >;
	using Uniform2x3d = TUniform< UniformType::eMat2x3d >;
	using Uniform2x4d = TUniform< UniformType::eMat2x4d >;
	using Uniform3x2d = TUniform< UniformType::eMat3x2d >;
	using Uniform3x3d = TUniform< UniformType::eMat3x3d >;
	using Uniform3x4d = TUniform< UniformType::eMat3x4d >;
	using Uniform4x2d = TUniform< UniformType::eMat4x2d >;
	using Uniform4x3d = TUniform< UniformType::eMat4x3d >;
	using Uniform4x4d = TUniform< UniformType::eMat4x4d >;
	using Uniform2x2r = TUniform< UniformType::eMat2x2r >;
	using Uniform2x3r = TUniform< UniformType::eMat2x3r >;
	using Uniform2x4r = TUniform< UniformType::eMat2x4r >;
	using Uniform3x2r = TUniform< UniformType::eMat3x2r >;
	using Uniform3x3r = TUniform< UniformType::eMat3x3r >;
	using Uniform3x4r = TUniform< UniformType::eMat3x4r >;
	using Uniform4x2r = TUniform< UniformType::eMat4x2r >;
	using Uniform4x3r = TUniform< UniformType::eMat4x3r >;
	using Uniform4x4r = TUniform< UniformType::eMat4x4r >;

	DECLARE_SMART_PTR( Uniform1b );
	DECLARE_SMART_PTR( Uniform1i );
	DECLARE_SMART_PTR( Uniform1s );
	DECLARE_SMART_PTR( Uniform1ui );
	DECLARE_SMART_PTR( Uniform1f );
	DECLARE_SMART_PTR( Uniform1d );
	DECLARE_SMART_PTR( Uniform1r );
	DECLARE_SMART_PTR( Uniform2b );
	DECLARE_SMART_PTR( Uniform3b );
	DECLARE_SMART_PTR( Uniform4b );
	DECLARE_SMART_PTR( Uniform2i );
	DECLARE_SMART_PTR( Uniform3i );
	DECLARE_SMART_PTR( Uniform4i );
	DECLARE_SMART_PTR( Uniform2ui );
	DECLARE_SMART_PTR( Uniform3ui );
	DECLARE_SMART_PTR( Uniform4ui );
	DECLARE_SMART_PTR( Uniform2f );
	DECLARE_SMART_PTR( Uniform3f );
	DECLARE_SMART_PTR( Uniform4f );
	DECLARE_SMART_PTR( Uniform2d );
	DECLARE_SMART_PTR( Uniform3d );
	DECLARE_SMART_PTR( Uniform4d );
	DECLARE_SMART_PTR( Uniform2r );
	DECLARE_SMART_PTR( Uniform3r );
	DECLARE_SMART_PTR( Uniform4r );
	DECLARE_SMART_PTR( Uniform2x2b );
	DECLARE_SMART_PTR( Uniform2x3b );
	DECLARE_SMART_PTR( Uniform2x4b );
	DECLARE_SMART_PTR( Uniform3x2b );
	DECLARE_SMART_PTR( Uniform3x3b );
	DECLARE_SMART_PTR( Uniform3x4b );
	DECLARE_SMART_PTR( Uniform4x2b );
	DECLARE_SMART_PTR( Uniform4x3b );
	DECLARE_SMART_PTR( Uniform4x4b );
	DECLARE_SMART_PTR( Uniform2x2i );
	DECLARE_SMART_PTR( Uniform2x3i );
	DECLARE_SMART_PTR( Uniform2x4i );
	DECLARE_SMART_PTR( Uniform3x2i );
	DECLARE_SMART_PTR( Uniform3x3i );
	DECLARE_SMART_PTR( Uniform3x4i );
	DECLARE_SMART_PTR( Uniform4x2i );
	DECLARE_SMART_PTR( Uniform4x3i );
	DECLARE_SMART_PTR( Uniform4x4i );
	DECLARE_SMART_PTR( Uniform2x2ui );
	DECLARE_SMART_PTR( Uniform2x3ui );
	DECLARE_SMART_PTR( Uniform2x4ui );
	DECLARE_SMART_PTR( Uniform3x2ui );
	DECLARE_SMART_PTR( Uniform3x3ui );
	DECLARE_SMART_PTR( Uniform3x4ui );
	DECLARE_SMART_PTR( Uniform4x2ui );
	DECLARE_SMART_PTR( Uniform4x3ui );
	DECLARE_SMART_PTR( Uniform4x4ui );
	DECLARE_SMART_PTR( Uniform2x2f );
	DECLARE_SMART_PTR( Uniform2x3f );
	DECLARE_SMART_PTR( Uniform2x4f );
	DECLARE_SMART_PTR( Uniform3x2f );
	DECLARE_SMART_PTR( Uniform3x3f );
	DECLARE_SMART_PTR( Uniform3x4f );
	DECLARE_SMART_PTR( Uniform4x2f );
	DECLARE_SMART_PTR( Uniform4x3f );
	DECLARE_SMART_PTR( Uniform4x4f );
	DECLARE_SMART_PTR( Uniform2x2d );
	DECLARE_SMART_PTR( Uniform2x3d );
	DECLARE_SMART_PTR( Uniform2x4d );
	DECLARE_SMART_PTR( Uniform3x2d );
	DECLARE_SMART_PTR( Uniform3x3d );
	DECLARE_SMART_PTR( Uniform3x4d );
	DECLARE_SMART_PTR( Uniform4x2d );
	DECLARE_SMART_PTR( Uniform4x3d );
	DECLARE_SMART_PTR( Uniform4x4d );
	DECLARE_SMART_PTR( Uniform2x2r );
	DECLARE_SMART_PTR( Uniform2x3r );
	DECLARE_SMART_PTR( Uniform2x4r );
	DECLARE_SMART_PTR( Uniform3x2r );
	DECLARE_SMART_PTR( Uniform3x3r );
	DECLARE_SMART_PTR( Uniform3x4r );
	DECLARE_SMART_PTR( Uniform4x2r );
	DECLARE_SMART_PTR( Uniform4x3r );
	DECLARE_SMART_PTR( Uniform4x4r );

	using PushUniform1b = TPushUniform< UniformType::eBool >;
	using PushUniform1i = TPushUniform< UniformType::eInt >;
	using PushUniform1s = TPushUniform< UniformType::eSampler >;
	using PushUniform1ui = TPushUniform< UniformType::eUInt >;
	using PushUniform1f = TPushUniform< UniformType::eFloat >;
	using PushUniform1d = TPushUniform< UniformType::eDouble >;
	using PushUniform1r = TPushUniform< UniformType::eReal >;
	using PushUniform2b = TPushUniform< UniformType::eVec2b >;
	using PushUniform3b = TPushUniform< UniformType::eVec3b >;
	using PushUniform4b = TPushUniform< UniformType::eVec4b >;
	using PushUniform2i = TPushUniform< UniformType::eVec2i >;
	using PushUniform3i = TPushUniform< UniformType::eVec3i >;
	using PushUniform4i = TPushUniform< UniformType::eVec4i >;
	using PushUniform2ui = TPushUniform< UniformType::eVec2ui >;
	using PushUniform3ui = TPushUniform< UniformType::eVec3ui >;
	using PushUniform4ui = TPushUniform< UniformType::eVec4ui >;
	using PushUniform2f = TPushUniform< UniformType::eVec2f >;
	using PushUniform3f = TPushUniform< UniformType::eVec3f >;
	using PushUniform4f = TPushUniform< UniformType::eVec4f >;
	using PushUniform2d = TPushUniform< UniformType::eVec2d >;
	using PushUniform3d = TPushUniform< UniformType::eVec3d >;
	using PushUniform4d = TPushUniform< UniformType::eVec4d >;
	using PushUniform2r = TPushUniform< UniformType::eVec2r >;
	using PushUniform3r = TPushUniform< UniformType::eVec3r >;
	using PushUniform4r = TPushUniform< UniformType::eVec4r >;
	using PushUniform2x2b = TPushUniform< UniformType::eMat2x2b >;
	using PushUniform2x3b = TPushUniform< UniformType::eMat2x3b >;
	using PushUniform2x4b = TPushUniform< UniformType::eMat2x4b >;
	using PushUniform3x2b = TPushUniform< UniformType::eMat3x2b >;
	using PushUniform3x3b = TPushUniform< UniformType::eMat3x3b >;
	using PushUniform3x4b = TPushUniform< UniformType::eMat3x4b >;
	using PushUniform4x2b = TPushUniform< UniformType::eMat4x2b >;
	using PushUniform4x3b = TPushUniform< UniformType::eMat4x3b >;
	using PushUniform4x4b = TPushUniform< UniformType::eMat4x4b >;
	using PushUniform2x2i = TPushUniform< UniformType::eMat2x2i >;
	using PushUniform2x3i = TPushUniform< UniformType::eMat2x3i >;
	using PushUniform2x4i = TPushUniform< UniformType::eMat2x4i >;
	using PushUniform3x2i = TPushUniform< UniformType::eMat3x2i >;
	using PushUniform3x3i = TPushUniform< UniformType::eMat3x3i >;
	using PushUniform3x4i = TPushUniform< UniformType::eMat3x4i >;
	using PushUniform4x2i = TPushUniform< UniformType::eMat4x2i >;
	using PushUniform4x3i = TPushUniform< UniformType::eMat4x3i >;
	using PushUniform4x4i = TPushUniform< UniformType::eMat4x4i >;
	using PushUniform2x2ui = TPushUniform< UniformType::eMat2x2ui >;
	using PushUniform2x3ui = TPushUniform< UniformType::eMat2x3ui >;
	using PushUniform2x4ui = TPushUniform< UniformType::eMat2x4ui >;
	using PushUniform3x2ui = TPushUniform< UniformType::eMat3x2ui >;
	using PushUniform3x3ui = TPushUniform< UniformType::eMat3x3ui >;
	using PushUniform3x4ui = TPushUniform< UniformType::eMat3x4ui >;
	using PushUniform4x2ui = TPushUniform< UniformType::eMat4x2ui >;
	using PushUniform4x3ui = TPushUniform< UniformType::eMat4x3ui >;
	using PushUniform4x4ui = TPushUniform< UniformType::eMat4x4ui >;
	using PushUniform2x2f = TPushUniform< UniformType::eMat2x2f >;
	using PushUniform2x3f = TPushUniform< UniformType::eMat2x3f >;
	using PushUniform2x4f = TPushUniform< UniformType::eMat2x4f >;
	using PushUniform3x2f = TPushUniform< UniformType::eMat3x2f >;
	using PushUniform3x3f = TPushUniform< UniformType::eMat3x3f >;
	using PushUniform3x4f = TPushUniform< UniformType::eMat3x4f >;
	using PushUniform4x2f = TPushUniform< UniformType::eMat4x2f >;
	using PushUniform4x3f = TPushUniform< UniformType::eMat4x3f >;
	using PushUniform4x4f = TPushUniform< UniformType::eMat4x4f >;
	using PushUniform2x2d = TPushUniform< UniformType::eMat2x2d >;
	using PushUniform2x3d = TPushUniform< UniformType::eMat2x3d >;
	using PushUniform2x4d = TPushUniform< UniformType::eMat2x4d >;
	using PushUniform3x2d = TPushUniform< UniformType::eMat3x2d >;
	using PushUniform3x3d = TPushUniform< UniformType::eMat3x3d >;
	using PushUniform3x4d = TPushUniform< UniformType::eMat3x4d >;
	using PushUniform4x2d = TPushUniform< UniformType::eMat4x2d >;
	using PushUniform4x3d = TPushUniform< UniformType::eMat4x3d >;
	using PushUniform4x4d = TPushUniform< UniformType::eMat4x4d >;
	using PushUniform2x2r = TPushUniform< UniformType::eMat2x2r >;
	using PushUniform2x3r = TPushUniform< UniformType::eMat2x3r >;
	using PushUniform2x4r = TPushUniform< UniformType::eMat2x4r >;
	using PushUniform3x2r = TPushUniform< UniformType::eMat3x2r >;
	using PushUniform3x3r = TPushUniform< UniformType::eMat3x3r >;
	using PushUniform3x4r = TPushUniform< UniformType::eMat3x4r >;
	using PushUniform4x2r = TPushUniform< UniformType::eMat4x2r >;
	using PushUniform4x3r = TPushUniform< UniformType::eMat4x3r >;
	using PushUniform4x4r = TPushUniform< UniformType::eMat4x4r >;

	DECLARE_SMART_PTR( PushUniform1b );
	DECLARE_SMART_PTR( PushUniform1i );
	DECLARE_SMART_PTR( PushUniform1s );
	DECLARE_SMART_PTR( PushUniform1ui );
	DECLARE_SMART_PTR( PushUniform1f );
	DECLARE_SMART_PTR( PushUniform1d );
	DECLARE_SMART_PTR( PushUniform1r );
	DECLARE_SMART_PTR( PushUniform2b );
	DECLARE_SMART_PTR( PushUniform3b );
	DECLARE_SMART_PTR( PushUniform4b );
	DECLARE_SMART_PTR( PushUniform2i );
	DECLARE_SMART_PTR( PushUniform3i );
	DECLARE_SMART_PTR( PushUniform4i );
	DECLARE_SMART_PTR( PushUniform2ui );
	DECLARE_SMART_PTR( PushUniform3ui );
	DECLARE_SMART_PTR( PushUniform4ui );
	DECLARE_SMART_PTR( PushUniform2f );
	DECLARE_SMART_PTR( PushUniform3f );
	DECLARE_SMART_PTR( PushUniform4f );
	DECLARE_SMART_PTR( PushUniform2d );
	DECLARE_SMART_PTR( PushUniform3d );
	DECLARE_SMART_PTR( PushUniform4d );
	DECLARE_SMART_PTR( PushUniform2r );
	DECLARE_SMART_PTR( PushUniform3r );
	DECLARE_SMART_PTR( PushUniform4r );
	DECLARE_SMART_PTR( PushUniform2x2b );
	DECLARE_SMART_PTR( PushUniform2x3b );
	DECLARE_SMART_PTR( PushUniform2x4b );
	DECLARE_SMART_PTR( PushUniform3x2b );
	DECLARE_SMART_PTR( PushUniform3x3b );
	DECLARE_SMART_PTR( PushUniform3x4b );
	DECLARE_SMART_PTR( PushUniform4x2b );
	DECLARE_SMART_PTR( PushUniform4x3b );
	DECLARE_SMART_PTR( PushUniform4x4b );
	DECLARE_SMART_PTR( PushUniform2x2i );
	DECLARE_SMART_PTR( PushUniform2x3i );
	DECLARE_SMART_PTR( PushUniform2x4i );
	DECLARE_SMART_PTR( PushUniform3x2i );
	DECLARE_SMART_PTR( PushUniform3x3i );
	DECLARE_SMART_PTR( PushUniform3x4i );
	DECLARE_SMART_PTR( PushUniform4x2i );
	DECLARE_SMART_PTR( PushUniform4x3i );
	DECLARE_SMART_PTR( PushUniform4x4i );
	DECLARE_SMART_PTR( PushUniform2x2ui );
	DECLARE_SMART_PTR( PushUniform2x3ui );
	DECLARE_SMART_PTR( PushUniform2x4ui );
	DECLARE_SMART_PTR( PushUniform3x2ui );
	DECLARE_SMART_PTR( PushUniform3x3ui );
	DECLARE_SMART_PTR( PushUniform3x4ui );
	DECLARE_SMART_PTR( PushUniform4x2ui );
	DECLARE_SMART_PTR( PushUniform4x3ui );
	DECLARE_SMART_PTR( PushUniform4x4ui );
	DECLARE_SMART_PTR( PushUniform2x2f );
	DECLARE_SMART_PTR( PushUniform2x3f );
	DECLARE_SMART_PTR( PushUniform2x4f );
	DECLARE_SMART_PTR( PushUniform3x2f );
	DECLARE_SMART_PTR( PushUniform3x3f );
	DECLARE_SMART_PTR( PushUniform3x4f );
	DECLARE_SMART_PTR( PushUniform4x2f );
	DECLARE_SMART_PTR( PushUniform4x3f );
	DECLARE_SMART_PTR( PushUniform4x4f );
	DECLARE_SMART_PTR( PushUniform2x2d );
	DECLARE_SMART_PTR( PushUniform2x3d );
	DECLARE_SMART_PTR( PushUniform2x4d );
	DECLARE_SMART_PTR( PushUniform3x2d );
	DECLARE_SMART_PTR( PushUniform3x3d );
	DECLARE_SMART_PTR( PushUniform3x4d );
	DECLARE_SMART_PTR( PushUniform4x2d );
	DECLARE_SMART_PTR( PushUniform4x3d );
	DECLARE_SMART_PTR( PushUniform4x4d );
	DECLARE_SMART_PTR( PushUniform2x2r );
	DECLARE_SMART_PTR( PushUniform2x3r );
	DECLARE_SMART_PTR( PushUniform2x4r );
	DECLARE_SMART_PTR( PushUniform3x2r );
	DECLARE_SMART_PTR( PushUniform3x3r );
	DECLARE_SMART_PTR( PushUniform3x4r );
	DECLARE_SMART_PTR( PushUniform4x2r );
	DECLARE_SMART_PTR( PushUniform4x3r );
	DECLARE_SMART_PTR( PushUniform4x4r );

	DECLARE_SMART_PTR( UniformBuffer );
	DECLARE_SMART_PTR( UniformBufferBinding );
	DECLARE_SMART_PTR( Uniform );
	DECLARE_SMART_PTR( PushUniform );
	DECLARE_SMART_PTR( ShaderObject );
	DECLARE_SMART_PTR( ShaderProgram );
	DECLARE_SMART_PTR( ShaderStorageBuffer );
	DECLARE_SMART_PTR( AtomicCounterBuffer );

	DECLARE_VECTOR( ShaderProgramSPtr, ShaderProgramPtr );
	DECLARE_VECTOR( ShaderObjectSPtr, ShaderObjectPtr );
	DECLARE_LIST( PushUniformSPtr, PushUniform );
	DECLARE_LIST( UniformSPtr, Uniform );
	DECLARE_LIST( UniformBufferSPtr, UniformBufferPtr );
	DECLARE_LIST( ShaderStorageBufferSPtr, ShaderStorageBufferPtr );
	DECLARE_LIST( AtomicCounterBufferSPtr, AtomicCounterBufferPtr );
	DECLARE_MAP( castor::String, PushUniformWPtr, PushUniform );
	DECLARE_MAP( castor::String, UniformWPtr, Uniform );
	DECLARE_MAP( ShaderProgramRPtr, UniformBufferBindingUPtr, UniformBufferBinding );
	DECLARE_MAP( castor::String, UniformBufferWPtr, UniformBufferPtrStr );
	DECLARE_MAP( castor::String, ShaderStorageBufferWPtr, ShaderStorageBufferPtrStr );
	DECLARE_MAP( castor::String, AtomicCounterBufferWPtr, AtomicCounterBufferPtrStr );
	DECLARE_MAP( ShaderType, UniformBufferWPtr, UniformBufferPtrShader );
	DECLARE_MAP( ShaderType, ShaderStorageBufferWPtr, ShaderStorageBufferPtrShader );
	DECLARE_MAP( ShaderType, AtomicCounterBufferWPtr, AtomicCounterBufferPtrShader );

	//@}

	namespace shader
	{
		enum class TypeName
		{
			eLight = int( glsl::TypeName::eCount ),
			eDirectionalLight,
			ePointLight,
			eSpotLight,
			eMaterial,
			eLegacyMaterial,
			eMetallicRoughnessMaterial,
			eSpecularGlossinessMaterial,
		};

		static constexpr uint32_t SpotShadowMapCount = 10u;
		static constexpr uint32_t PointShadowMapCount = 6u;
		static constexpr int BaseLightComponentsCount = 2;
		static constexpr int MaxLightComponentsCount = 14;
		static constexpr float LightComponentsOffset = MaxLightComponentsCount * 0.001f;


		class Shadow;
		struct Light;
		struct DirectionalLight;
		struct PointLight;
		struct SpotLight;
		class Materials;
		class LightingModel;
		class PhongLightingModel;
		class MetallicBrdfLightingModel;
		class SpecularBrdfLightingModel;
		struct BaseMaterial;
		struct LegacyMaterial;
		struct MetallicRoughnessMaterial;
		struct SpecularGlossinessMaterial;

		namespace legacy
		{
			C3D_API void computePreLightingMapContributions( glsl::GlslWriter & writer
				, glsl::Vec3 & p_normal
				, glsl::Float & p_shininess
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags
				, PassFlags const & passFlags );

			C3D_API void computePostLightingMapContributions( glsl::GlslWriter & writer
				, glsl::Vec3 & p_diffuse
				, glsl::Vec3 & p_specular
				, glsl::Vec3 & p_emissive
				, glsl::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, ShadowType shadows
				, uint32_t & index );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, LightType light
				, ShadowType shadows
				, uint32_t & index );
		}

		namespace pbr
		{
			namespace mr
			{
				C3D_API void computePreLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_normal
					, glsl::Float & p_metallic
					, glsl::Float & p_roughness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags );

				C3D_API void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_albedo
					, glsl::Vec3 & p_emissive
					, glsl::Float const & p_gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, ShadowType shadows
					, uint32_t & index );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType light
					, ShadowType shadows
					, uint32_t & index );
			}

			namespace sg
			{
				C3D_API void computePreLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & normal
					, glsl::Vec3 & specular
					, glsl::Float & glossiness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags );

				C3D_API void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & diffuse
					, glsl::Vec3 & emissive
					, glsl::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, ShadowType shadows
					, uint32_t & index );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType light
					, ShadowType shadows
					, uint32_t & index );
			}
		}
		/**
		 *\~english
		 *\brief		Creates the appropriate GLSL materials buffer.
		 *\param[in]	writer		The GLSL writer.
		 *\param		passFlags	The pass flags.
		 *\~french
		 *\brief		Crée le tampon de matériaux GLSL approprié.
		 *\param[in]	writer		Le writer GLSL.
		 *\param		passFlags	Les indicateurs de passe.
		 */
		C3D_API std::unique_ptr< Materials > createMaterials( glsl::GlslWriter & writer
			, PassFlags const & passFlags );
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param[in]	writer		The GLSL writer.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	alpha		The alpha TypeEnum.
		 *\param[in]	alphaRef	The alpha comparison reference TypeEnum.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param[in]	writer		Le writer GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	alpha		La valeur d'opacité.
		 *\param[in]	alphaRef	La valeur de référence pour la comparaison alpha.
		 */
		C3D_API void applyAlphaFunc( glsl::GlslWriter & writer
			, ComparisonFunc alphaFunc
			, glsl::Float const & alpha
			, glsl::Float const & alphaRef );

		using ParallaxFunction = glsl::Function< glsl::Vec2, glsl::InParam< glsl::Vec2 >, glsl::InParam< glsl::Vec3 > >;
		using ParallaxShadowFunction = glsl::Function< glsl::Float, glsl::InParam< glsl::Vec3 >, glsl::InParam< glsl::Vec2 >, glsl::InParam< glsl::Float > >;

		C3D_API ParallaxFunction declareParallaxMappingFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags );

		C3D_API ParallaxShadowFunction declareParallaxShadowFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags );

		DECLARE_GLSL_PARAMETER( Light );
	}
}

namespace glsl
{
	template<>
	struct TypeTraits< castor3d::shader::Light >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::DirectionalLight >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eDirectionalLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::PointLight >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::ePointLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::SpotLight >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eSpotLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::BaseMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eMaterial );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::LegacyMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eLegacyMaterial );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::MetallicRoughnessMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eMetallicRoughnessMaterial );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::SpecularGlossinessMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eSpecularGlossinessMaterial );
		C3D_API static castor::String const Name;
	};
}

#endif
