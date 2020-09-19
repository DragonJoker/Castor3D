/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTargetCache_H___
#define ___C3D_RenderTargetCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"

#include <atomic>

namespace castor3d
{
	class RenderTargetCache
		: public castor::OwnedBy< Engine >
	{
		CU_DeclareVector( RenderTargetSPtr, RenderTarget );
		CU_DeclareArray( RenderTargetArray, TargetType::eCount, TargetType );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit RenderTargetCache( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderTargetCache();
		/**
		 *\~english
		 *\brief		Creates a render target of given type
		 *\param[in]	type	The render target type
		 *\return		The render target
		 *\~french
		 *\brief		Crée une cible de rendu du type voulu
		 *\param[in]	type	Le type de cible de rendu
		 *\return		La cible de rendu
		 */
		C3D_API RenderTargetSPtr add( TargetType type );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	target	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	target	La cible de rendu
		 */
		C3D_API void remove( RenderTargetSPtr target );
		/**
		 *\~english
		 *\brief		Updates the render targets.
		 *\~french
		 *\brief		Met à jour les cibles de rendu.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates the render targets.
		 *\~french
		 *\brief		Met à jour les cibles de rendu.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Renders into the render targets.
		 *\param[in,out]	info	Receives the render informations.
		 *\~french
		 *\brief			Dessine dans les cibles de rendu.
		 *\param[in,out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void render( RenderInfo & info );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ToneMappingFactory const & getToneMappingFactory()const
		{
			return m_toneMappingFactory;
		}

		inline ToneMappingFactory & getToneMappingFactory()
		{
			return m_toneMappingFactory;
		}

		inline PostEffectFactory const & getPostEffectFactory()const
		{
			return m_postEffectFactory;
		}

		inline PostEffectFactory & getPostEffectFactory()
		{
			return m_postEffectFactory;
		}

		inline RenderTargetArray const & getRenderTargets( TargetType type )
		{
			CU_Require( m_locked );
			return m_renderTargets[size_t( type )];
		}
		/**@}*/
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const
		{
			m_locked = true;
			m_mutex.lock();
		}
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		inline void unlock()const
		{
			m_mutex.unlock();
			m_locked = false;
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	func	La fonction.
		 */
		template< typename FuncType >
		inline void forEach( FuncType func )const
		{
			auto lock( castor::makeUniqueLock( *this ) );

			for ( auto const & typeTargets : m_renderTargets )
			{
				for ( auto const & target : typeTargets )
				{
					func( *target );
				}
			}
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	func	La fonction.
		 */
		template< typename FuncType >
		inline void forEach( FuncType func )
		{
			auto lock( castor::makeUniqueLock( *this ) );

			for ( auto const & typeTargets : m_renderTargets )
			{
				for ( auto const & target : typeTargets )
				{
					func( *target );
				}
			}
		}

	private:
		mutable std::atomic_bool m_locked;
		TargetTypeArray m_renderTargets;
		mutable std::mutex m_mutex;
		ToneMappingFactory m_toneMappingFactory;
		PostEffectFactory m_postEffectFactory;
	};
}

#endif
