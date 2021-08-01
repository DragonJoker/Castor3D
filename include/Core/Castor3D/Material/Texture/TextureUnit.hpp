/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureUnit_H___
#define ___C3D_TextureUnit_H___
#pragma once

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Animation/Animable.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	class TextureUnit
		: public AnimableT< Engine >
	{
	public:
		TextureUnit( TextureUnit const & ) = delete;
		TextureUnit( TextureUnit && ) = default;
		TextureUnit & operator=( TextureUnit const & ) = delete;
		TextureUnit & operator=( TextureUnit && ) = delete;
		/**
		*\~english
		*name
		*	Creators.
		*\~french
		*name
		*	Créateurs.
		*/
		/**@{*/
		C3D_API static TextureUnit create( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, uint32_t size
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		C3D_API static TextureUnit create( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, uint32_t size
			, uint32_t arrayLayers
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		C3D_API static TextureUnit create( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		C3D_API static TextureUnit create( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size
			, uint32_t arrayLayers
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		C3D_API static TextureUnit create( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent3D const & size
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param		engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param		engine	Le moteur.
		 */
		C3D_API explicit TextureUnit( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TextureUnit();
		/**
		 *\~english
		 *\brief		Initialises the texture, id est : fills the image buffer, creates the texture in the render system.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise la texture, id est : remplit le buffer d'image, cree la texture au niveau du renderer.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, QueueData const & queueData );
		/**
		 *\~english
		 *\brief		Cleans up the texture.
		 *\~french
		 *\brief		Nettoie l'objet.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Sets the texture.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Définit la texture.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void setTexture( TextureLayoutSPtr texture );
		/**
		*\~english
		*name
		*	Animation.
		*\~french
		*name
		*	Animation.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief		Creates the animation
		 *\return		The animation
		 *\~french
		 *\brief		Crée l'animation
		 *\return		l'animation
		 */
		C3D_API TextureAnimation & createAnimation();
		/**
		 *\~english
		 *\brief		Removes the animation
		 *\~french
		 *\brief		Retire l'animation
		 */
		C3D_API void removeAnimation();
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		C3D_API TextureAnimation & getAnimation();
		/**
		 *\~english
		 *\return		The animation.
		 *\~french
		 *\return		L'animation.
		 */
		C3D_API TextureAnimation const & getAnimation()const;
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API VkImageType getType()const;
		C3D_API castor::String toString()const;
		C3D_API TextureFlags getFlags()const;
		C3D_API bool isInitialised()const;
		
		TextureLayoutSPtr getTexture()const
		{
			return m_texture;
		}

		TextureConfiguration const & getConfiguration()const
		{
			return m_configuration;
		}

		SamplerSPtr getSampler()const
		{
			return m_sampler.lock();
		}

		bool isTextured()const
		{
			return m_texture != nullptr;
		}

		RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}

		VkWriteDescriptorSet getDescriptor()const
		{
			return m_descriptor;
		}

		uint32_t getId()const
		{
			return m_id;
		}

		bool hasDevice()const
		{
			return m_device != nullptr;
		}

		RenderDevice const & getDevice()const
		{
			CU_Require( hasDevice() );
			return *m_device;
		}

		bool hasAnimation()const
		{
			return m_animated;
		}

		TextureTransform const & getTransform()const
		{
			return m_transform;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		C3D_API void setConfiguration( TextureConfiguration value );
		C3D_API void setTransform( castor::Point3f const & translate
			, castor::Angle const & rotate
			, castor::Point3f const & scale );
		C3D_API void setTransform( TextureTransform const & transform );
		C3D_API void setAnimationTransform( castor::Point3f const & translate
			, castor::Angle const & rotate
			, castor::Point3f const & scale );

		void setRenderTarget( RenderTargetSPtr value )
		{
			m_renderTarget = value;
		}

		void setSampler( SamplerSPtr value )
		{
			m_sampler = value;
		}

		void setOwnSampler( SamplerSPtr value )
		{
			m_ownSampler = value;
			setSampler( m_ownSampler );
		}

		void setId( uint32_t value )
		{
			m_id = value;
		}
		/**@}*/

	public:
		OnTextureUnitChanged onChanged;

	private:
		using AnimableT< Engine >::hasAnimation;
		using AnimableT< Engine >::getAnimation;

		void doUpdateTransform( castor::Point3f const & translate
			, castor::Angle const & rotate
			, castor::Point3f const & scale );

	private:
		friend class TextureRenderer;
		RenderDevice const * m_device{ nullptr };
		TextureConfiguration m_configuration;
		TextureTransform m_transform;
		castor::Matrix4x4f m_transformations;
		TextureLayoutSPtr m_texture;
		RenderTargetWPtr m_renderTarget;
		SamplerWPtr m_sampler;
		SamplerSPtr m_ownSampler;
		ashes::WriteDescriptorSet m_descriptor;
		uint32_t m_id{ 0u };
		mutable bool m_changed;
		castor::String m_name;
		bool m_initialised{ false };
		bool m_animated{ false };
	};
}

#endif
