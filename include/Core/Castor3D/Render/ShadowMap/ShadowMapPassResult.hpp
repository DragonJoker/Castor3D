/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapPassResult_H___
#define ___C3D_ShadowMapPassResult_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"

namespace castor3d
{
	class ShadowMapPassResult
	{
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
		C3D_API ShadowMapPassResult( Engine & engine
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline TextureUnit const & operator[]( SmTexture index )const
		{
			return m_owned[size_t( index )];
		}
		
		inline TextureUnit & operator[]( SmTexture index )
		{
			return m_owned[size_t( index )];
		}

		inline TextureUnitArray::const_iterator begin()const
		{
			return m_owned.begin();
		}

		inline TextureUnitArray::const_iterator end()const
		{
			return m_owned.end();
		}

		inline TextureUnitArray::iterator begin()
		{
			return m_owned.begin();
		}

		inline TextureUnitArray::iterator end()
		{
			return m_owned.end();
		}
		/**@}*/

	private:
		TextureUnitArray m_owned;
	};
}

#endif
