/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTargetCache_H___
#define ___C3D_RenderTargetCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class RenderTargetCache
		: public castor::OwnedBy< Engine >
	{
		CU_DeclareVector( RenderTargetUPtr, RenderTarget );
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
		 *\brief		Creates a render target of given type.
		 *\param[in]	type		The render target type.
		 *\param[in]	size		The render target dimensions.
		 *\param[in]	pixelFormat	The render target pixels format.
		 *\return		The render target
		 *\~french
		 *\brief		Crée une cible de rendu du type voulu.
		 *\param[in]	type		Le type de cible de rendu.
		 *\param[in]	size		Les dimensions de la cible de rendu.
		 *\param[in]	pixelFormat	Le format des pixels de la cible de rendu.
		 *\return		La cible de rendu
		 */
		C3D_API RenderTargetRPtr add( TargetType type
			, castor::Size const & size
			, castor::PixelFormat pixelFormat );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	target	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	target	La cible de rendu
		 */
		C3D_API void remove( RenderTarget const * target )noexcept;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Uploads all overlay GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU d'incrustations en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief			Renders into the render targets.
		 *\param[in]		device			The GPU device.
		 *\param[in,out]	info			Receives the render informations.
		 *\param[in]		queue			The queue to which the render commands will be submitted to.
		 *\param[in]		signalsToWait	The semaphores and stages to wait for.
		 *\return			The semaphores and stages that will be signaled.
		 *\~french
		 *\brief			Dessine dans les cibles de rendu.
		 *\param[in]		device			Le device GPU.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 *\param[in]		queue			La queue à laquelle les commandes de rendu seront soumises.
		 *\param[in]		signalsToWait	Les sémaphores et stages à attendre.
		 *\return			Les sémaphores et stages qui seront signalés.
		 */
		C3D_API crg::SemaphoreWaitArray render( RenderDevice const & device
			, RenderInfo & info
			, ashes::Queue const & queue
			, crg::SemaphoreWaitArray signalsToWait );
		/**
		 *\~english
		 *\brief		Cleans up the collection.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie la collection.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		Registers tone mapping display name.
		 *\~french
		 *\brief		Enregistre le nom affiché d'un tone mapping.
		 */
		C3D_API void registerToneMappingName( castor::String const & name
			, castor::StringView fullName );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::String const & getToneMappingName( castor::String const & name )const
		{
			auto it = m_toneMappings.find( name );
			CU_Require( it != m_toneMappings.end() );
			return it->second;
		}
		
		ToneMappingFactory const & getToneMappingFactory()const
		{
			return m_toneMappingFactory;
		}

		ToneMappingFactory & getToneMappingFactory()
		{
			return m_toneMappingFactory;
		}

		PostEffectFactory const & getPostEffectFactory()const
		{
			return m_postEffectFactory;
		}

		PostEffectFactory & getPostEffectFactory()
		{
			return m_postEffectFactory;
		}

		RenderTargetArray const & getRenderTargets( TargetType type )
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
		void lock()const
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
		void unlock()const
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
		void forEach( FuncType func )const
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
		void forEach( FuncType func )
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
		castor::StrStrMap m_toneMappings;
	};
}

#endif
