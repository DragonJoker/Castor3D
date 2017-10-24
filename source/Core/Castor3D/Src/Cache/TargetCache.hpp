/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TARGET_CACHE_H___
#define ___C3D_TARGET_CACHE_H___

#include "Castor3DPrerequisites.hpp"

#include "HDR/ToneMappingFactory.hpp"
#include "PostEffect/PostEffectFactory.hpp"
#include "Render/RenderInfo.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		RenderTarget cache.
	\~french
	\brief		Cache de RenderTarget.
	*/
	class RenderTargetCache
		: public castor::OwnedBy< Engine >
	{
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
		 *\param[in]	p_type	The render target type
		 *\return		The render target
		 *\~french
		 *\brief		Crée une cible de rendu du type voulu
		 *\param[in]	p_type	Le type de cible de rendu
		 *\return		La cible de rendu
		 */
		C3D_API RenderTargetSPtr add( TargetType p_type );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	p_target	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	p_target	La cible de rendu
		 */
		C3D_API void remove( RenderTargetSPtr p_target );
		/**
		 *\~english
		 *\brief			Renders into the render targets.
		 *\param[in,out]	p_vtxCount	Receives the render informations.
		 *\~french
		 *\brief			Dessine dans les cibles de rendu.
		 *\param[in,out]	p_vtxCount	Reçoit les informations de rendu.
		 */
		C3D_API void render( RenderInfo & p_info );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\return		The ToneMapping factory.
		 *\~french
		 *\return		La fabrique de ToneMapping.
		 */
		inline ToneMappingFactory const & getToneMappingFactory()const
		{
			return m_toneMappingFactory;
		}
		/**
		 *\~english
		 *\return		The ToneMapping factory.
		 *\~french
		 *\return		La fabrique de ToneMapping.
		 */
		inline ToneMappingFactory & getToneMappingFactory()
		{
			return m_toneMappingFactory;
		}
		/**
		 *\~english
		 *\return		The PostEffect factory.
		 *\~french
		 *\return		La fabrique de PostEffect.
		 */
		inline PostEffectFactory const & getPostEffectFactory()const
		{
			return m_postEffectFactory;
		}
		/**
		 *\~english
		 *\return		The PostEffect factory.
		 *\~french
		 *\return		La fabrique de PostEffect.
		 */
		inline PostEffectFactory & getPostEffectFactory()
		{
			return m_postEffectFactory;
		}
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const
		{
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
		}

	private:
		DECLARE_VECTOR( RenderTargetSPtr, RenderTarget );
		DECLARE_ARRAY( RenderTargetArray, TargetType::eCount, TargetType );
		//!\~english	The render targets sorted by target type.
		//!\~french		Les cibles de rendu, triées par type de cible de rendu.
		TargetTypeArray m_renderTargets;
		//!\~english	The mutex protecting the render targets array.
		//!\~french		Le mutex protégeant le tableau de cibles de rendu.
		mutable std::mutex m_mutex;
		//!\~english	The tone mapping factory.
		//!\~french		La fabrique de mappage de tons.
		ToneMappingFactory m_toneMappingFactory;
		//!\~english	The post effect factory.
		//!\~french		La fabrique d'effets post rendu.
		PostEffectFactory m_postEffectFactory;
	};
}

#endif
