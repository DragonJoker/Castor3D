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
#ifndef ___C3D_PqssBuffer_H___
#define ___C3D_PqssBuffer_H___

#include "Material/Pass.hpp"
#include "Texture/TextureUnit.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Base class of a material pass.
	\~french
	\brief		Classe de base d'une passe d'un matériau.
	*/
	class PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_parent	Le matériau parent.
		 */
		C3D_API PassBuffer( Engine & p_engine
			, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PassBuffer();
		/**
		 *\~english
		 *\brief		Adds a pass to the buffer.
		 *\remarks		Sets the pass' ID.
		 *\param[in]	p_pass	The pass.
		 *\~french
		 *\brief		Ajoute une passe au tampon.
		 *\remarks		Définit l'ID de la passe.
		 *\param[in]	p_pass	La passe.
		 */
		C3D_API uint32_t AddPass( Pass & p_pass );
		/**
		 *\~english
		 *\brief		Removes a pass from the buffer.
		 *\~french
		 *\brief		Supprime une pass du tampon.
		 */
		C3D_API void RemovePass( Pass & p_pass );
		/**
		 *\~english
		 *\brief		Updates the passes buffer.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 */
		C3D_API void Update( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Binds the texture buffer.
		 *\~french
		 *\brief		Active le tampon de texture.
		 */
		C3D_API void Bind()const;
		/**
		 *\~english
		 *\brief		Unbinds the texture buffer.
		 *\~french
		 *\brief		Désactive le tampon de texture.
		 */
		C3D_API void Unbind()const;
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		C3D_API void SetComponents( uint32_t p_index
			, uint32_t p_components
			, float p_r
			, float p_g
			, float p_b
			, float p_a );
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		C3D_API void SetComponents( uint32_t p_index
			, uint32_t p_components
			, Castor::Colour const & p_rgb
			, float p_a );
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		C3D_API void SetComponents( uint32_t p_index
			, uint32_t p_components
			, float p_r
			, Castor::Colour const & p_gba );

	private:
		//!\~english	The materials texture.
		//!\~french		La texture contenant les matériaux.
		TextureUnit m_texture;
		//!\~english	The current passes.
		//!\~french		Les passes actuelles.
		std::vector< Pass * > m_passes;
		//!\~english	The current passes.
		//!\~french		Les passes actuelles.
		std::vector< Pass const * > m_dirty;
		//!\~english	The connections to change signal for current passes.
		//!\~french		Les connexions aux signaux de changement des passes actuelles.
		std::vector< Pass::OnChangedConnection > m_connections;
		//!\~english	The pass size.
		//!\~french		La taille d'une passe.
		uint32_t m_passSize;
		//!\~english	The next pass ID.
		//!\~french		L'ID de la passe suivante.
		uint32_t m_passID{ 1u };
		//!\~english	The materials texture buffer.
		//!\~french		Le tampon de la texture contenant les matériaux.
		Castor::PxBufferBaseSPtr m_buffer;
	};
}

#endif
