/*
See LICENSE file in root folder
*/
#ifndef ___GL_RENDER_BUFFER_H___
#define ___GL_RENDER_BUFFER_H___

#include "Common/GlBindable.hpp"

#include <Graphics/Size.hpp>

namespace GlRender
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
	class GlRenderBuffer
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
		GlRenderBuffer( OpenGl & p_gl, GlInternal p_internal, castor3d::RenderBuffer & p_renderBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~GlRenderBuffer();
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
		bool initialise( castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleans the render buffer up.
		 *\~french
		 *\brief		Nettoie le tampon de rendu.
		 */
		void cleanup();
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
		bool resize( castor::Size const & p_size );
		/**
		 *\~english
		 *\return		The OpenGL pixels format.
		 *\~french
		 *\return		Le format OpenGL des pixels.
		 */
		inline GlInternal getInternal()const
		{
			return m_internal;
		}

	private:
		GlInternal m_internal;
		castor::Size m_size;
		castor3d::RenderBuffer & m_renderBuffer;
	};
}

#endif
