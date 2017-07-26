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
#ifndef ___C3D_PROGRAM_INPUT_LAYOUT_H___
#define ___C3D_PROGRAM_INPUT_LAYOUT_H___

#include "Castor3DPrerequisites.hpp"

#include "Mesh/Buffer/BufferElementDeclaration.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		03/02/2016
	\~english
	\brief		ProgramInputLayout class.
	\remark		Holds the vertex buffer declaration, initialised by the shader program when compiled.
	\~french
	\brief		Classe ProgramInputLayout.
	\remark		Contient la déclaration des éléments du tampon de sommets, initialisé par le programme shader à la compilation.
	*/
	class ProgramInputLayout
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API explicit ProgramInputLayout( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ProgramInputLayout();
		/**
		 *\~english
		 *\brief		Initialisation function, on GPU side.
		 *\param[in]	p_program	The shader program used to fill this layout.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation, côté GPU.
		 *\param[in]	p_program	Le programme utilisé pour remplir ce layout.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool Initialise( ShaderProgram const & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function, on GPU side.
		 *\~french
		 *\brief		Fonction de nettoyage côté GPU.
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\return		An iterator to the beginning of the layout elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments du layout.
		 */
		inline std::vector< BufferElementDeclaration >::const_iterator begin()const
		{
			return m_layout.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the layout elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments du layout.
		 */
		inline std::vector< BufferElementDeclaration >::const_iterator end()const
		{
			return m_layout.end();
		}

	protected:
		/**
		 *\~english
		 *\brief		Adds an attribute to the layout.
		 *\param[in]	p_name		Associated variable name.
		 *\param[in]	p_type		Element type.
		 *\param[in]	p_offset	Offset in the stream.
		 *\~french
		 *\brief		Ajoute un attribut au layout.
		 *\param[in]	p_name		Nom de la variable associée.
		 *\param[in]	p_type		Type de l'élément.
		 *\param[in]	p_offset	Offset dans le tampon.
		 */
		C3D_API void DoAddAttribute( Castor::String const & p_name, ElementType p_type, uint32_t p_offset = 0 );
		/**
		 *\~english
		 *\brief		Tries to guess possible usage for attribute, given its name and type.
		 *\param[in]	p_name		Associated variable name.
		 *\param[in]	p_type		Element type.
		 *\return		The possible usages.
		 *\~french
		 *\brief		Essaie de deviner l'utilisation d'un attribut, en fonction de son nom et de son type.
		 *\param[in]	p_name		Nom de la variable associée.
		 *\param[in]	p_type		Type de l'élément.
		 *\return		Les utilisations possibles.
		 */
		Castor::FlagCombination< ElementUsage > DoGuessUsages( Castor::String const & p_name, ElementType p_type );

	protected:
		friend C3D_API bool operator==( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs );
		//!\~english Buffer elements description.	\~french Description des élément du tampon.
		std::vector< BufferElementDeclaration > m_layout;
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator==( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs );
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator!=( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs );
}

#endif
