/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_TARGET_CACHE_H___
#define ___C3D_TARGET_CACHE_H___

#include "Castor3DPrerequisites.hpp"

#include "HDR/ToneMappingFactory.hpp"
#include "PostEffect/PostEffectFactory.hpp"
#include "Render/RenderInfo.hpp"

namespace Castor3D
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
		: public Castor::OwnedBy< Engine >
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
		C3D_API RenderTargetCache( Engine & engine );
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
		C3D_API RenderTargetSPtr Add( TargetType p_type );
		/**
		 *\~english
		 *\brief		Adds a shadow producing light source.
		 *\param[in]	p_light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse produisant des ombres.
		 *\param[in]	p_light	La source lumineuse.
		 */
		C3D_API void AddShadowProducer( Light & p_light );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	p_target	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	p_target	La cible de rendu
		 */
		C3D_API void Remove( RenderTargetSPtr p_target );
		/**
		 *\~english
		 *\brief			Renders into the render targets.
		 *\param[in,out]	p_vtxCount	Receives the render informations.
		 *\~french
		 *\brief			Dessine dans les cibles de rendu.
		 *\param[in,out]	p_vtxCount	Reçoit les informations de rendu.
		 */
		C3D_API void Render( RenderInfo & p_info );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\return		The ToneMapping factory.
		 *\~french
		 *\return		La fabrique de ToneMapping.
		 */
		inline ToneMappingFactory const & GetToneMappingFactory()const
		{
			return m_toneMappingFactory;
		}
		/**
		 *\~english
		 *\return		The ToneMapping factory.
		 *\~french
		 *\return		La fabrique de ToneMapping.
		 */
		inline ToneMappingFactory & GetToneMappingFactory()
		{
			return m_toneMappingFactory;
		}
		/**
		 *\~english
		 *\return		The PostEffect factory.
		 *\~french
		 *\return		La fabrique de PostEffect.
		 */
		inline PostEffectFactory const & GetPostEffectFactory()const
		{
			return m_postEffectFactory;
		}
		/**
		 *\~english
		 *\return		The PostEffect factory.
		 *\~french
		 *\return		La fabrique de PostEffect.
		 */
		inline PostEffectFactory & GetPostEffectFactory()
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
