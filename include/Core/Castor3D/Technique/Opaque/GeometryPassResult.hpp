/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryPassResult_H___
#define ___C3D_GeometryPassResult_H___

#include "Castor3D/Technique/Opaque/LightPass.hpp"

#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

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
		using Textures = std::array< ashes::Image const *, size_t( DsTexture::eCount ) >;
		using Views = std::array< ashes::ImageView, size_t( DsTexture::eCount ) >;
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
			, ashes::Image const & depthTexture
			, ashes::Image const & velocityTexture );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::Image const & operator[]( size_t index )const
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

		inline ashes::ImageView const & getDepthStencilView()const
		{
			return m_depthStencilView;
		}

		inline ashes::Sampler const & getSampler()const
		{
			return *m_sampler;
		}
		/**@}*/

	private:
		Engine & m_engine;
		Textures m_result;
		std::vector< ashes::ImagePtr > m_owned;
		Views m_samplableViews;
		ashes::ImageView m_depthStencilView;
		ashes::SamplerPtr m_sampler;
	};
}

#endif
