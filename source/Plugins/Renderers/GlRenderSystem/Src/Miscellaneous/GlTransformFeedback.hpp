/*
See LICENSE file in root folder
*/
#ifndef ___GL_TRANSFORM_FEEDBACK_H___
#define ___GL_TRANSFORM_FEEDBACK_H___

#include <Miscellaneous/TransformFeedback.hpp>

#include "Common/GlBindable.hpp"

namespace GlRender
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
	class GlTransformFeedback
		: public castor3d::TransformFeedback
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
		 *\param[in]	renderSystem	The RenderSystem
		 *\param[in]	m_declaration	The buffer elements declaration.
		 *\param[in]	m_program		The shader program.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	m_declaration	La déclaration des éléments du tampon.
		 *\param[in]	m_program		Le programm shader.
		 */
		GlTransformFeedback( OpenGl & p_gl, GlRenderSystem & renderSystem, castor3d::BufferDeclaration const & p_computed, castor3d::Topology p_topology, castor3d::ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~GlTransformFeedback();

	private:
		/**
		 *\copydoc		castor3d::TransformFeedback::Initialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::TransformFeedback::Cleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::TransformFeedback::Bind
		 */
		void doBind()const override;
		/**
		 *\copydoc		castor3d::TransformFeedback::Unbind
		 */
		void doUnbind()const override;
		/**
		 *\copydoc		castor3d::TransformFeedback::doBegin
		 */
		void doBegin()const override;
		/**
		 *\copydoc		castor3d::TransformFeedback::doEnd
		 */
		void doEnd()const override;

	private:
		GlTopology m_topology;
	};
}

#endif
