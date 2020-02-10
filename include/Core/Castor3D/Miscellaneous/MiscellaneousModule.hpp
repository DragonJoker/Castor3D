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
	*	Frustum corners enumeration
	*\~french
	*\brief
	*	Enumération des coins d'un frustum
	*/
	enum class FrustumCorner
	{
		//!\~english	Far left bottom corner.
		//!\~french		Coin éloigné bas gauche.
		eFarLeftBottom,
		//!\~english	Far left top corner.
		//!\~french		Coin éloigné haut gauche.
		eFarLeftTop,
		//!\~english	Far right top corner.
		//!\~french		Coin éloigné haut droit.
		eFarRightTop,
		//!\~english	Far right bottom corner.
		//!\~french		Coin éloigné bas droit.
		eFarRightBottom,
		//!\~english	Near left bottom corner.
		//!\~french		Coin proche bas gauche.
		eNearLeftBottom,
		//!\~english	Near left top corner.
		//!\~french		Coin proche haut gauche.
		eNearLeftTop,
		//!\~english	Near right top corner.
		//!\~french		Coin proche haut droit.
		eNearRightTop,
		//!\~english	Near right bottom corner.
		//!\~french		Coin proche bas droit.
		eNearRightBottom,
		CU_ScopedEnumBounds( eFarLeftBottom )
	};
	castor::String getName( FrustumCorner value );
	/**
	*\~english
	*\brief
	*	Frustum planes enumeration
	*\~french
	*\brief
	*	Enumération des plans d'un frustum.
	*/
	enum class FrustumPlane
	{
		//!\~english	Near plane.
		//!\~french		Plan proche.
		eNear,
		//!\~english	Far plane.
		//!\~french		Plan éloigné.
		eFar,
		//!\~english	Left plane.
		//!\~french		Plan gauche.
		eLeft,
		//!\~english	Right plane.
		//!\~french		Plan droit.
		eRight,
		//!\~english	Top plane.
		//!\~french		Plan haut.
		eTop,
		//!\~english	Bottom plane.
		//!\~french		Plan bas.
		eBottom,
		CU_ScopedEnumBounds( eNear )
	};
	castor::String getName( FrustumPlane value );
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
		eTexture3DSize,
		eTextureRectangleSize,
		eTextureCubeMapSize,
		eTextureBufferSize,
		eTextureSize,
		eTextureLodBias,

		eArrayTextureLayers,
		eClipDistances,

		eElementIndices,
		eElementVertices,

		eFramebufferWidth,
		eFramebufferHeight,
		eFramebufferLayers,
		eFramebufferSamples,

		eUniformBufferSize,

		eViewportWidth,
		eViewportHeight,
		eViewports,

		CU_ScopedEnumBounds( eTexture3DSize )
	};
	/**
	*\~english
	*\brief
	*	The picking node types.
	*\~french
	*\brief
	*	Les types de noeud de picking.
	*/
	enum class PickNodeType
		: uint8_t
	{
		eNone,
		eStatic,
		eInstantiatedStatic,
		eSkinning,
		eInstantiatedSkinning,
		eMorphing,
		eBillboard
	};
	castor::String getName( PickNodeType value );
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
	*	Implements a frustum and the checks related to frustum culling.
	*\~french
	*\brief
	*	Implémente un frustum et les vérifications relatives au frustum culling.
	*/
	class Frustum;
	/**
	*\~english
	*\brief
	*	Gaussian blur pass.
	*\~french
	*\brief
	*	Passe flou gaussien.
	*/
	class GaussianBlur;
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
	class Parameters;
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
	*	Picking pass, using FBO.
	*\~french
	*\brief
	*	Passe de picking, utilisant les FBO.
	*/
	class PickingPass;
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
	*	Ray representation
	*\remarks
	*	A ray is an origin and a direction in 3D.
	*\~french
	*\brief
	*	Représentation d'un rayon.
	*\remarks
	*	Un rayon est représentaté par une origine et une direction.
	*/
	class Ray;
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

	CU_DeclareSmartPtr( PickingPass );
	CU_DeclareSmartPtr( GaussianBlur );

	//@}
}

#endif
