/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_RENDER_BUFFER_H___
#define ___C3DGLES3_RENDER_BUFFER_H___

#include "Common/GlES3Bindable.hpp"

#include <Graphics/Size.hpp>

namespace GlES3Render
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		OpenGL render buffer class.
	\~french
	\brief		Tampon de rendu OpenGL.
	*/
	class GlES3RenderBuffer
		: public Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL api.
		 *\param[in]	p_internal		The OpenGL pixels format.
		 *\param[in]	p_renderBuffer	The parent render buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_gl			L'api OpenGL.
		 *\param[in]	p_internal		Le format OpenGL des pixels.
		 *\param[in]	p_renderBuffer	Le tampon de rendu parent.
		 */
		GlES3RenderBuffer( OpenGlES3 & p_gl, GlES3Internal p_internal, Castor3D::RenderBuffer & p_renderBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~GlES3RenderBuffer();
		/**
		 *\~english
		 *\brief		Initialises the render buffer to given size.
		 *\param[in]	p_size	The size.
		 *\return		\p true on Ok.
		 *\~french
		 *\brief		Initialise le tampon de rendu à la taille donnàe.
		 *\param[in]	p_size	Les dimensions.
		 *\return		\p true si tout s'est bien passà.
		 */
		bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleans the render buffer up.
		 *\~french
		 *\brief		Nettoie le tampon de rendu.
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Resizes the render buffer to given size.
		 *\param[in]	p_size	The size.
		 *\return		\p true on Ok.
		 *\~french
		 *\brief		Redimensionne le tampon de rendu à la taille donnàe.
		 *\param[in]	p_size	Les dimensions.
		 *\return		\p true si tout s'est bien passà.
		 */
		bool Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\return		The OpenGL pixels format.
		 *\~french
		 *\return		Le format OpenGL des pixels.
		 */
		inline GlES3Internal GetInternal()const
		{
			return m_internal;
		}

	private:
		GlES3Internal m_internal;
		Castor::Size m_size;
		Castor3D::RenderBuffer & m_renderBuffer;
	};
}

#endif
