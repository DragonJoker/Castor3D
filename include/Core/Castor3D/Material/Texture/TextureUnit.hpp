/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureUnit_H___
#define ___C3D_TextureUnit_H___
#pragma once

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	struct TextureUnitData
	{
		TextureSourceInfo sourceInfo;
		PassTextureConfig passConfig{};
		TextureAnimationUPtr animation{};
		castor::PxBufferBaseUPtr buffer{};
	};

	class TextureUnit
		: public AnimableT< Engine >
	{
	public:
		TextureUnit( TextureUnit const & ) = delete;
		TextureUnit & operator=( TextureUnit const & ) = delete;
		C3D_API TextureUnit( TextureUnit && rhs );
		C3D_API TextureUnit & operator=( TextureUnit && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param		engine	The engine.
		 *\param		data	The texture informations.
		 *\~french
		 *\brief		Constructeur.
		 *\param		engine	Le moteur.
		 *\param		data	Les données de la texture.
		 */
		C3D_API explicit TextureUnit( Engine & engine
			, TextureUnitData & data );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TextureUnit()override;
		/**
		 *\~english
		 *\brief		Initialises the texture, id est : fills the image buffer, creates the texture in the render system.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Initialise la texture, id est : remplit le buffer d'image, cree la texture au niveau du renderer.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
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
		C3D_API void setTexture( TextureLayoutUPtr texture );
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
		C3D_API TextureFlagsSet getFlags()const;
		C3D_API bool isInitialised()const;
		C3D_API bool isTransformAnimated()const;
		C3D_API bool isTileAnimated()const;
		
		TextureLayoutRPtr getTexture()const
		{
			return m_texture.get();
		}

		TextureConfiguration const & getConfiguration()const
		{
			return m_configuration;
		}

		SamplerObs getSampler()const
		{
			return m_sampler;
		}

		bool isTextured()const
		{
			return m_texture != nullptr;
		}

		RenderTargetRPtr getRenderTarget()const
		{
			return m_renderTarget;
		}

		ashes::WriteDescriptorSet getDescriptor()const
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
			return m_animated || hasAnimation( "Default" );
		}

		TextureTransform const & getTransform()const
		{
			return m_transform;
		}

		TextureSourceInfo const & getSourceInfo()const
		{
			return m_data.sourceInfo;
		}

		uint32_t getTexcoordSet()const
		{
			return m_data.passConfig.texcoordSet;
		}

		TextureUnitData & getData()const
		{
			return m_data;
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
		C3D_API void setTexcoordSet( uint32_t value );

		void setRenderTarget( RenderTargetRPtr value )
		{
			m_renderTarget = value;
		}

		void setSampler( SamplerObs value )
		{
			m_sampler = value;
		}

		void setId( uint32_t value )
		{
			m_id = value;
			onIdChanged( *this );
		}
		/**@}*/

	public:
		OnTextureUnitChanged onChanged;
		OnTextureUnitChanged onIdChanged;

	private:
		using AnimableT< Engine >::hasAnimation;
		using AnimableT< Engine >::getAnimation;

		void doUpdateTransform( castor::Point3f const & translate
			, castor::Angle const & rotate
			, castor::Point3f const & scale );

	private:
		friend class TextureRenderer;
		TextureUnitData & m_data;
		RenderDevice const * m_device{ nullptr };
		TextureConfiguration m_configuration;
		TextureTransform m_transform;
		castor::Matrix4x4f m_transformations;
		TextureLayoutUPtr m_texture;
		RenderTargetRPtr m_renderTarget{};
		SamplerObs m_sampler{};
		ashes::WriteDescriptorSet m_descriptor;
		uint32_t m_id{ 0u };
		mutable bool m_changed;
		castor::String m_name;
		bool m_initialised{ false };
		bool m_animated{ false };
		uint32_t m_setIndex{};
	};
}

#endif
