/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GBuffer_H___
#define ___C3D_GBuffer_H___

#include "RenderModule.hpp"

#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

namespace castor3d
{
	class GBuffer
	{
	public:
		using Textures = std::vector< ashes::Image const * >;
		using Views = std::vector< ashes::ImageView >;

	public:
		/**
		*\~english
		*\brief
		*	Initialises g-buffer related stuff.
		*	Given depth buffer will be stored at index 0.
		*\param[in] engine
		*	The engine.
		*\param[in] textures
		*	The gbuffer's images.
		*\~french
		*\brief
		*	Initialise les données liées au g-buffer.
		*	La texture de profondeur donnée sera stockée à l'index 0.
		*\param[in] engine
		*	Le moteur.
		*\param[in] textures
		*	Les images du gbuffer.
		*/
		C3D_API GBuffer( Engine & engine );
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

	protected:
		void doInitialise( Textures textures );

	protected:
		Engine & m_engine;
		Textures m_result;
		Views m_samplableViews;
		ashes::ImageView m_depthStencilView;
		ashes::SamplerPtr m_sampler;
	};
}

#endif
