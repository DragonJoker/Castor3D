/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_TRANSFORM_FEEDBACK_H___
#define ___C3DGLES3_TRANSFORM_FEEDBACK_H___

#include <Miscellaneous/TransformFeedback.hpp>

#include "Common/GlES3Bindable.hpp"

namespace GlES3Render
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.9.0
	\date 		19/10/2016
	\~english
	\brief		Transform feedback implementation.
	\~french
	\brief		Implémentation du Transform feedback.
	*/
	class GlES3TransformFeedback
		: public Castor3D::TransformFeedback
		, public Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem
		 *\param[in]	m_declaration	The buffer elements declaration.
		 *\param[in]	m_program		The shader program.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	m_declaration	La déclaration des éléments du tampon.
		 *\param[in]	m_program		Le programm shader.
		 */
		GlES3TransformFeedback( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem, Castor3D::BufferDeclaration const & p_computed, Castor3D::Topology p_topology, Castor3D::ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~GlES3TransformFeedback();

	private:
		/**
		 *\copydoc		Castor3D::TransformFeedback::Initialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::Cleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::Bind
		 */
		void DoBind()const override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::Unbind
		 */
		void DoUnbind()const override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::DoBegin
		 */
		void DoBegin()const override;
		/**
		 *\copydoc		Castor3D::TransformFeedback::DoEnd
		 */
		void DoEnd()const override;

	private:
		GlES3Topology m_topology;
	};
}

#endif
