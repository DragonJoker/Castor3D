/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureUnit_H___
#define ___C3D_TextureUnit_H___
#pragma once

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	class TextureUnit
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		TextureUnit loader.
		\~french
		\brief		Loader de TextureUnit.
		*/
		class TextWriter
			: public castor::TextWriter< TextureUnit >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs
				, MaterialType type );
			/**
			 *\~english
			 *\brief		Writes a TextureUnit into a text file.
			 *\param[in]	file	The file.
			 *\param[in]	unit	The TextureUnit.
			 *\~french
			 *\brief		Ecrit une TextureUnit dans un fichier texte.
			 *\param[in]	file	Le fichier.
			 *\param[in]	unit	La TextureUnit.
			 */
			C3D_API bool operator()( TextureUnit const & unit, castor::TextFile & file )override;

		private:
			MaterialType m_type;
		};

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
			, castor::String const & name
			, VkFormat format
			, uint32_t size
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		C3D_API static TextureUnit create( Engine & engine
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags );
		C3D_API static TextureUnit create( Engine & engine
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
		 *\~french
		 *\brief		Initialise la texture, id est : remplit le buffer d'image, cree la texture au niveau du renderer.
		 */
		C3D_API bool initialise();
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
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API VkImageType getType()const;
		C3D_API castor::String toString()const;

		inline TextureLayoutSPtr getTexture()const
		{
			return m_texture;
		}

		inline TextureConfiguration const & getConfiguration()const
		{
			return m_configuration;
		}

		inline SamplerSPtr getSampler()const
		{
			return m_sampler.lock();
		}

		inline bool isTextured()const
		{
			return m_texture != nullptr;
		}

		inline bool getAutoMipmaps()const
		{
			return m_autoMipmaps;
		}

		inline RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}

		inline VkWriteDescriptorSet getDescriptor()const
		{
			return m_descriptor;
		}

		inline uint32_t getId()const
		{
			return m_id;
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

		inline void setRenderTarget( RenderTargetSPtr value )
		{
			m_renderTarget = value;
		}

		inline void setSampler( SamplerSPtr value )
		{
			m_sampler = value;
		}

		inline void setAutoMipmaps( bool value )
		{
			m_autoMipmaps = value;
		}

		inline void setId( uint32_t value )
		{
			m_id = value;
		}
		/**@}*/

	public:
		OnTextureUnitChanged onChanged;

	private:
		friend class TextureRenderer;
		TextureConfiguration m_configuration;
		castor::Matrix4x4f m_transformations;
		TextureLayoutSPtr m_texture;
		RenderTargetWPtr m_renderTarget;
		SamplerWPtr m_sampler;
		bool m_autoMipmaps;
		ashes::WriteDescriptorSet m_descriptor;
		uint32_t m_id{ 0u };
		mutable bool m_changed;
	};
}

#endif
