/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryPassResult_H___
#define ___C3D_GeometryPassResult_H___

#include "Technique/Opaque/LightPass.hpp"

#include <Image/Sampler.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.11.0
	\date		17/03/2018
	\~english
	\brief		The result of the geometry pass in deferred rendering.
	\~french
	\brief		Résultat de la passe de géométries dans le rendu différé.
	*/
	class GeometryPassResult
	{
		using Textures = std::array< renderer::Texture const *, size_t( DsTexture::eCount ) >;
		using Views = std::array< renderer::TextureViewPtr, size_t( DsTexture::eCount ) >;
	public:
		/**
		*\~english
		*\brief
		*	Initialises deferred rendering related stuff.
		*\param[in] engine
		*	The engine.
		*\param[in] depthTexture
		*	The depth texture.
		*\param[in] velocityTexture
		*	The velocity texture.
		*\~french
		*\brief
		*	Initialise les données liées au deferred rendering.
		*\param[in] engine
		*	Le moteur.
		*\param[in] depthTexture
		*	La texture de profondeur.
		*\param[in] velocityTexture
		*	La texture de vélocité.
		*/
		C3D_API GeometryPassResult( Engine & engine
			, renderer::Texture const & depthTexture
			, renderer::Texture const & velocityTexture );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline renderer::Texture const & operator[]( size_t index )const
		{
			return *m_result[index];
		}

		inline Textures::const_iterator begin()const
		{
			return m_result.begin();
		}

		inline Textures::const_iterator end()const
		{
			return m_result.end();
		}

		inline Views const & getViews()const
		{
			return m_samplableViews;
		}

		inline renderer::TextureView const & getDepthStencilView()const
		{
			return *m_depthStencilView;
		}

		inline renderer::Sampler const & getSampler()const
		{
			return *m_sampler;
		}
		/**@}*/

	private:
		Engine & m_engine;
		Textures m_result;
		std::vector< renderer::TexturePtr > m_owned;
		Views m_samplableViews;
		renderer::TextureViewPtr m_depthStencilView;
		renderer::SamplerPtr m_sampler;
	};
}

#endif
