/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureUnit_H___
#define ___C3D_TextureUnit_H___
#pragma once

#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Render/Texture.hpp"

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Image/Image.hpp>

namespace castor3d
{
	struct TextureData
	{
		explicit TextureData( TextureSourceInfo psourceInfo
			, castor::ImageRPtr pimage = {}
			, VkImageUsageFlags pusage = VkImageUsageFlags{ VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT } )
			: sourceInfo{ std::move( psourceInfo ) }
			, image{ std::move( pimage ) }
			, usage{ pusage }
		{
		}

		TextureSourceInfo sourceInfo;
		castor::ImageRPtr image;
		VkImageUsageFlags usage;
	};

	struct TextureUnitData
	{
		explicit TextureUnitData( TextureData * pbase = {}
			, PassTextureConfig ppassConfig = {}
			, TextureAnimationUPtr panimation = {} )
			: base{ std::move( pbase ) }
			, passConfig{ std::move( ppassConfig ) }
			, animation{ std::move( panimation ) }
		{
		}

		TextureData * base;
		PassTextureConfig passConfig;
		TextureAnimationUPtr animation;
	};

	class TextureUnit
		: public AnimableT< Engine >
	{
	public:
		TextureUnit( TextureUnit const & ) = delete;
		TextureUnit & operator=( TextureUnit const & ) = delete;
		C3D_API TextureUnit( TextureUnit && rhs )noexcept;
		C3D_API TextureUnit & operator=( TextureUnit && rhs )noexcept = delete;
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
		C3D_API ~TextureUnit()noexcept override;
		/**
		 *\~english
		 *\brief		Creates the texture in the render system.
		 *\param[in]	device		The GPU device.
		 *\~french
		 *\brief		Crée la texture au niveau du renderer.
		 *\param[in]	device		Le device GPU.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the texture.
		 *\~french
		 *\brief		Nettoie l'objet.
		 */
		C3D_API void cleanup()noexcept;
		/**
		 *\~english
		 *\brief		Sets the sampler.
		 *\param[in]	sampler	The sampler.
		 *\~french
		 *\brief		Définit le sampler.
		 *\param[in]	sampler	Le sampler.
		 */
		C3D_API void setSampler( SamplerObs sampler );
		/**
		 *\~english
		 *\brief		Sets the texture.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Définit la texture.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void setTexture( Texture const * texture );
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
		*name
		*	Parsers.
		*/
		/**@{*/
		C3D_API static void addParsers( castor::AttributeParsers & result
			, castor::UInt32StrMap const & textureChannels );
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
		C3D_API ashes::Sampler const & getSampler()const;
		C3D_API RenderTargetRPtr getRenderTarget()const;

		C3D_API castor::String getTextureName()const;
		C3D_API castor::Path getTexturePath()const;
		C3D_API bool isTextureStatic()const;
		C3D_API VkFormat getTexturePixelFormat()const;
		C3D_API castor::Point3ui getTextureImageTiles()const;
		C3D_API bool hasTextureImageBuffer()const;
		C3D_API castor::PxBufferBase const & getTextureImageBuffer()const;
		C3D_API VkExtent3D getTextureDimensions()const;
		C3D_API uint32_t getTextureMipmapCount()const;

		TextureConfiguration const & getConfiguration()const noexcept
		{
			return m_configuration;
		}

		bool isTextured()const noexcept
		{
			return m_texture != nullptr;
		}

		bool isRenderTarget()const noexcept
		{
			return m_data.base->sourceInfo.isRenderTarget();
		}

		ashes::WriteDescriptorSet getDescriptor()const noexcept
		{
			return m_descriptor;
		}

		uint32_t getId()const noexcept
		{
			return m_id;
		}

		bool hasDevice()const noexcept
		{
			return m_device != nullptr;
		}

		RenderDevice const & getDevice()const noexcept
		{
			CU_Require( hasDevice() );
			return *m_device;
		}

		bool hasAnimation()const noexcept
		{
			return m_animated || hasAnimation( "Default" );
		}

		TextureTransform const & getTransform()const noexcept
		{
			return m_transform;
		}

		TextureSourceInfo const & getSourceInfo()const noexcept
		{
			return m_data.base->sourceInfo;
		}

		uint32_t getTexcoordSet()const noexcept
		{
			return m_data.passConfig.texcoordSet;
		}

		TextureUnitData & getData()const noexcept
		{
			return m_data;
		}

		castor::Image const & getCPUImage()const noexcept
		{
			CU_Require( m_data.base->image );
			return *m_data.base->image;
		}

		ashes::Image const & getGPUImage()const noexcept
		{
			CU_Require( m_gpuImage );
			return *m_gpuImage;
		}

		bool failed()const noexcept
		{
			return m_failed;
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

		void setId( uint32_t value )
		{
			m_id = value;
			onIdChanged( *this );
		}

		void reportFailure()
		{
			m_failed = true;
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
		TextureConfiguration m_configuration{};
		TextureTransform m_transform{};
		castor::Matrix4x4f m_transformations;
		ashes::Image * m_gpuImage{};
		SamplerObs m_sampler{};
		Texture const * m_texture{};
		ashes::WriteDescriptorSet m_descriptor{ 0u, 0u, 1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
		uint32_t m_id{ 0u };
		castor::String m_name{};
		bool m_initialised{ false };
		bool m_failed{ false };
		bool m_animated{ false };
		bool m_needsUpload{ false };
		uint32_t m_setIndex{};
	};

	class RenderTarget;

	struct PassContext;
	struct RootContext;
	struct SceneContext;

	struct TextureContext
	{
		SceneContext * scene{};
		RootContext * root{};
		PassContext * pass{};

		castor::String name{};
		castor::Path folder{};
		castor::Path relative{};
		castor::ImageRPtr image{};
		TextureConfiguration configuration{};
		RenderTarget * renderTarget{};

		uint32_t mipLevels{ ashes::RemainingArrayLayers };
		uint32_t texcoordSet{};
		TextureAnimationUPtr textureAnimation{};
		SamplerObs sampler{};
		TextureTransform textureTransform{};
	};

	C3D_API Engine * getEngine( TextureContext const & context );

	template< typename ComponentT >
	ComponentT & getPassComponent( TextureContext & context )
	{
		return getPassComponent< ComponentT >( *context.pass );
	}
}

#endif
