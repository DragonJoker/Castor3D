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
	};
	CU_ImplementFlags( GpuFeature )
	/**
	*\~english
	*\brief
	*	All supported GPU feature flags.
	*\~french
	*\brief
	*	Indicateurs de caractérisituqes du GPU.
	*/
	enum class GpuMax
		: uint32_t
	{
		eTexture1DSize,
		eTexture2DSize,
		eTexture3DSize,
		eTextureCubeSize,
		eTextureLayers,
		eSamplerLodBias,
		eClipDistances,
		eFramebufferWidth,
		eFramebufferHeight,
		eFramebufferLayers,
		eFramebufferSamples,
		eTexelBufferSize,
		eUniformBufferSize,
		eStorageBufferSize,
		eViewportWidth,
		eViewportHeight,
		eViewports,
		CU_ScopedEnumBounds( eTexture1DSize )
	};
	template< typename AshesType >
	struct AshesTypeTraits;
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
	*	Post effect visitor base class.
	*\~french
	*\brief
	*	Classe de base d'un visiteur d'effet post rendu.
	*/
	class PipelineVisitorBase;
	/**
	*\~english
	*\brief
	*	Post effect visitor base class.
	*\~french
	*\brief
	*	Classe de base d'un visiteur d'effet post rendu.
	*/
	class PipelineVisitor;
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

	struct RenderDevice;
	C3D_API void setDebugObjectName( ashes::Device const & device
		, uint64_t object
		, uint32_t type
		, std::string const & name
		, std::string const & typeName );
	C3D_API void setDebugObjectName( RenderDevice const & device
		, uint64_t object
		, uint32_t type
		, std::string const & name
		, std::string const & typeName );
	C3D_API ashes::DeviceMemoryPtr setupMemory( ashes::Device const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, std::string const & name );
	C3D_API ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, std::string const & name );

	//@}
}

#endif
