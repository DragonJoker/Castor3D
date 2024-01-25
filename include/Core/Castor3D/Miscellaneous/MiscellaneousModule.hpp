/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MiscellaneousModule_H___
#define ___C3D_MiscellaneousModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace castor3d
{
	/**@name Miscellaneous */
	//@{

	/**
	*\~english
	*\brief
	*	All supported GPU feature flags.
	*\~french
	*\brief
	*	Indicateurs de caractérisituqes du GPU.
	*/
	enum class GpuFeature
		: uint32_t
	{
		//!\~english	Tells whether or not the selected render API supports stereographic rendering.
		//!\~french		Dit si l'API de rendu choisie supporte la stéréographie.
		eStereoRendering = 0x00000001,
		//!\~english	Tells whether or not the selected render API supports shader stoarage buffers.
		//!\~french		Dit si l'API de rendu choisie supporte les tampons de stockage shader.
		eShaderStorageBuffers = 0x00000002,
		//!\~english	Tells whether or not the selected render API supports bindless render.
		//!\~french		Dit si l'API de rendu choisie supporte le rendu en bindless.
		eBindless = 0x00000004,
		//!\~english	Tells whether or not the selected render API supports geometry shaders.
		//!\~french		Dit si l'API de rendu choisie supporte les geometry shaders.
		eGeometry = 0x00000008,
		//!\~english	Tells whether or not the selected render API supports tessellation shaders.
		//!\~french		Dit si l'API de rendu choisie supporte les tessellation shaders.
		eTessellation = 0x00000010,
		//!\~english	Tells whether or not the selected render API supports ray tracing shaders.
		//!\~french		Dit si l'API de rendu choisie supporte les ray tracing shaders.
		eRayTracing = 0x00000020,
		//!\~english	Tells whether or not the selected render API supports mesh shaders.
		//!\~french		Dit si l'API de rendu choisie supporte les mesh shaders.
		eMesh = 0x00000040,
	};
	CU_ImplementFlags( GpuFeature )
	/**
	*\~english
	*\brief
	*	Maximum values enumerator.
	*\~french
	*\brief
	*	Enumération des valeurs maximales.
	*/
	enum class GpuMax
		: uint32_t
	{
		eTexture1DSize = 0,
		eTexture2DSize = 1,
		eTexture3DSize = 2,
		eTextureCubeSize = 3,
		eTextureLayers = 4,
		eSamplerLodBias = 5,
		eClipDistances = 6,
		eFramebufferWidth = 7,
		eFramebufferHeight = 8,
		eFramebufferLayers = 9,
		eFramebufferSamples = 10,
		eTexelBufferSize = 11,
		eUniformBufferSize = 12,
		eStorageBufferSize = 13,
		eViewportWidth = 14,
		eViewportHeight = 15,
		eViewports = 16,
		eWorkGroupCountX = 17,
		eWorkGroupCountY = 18,
		eWorkGroupCountZ = 19,
		eWorkGroupSizeX = 20,
		eWorkGroupSizeY = 21,
		eWorkGroupSizeZ = 22,
		eWorkGroupInvocations = 23,
		eMeshWorkGroupInvocations = 24,
		eMeshWorkGroupSizeX = 25,
		eMeshWorkGroupSizeY = 26,
		eMeshWorkGroupSizeZ = 27,
		eMeshOutputVertices = 28,
		eMeshOutputPrimitives = 29,
		eTaskWorkGroupInvocations = 30,
		eTaskWorkGroupSizeX = 31,
		eTaskWorkGroupSizeY = 32,
		eTaskWorkGroupSizeZ = 33,
		CU_ScopedEnumBounds( eTexture1DSize, eTaskWorkGroupSizeZ )
	};
	/**
	*\~english
	*\brief
	*	Minimum values enumerator.
	*\~french
	*\brief
	*	Enumération des valeurs minimales.
	*/
	enum class GpuMin
		: uint32_t
	{
		eBufferMapSize = 0,
		eUniformBufferOffsetAlignment = 1,
		CU_ScopedEnumBounds( eBufferMapSize, eUniformBufferOffsetAlignment )
	};
	/**
	*\~english
	*\brief
	*	Factors applied when displaying a debug texture.
	*\~french
	*\brief
	*	Facteurs appliqués lors de l'affichage debug d'une texture.
	*/
	struct TextureFactors
	{
		castor::Point3f multiply{ 1.0f, 1.0f, 1.0f };
		castor::Point3f add{};
		castor::Point4f const * grid{ nullptr };
		bool invertY{ false };
		bool isSlice{};
		uint32_t slice{};
		bool isDepth{};

		static TextureFactors tex2D( castor::Point3f multiply = castor::Point3f{ 1.0f, 1.0f, 1.0f }
			, castor::Point3f add = castor::Point3f{} )
		{
			TextureFactors result;
			result.multiply = multiply;
			result.add = add;
			return result;
		}

		static TextureFactors tex3D( castor::Point4f const * grid )
		{
			TextureFactors result;
			result.grid = grid;
			return result;
		}

		static TextureFactors tex3DSlice( uint32_t slice )
		{
			TextureFactors result;
			result.isSlice = true;
			result.slice = slice;
			return result;
		}

		TextureFactors & invert( bool value )
		{
			invertY = value;
			return *this;
		}

		TextureFactors & depth( bool value )
		{
			isDepth = value;
			return *this;
		}
	};
	/**
	*\~english
	*\brief
	*	Wraps a point to make it considered as a colour by visitors.
	*\~french
	*\brief
	*	Contient un point pour le faire considérer comme une couleur par les visiteurs.
	*/
	struct ColourWrapper
	{
		castor::Point3f * value;
	};
	/**
	*\~english
	*\brief
	*	Wrapper around debug_utils and debug_report callbacks.
	*\~french
	*\brief
	*	Wrapper autour des callbacks de debug_utils and debug_report callbacks.
	*/
	class DebugCallbacks;
	/**
	*\~english
	*\brief
	*	Holds GPU informations.
	*\~french
	*\brief
	*	Contient des informations sur le GPU.
	*/
	class GpuInformations;
	/**
	*\~english
	*\brief
	*	Tracks objects allocated on GPU.
	*\~french
	*\brief
	*	Trace les objets alloués sur le GPU.
	*/
	class GpuObjectTracker;
	/**
	\~english
	\brief		Well... a loading screen implementation.
	\~french
	\brief		Euh... Une implémentation d'écran de chargement.
	*/
	class LoadingScreen;
	/**
	\~english
	\brief
	*	Configuration parameters.
	\remarks
	*	Pair of a name and binary data.
	\~french
	\brief
	*	Paramètres de configuration.
	\remarks
	*	Couple nom/données binaires.
	*/
	template< typename KeyT >
	class ParametersT;
	/**
	*\~english
	*\brief
	*	Pattern handler class.
	*\remark
	*	A pattern is a collection of consecutive points.
	*\~french
	*\brief
	*	Classe de gestion de chemin.
	*\remarks
	*	Un chemin est une collection de points consécutifs.
	*/
	template< typename T >
	class Pattern;
	/**
	*\~english
	*\brief
	*	Pipeline visitor base class.
	*\~french
	*\brief
	*	Classe de base d'un visiteur de pipeline.
	*/
	class ConfigurationVisitorBase;
	/**
	*\~english
	*\brief
	*	Pipeline visitor base class.
	*\~french
	*\brief
	*	Classe de base d'un visiteur de pipeline.
	*/
	class ConfigurationVisitor;
	/**
	*\~english
	*\brief
	*	A progress bar, overlay based.
	*\~french
	*\brief
	*	Une barre de progression, basée sur des overlays.
	*/
	class ProgressBar;
	/**
	*\~english
	*\brief
	*	Version management class
	*\remark
	*	Class used to manage versions and versions dependencies for plug-ins
	*\~french
	*\brief
	*	Classe de gestion de version
	*\remark
	*	Classe utilisee pour gerer les versions et dependances de version pour les plug-ins
	*/
	class Version;
	/**
	*\~english
	*\brief
	*	Version comparison exception
	*\~french
	*\brief
	*	Exception de comparaison de versions
	*/
	class VersionException;

	using Parameters = ParametersT< castor::String >;

	CU_DeclareSmartPtr( castor3d, LoadingScreen, C3D_API );
	CU_DeclareSmartPtr( castor3d, ProgressBar, C3D_API );

	struct RenderDevice;
	C3D_API ashes::DeviceMemoryPtr setupMemory( ashes::Device const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, castor::String const & name );
	C3D_API ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, castor::String const & name );

	C3D_API void initProgressBarGlobalRange( ProgressBar * progress
		, uint32_t value );
	C3D_API uint32_t incProgressBarGlobalRange( ProgressBar * progress
		, uint32_t increment );
	C3D_API void setProgressBarGlobalTitle( ProgressBar * progress
		, castor::String const & globalTitle );
	C3D_API void setProgressBarGlobalStep( ProgressBar * progress
		, castor::String const & globalTitle
		, uint32_t count );

	C3D_API void stepProgressBarLocal( ProgressBar * progress
		, castor::String const & localLabel );

	C3D_API void stepProgressBarGlobalStartLocal( ProgressBar * progress
		, castor::String const & globalLabel
		, uint32_t rangeMax );
	C3D_API void setProgressBarLocal( ProgressBar * progress
		, castor::String const & globalLabel
		, castor::String const & localLabel
		, uint32_t rangeMax
		, uint32_t value );

	//@}
}

#endif
