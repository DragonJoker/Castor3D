/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrameEventModule_H___
#define ___C3D_FrameEventModule_H___

#include "Castor3D/Cache/CacheModule.hpp"

namespace castor3d
{
	/**@name Event */
	//@{
	/**@name Frame */
	//@{

	/**
	*\~english
	*\brief
	*	CPU Frame Event Type enumeration
	*\~french
	*\brief
	*	Enumération des types d'évènement CPU de frame
	*/
	enum class CpuEventType
		: uint8_t
	{
		//!\~english	This kind of event happens before the GPU step.
		//!\~french		Ce type d'évènement est traité avant la phase GPU
		ePreGpuStep = 0,
		//!\~english	This kind of event happens after GPU step and before CPU step.
		//!\~french		Ce type d'évènement est traité après la phase GPU et avant la phase CPU.
		ePreCpuStep = 1,
		//!\~english	This kind of event happens after the CPU step.
		//!\~french		Ce type d'évènement est traité après la phase CPU.
		ePostCpuStep = 2,
		CU_ScopedEnumBounds( ePreGpuStep, ePostCpuStep )
	};
	C3D_API castor::String getName( CpuEventType value );
	/**
	*\~english
	*\brief
	*	GPU Frame Event Type enumeration
	*\~french
	*\brief
	*	Enumération des types d'évènement GPU de frame
	*/
	enum class GpuEventType
		: uint8_t
	{
		//!\~english	This kind of event happens before the upload.
		//!\~french		Ce type d'évènement est traité avant l'upload.
		ePreUpload = 0,
		//!\~english	This kind of event happens after the upload and before render.
		//!\~french		Ce type d'évènement est traité après l'upload et avant le rendu.
		ePreRender = 1,
		//!\~english	This kind of event happens after the render.
		//!\~french		Ce type d'évènement est traité après le rendu.
		ePostRender = 2,
		CU_ScopedEnumBounds( ePreUpload, ePostRender )
	};
	C3D_API castor::String getName( GpuEventType value );
	/**
	*\~english
	*\brief
	*	The interface which represents a frame event.
	*\remarks
	*	Basically a frame event has a EventType to know when it must be applied.
	*	<br />It can be applied, so the function must be implemented by children classes
	*\~french
	*\brief
	*	Interface représentant un évènement de frame
	*\remarks
	*	Un évènement a un EventType pour savoir quand il doit être traité.
	*	<br />La fonction de traitement doit être implémentée par les classes filles.
	*/
	class CpuFrameEvent;
	/**
	*\~english
	*\brief
	*	Functor event
	*\remarks
	*	Executes a function when processed
	*\~french
	*\brief
	*	Evènement foncteur
	*\remarks
	*	Excécute une fonction lorsqu'il est traité
	*/
	class CpuFunctorEvent;
	/**
	*\~english
	*\brief
	*	User event synchronisation class.
	*\remarks
	*	The handler of the frame events. It can add frame events and applies them at the wanted times.
	*\~french
	*\brief
	*	Classe de synchronisation des évènements.
	*\remarks
	*	Le gestionnaire des évènements de frame, on peut y ajouter des évènements à traiter, qui le seront au moment voulu (en fonction de leur EventType).
	*/
	class FrameListener;
	/**
	*\~english
	*\brief
	*	The interface which represents a frame event.
	*\remarks
	*	Basically a frame event has a EventType to know when it must be applied.
	*	<br />It can be applied, so the function must be implemented by children classes
	*\~french
	*\brief
	*	Interface représentant un évènement de frame
	*\remarks
	*	Un évènement a un EventType pour savoir quand il doit être traité.
	*	<br />La fonction de traitement doit être implémentée par les classes filles.
	*/
	class GpuFrameEvent;
	/**
	*\~english
	*\brief
	*	Functor event
	*\remarks
	*	Executes a function when processed
	*\~french
	*\brief
	*	Evènement foncteur
	*\remarks
	*	Excécute une fonction lorsqu'il est traité
	*/
	class GpuFunctorEvent;

	CU_DeclareSmartPtr( castor3d, CpuFrameEvent, C3D_API );
	CU_DeclareSmartPtr( castor3d, FrameListener, C3D_API );
	CU_DeclareSmartPtr( castor3d, GpuFrameEvent, C3D_API );
	CU_DeclareSmartPtr( castor3d, CpuFunctorEvent, C3D_API );
	CU_DeclareSmartPtr( castor3d, GpuFunctorEvent, C3D_API );

	CU_DeclareVector( CpuFrameEventUPtr, CpuFrameEventPtr );
	CU_DeclareVector( GpuFrameEventUPtr, GpuFrameEventPtr );
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for FrameListener.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour FrameListener.
	*/
	template<>
	struct PtrCacheTraitsT< FrameListener, castor::String >
		: PtrCacheTraitsBaseT< FrameListener, castor::String >
	{
		using ResT = FrameListener;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using FrameListenerCacheTraits = PtrCacheTraitsT< FrameListener, castor::String >;
	using FrameListenerCache = castor::ResourceCacheT< FrameListener
		, castor::String
		, FrameListenerCacheTraits >;

	using FrameListenerRes = FrameListenerCacheTraits::ElementPtrT;
	using FrameListenerResPtr = FrameListenerCacheTraits::ElementObsT;

	CU_DeclareSmartPtr( castor3d, FrameListenerCache, C3D_API );
	/**
	*\~english
	*	Cached resource initialiser, through a CPU frame event.
	*\~french
	*	Initialiseur de ressource cachée, au travers d'un CPU frame event.
	*/
	template< typename CacheT >
	struct CpuEventInitialiserT;
	/**
	*\~english
	*	Cached resource cleaner, through a CPU frame event.
	*\~french
	*	Nettoyeur de ressource cachée, au travers d'un CPU frame event.
	*/
	template< typename CacheT >
	struct CpuEventCleanerT;
	/**
	*\~english
	*	Cached resource initialiser, through a GPU frame event.
	*\~french
	*	Initialiseur de ressource cachée, au travers d'un GPU frame event.
	*/
	template< typename CacheT >
	struct GpuEventInitialiserT;
	/**
	*\~english
	*	Cached resource cleaner, through a GPU frame event.
	*\~french
	*	Nettoyeur de ressource cachée, au travers d'un GPU frame event.
	*/
	template< typename CacheT >
	struct GpuEventCleanerT;

	//@}
	//@}
}

#endif
