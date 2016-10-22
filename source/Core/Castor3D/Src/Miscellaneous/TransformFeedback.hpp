/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_TRANSFORM_FEEDBACK_H___
#define ___C3D_TRANSFORM_FEEDBACK_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

#include <cstddef>

namespace Castor3D
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
	class TransformFeedback
		: Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem
		 *\param[in]	p_computed		The computed elements description.
		 *\param[in]	p_topology		The primitives type.
		 *\param[in]	p_program		The shader program.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_computed		La description des éléments calculés.
		 *\param[in]	p_topology		Le type de primitives.
		 *\param[in]	p_program		Le programm shader.
		 */
		C3D_API TransformFeedback( RenderSystem & p_renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TransformFeedback();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	p_buffers	The vertex buffers.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	p_buffers	Les tampons de sommets.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( VertexBufferArray const & p_buffers );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Bind()const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		C3D_API void Unbind()const;
		/**
		 *\~english
		 *\return		The primitives written at last call.
		 *\~french
		 *\brief		Les primitives écrites au dernier appel.
		 */
		inline uint32_t GetWrittenPrimitives()const
		{
			return m_writtenPrimitives;
		}

	private:
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoBind()const = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		C3D_API virtual void DoUnbind()const = 0;
		/**
		 *\~english
		 *\brief		Begins transform feedback.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Démarre le transform feedback.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoBegin()const = 0;
		/**
		 *\~english
		 *\brief		Ends transform feedback.
		 *\~french
		 *\brief		Termine le transform feedback.
		 */
		C3D_API virtual void DoEnd()const = 0;

	protected:
		//!\~english	The shader program.
		//!\~french		Le programm shader.
		ShaderProgram & m_program;
		//!\~english	The computed elements description.
		//!\~french		La description des éléments calculés.
		BufferDeclaration m_computed;
		//!\~english	The attached buffers.
		//!\~french		Les tampons attachés.
		VertexBufferArray m_buffers;
		//!\~english	The primitives written GPU query.
		//!\~french		La requête GPU de primitives écrites.
		GpuQueryUPtr m_query;
		//!\~english	The written primitives count.
		//!\~french		Le nomnbre de primitives écrites.
		mutable uint32_t m_writtenPrimitives{ 1u };
		//!\~english	Tells if the next call to Draw is the first one.
		//!\~french		Dit si le prochain appel à Draw est le premier.
		bool m_firstDraw{ true };
	};
}

#endif
