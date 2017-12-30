/* See LICENSE file in root folder */
#ifndef ___TRS_TRANSFORM_FEEDBACK_H___
#define ___TRS_TRANSFORM_FEEDBACK_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Miscellaneous/TransformFeedback.hpp>

namespace TestRender
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
	class TestTransformFeedback
		: public castor3d::TransformFeedback
	{
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
		TestTransformFeedback( TestRenderSystem & p_renderSystem, castor3d::BufferDeclaration const & p_computed, castor3d::Topology p_topology, castor3d::ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~TestTransformFeedback();

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
	};
}

#endif
