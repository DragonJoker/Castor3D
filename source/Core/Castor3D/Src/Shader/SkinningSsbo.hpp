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
#ifndef ___C3D_SkinningSsbo_H___
#define ___C3D_SkinningSsbo_H___

#include "ShaderStorageBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Skinning SSBO management.
	\~french
	\brief		Gestion du tampon shader pour le skinning.
	*/
	class SkinningSsbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API SkinningSsbo( SkinningSsbo const & ) = delete;
		C3D_API SkinningSsbo & operator=( SkinningSsbo const & ) = delete;
		C3D_API SkinningSsbo( SkinningSsbo && ) = default;
		C3D_API SkinningSsbo & operator=( SkinningSsbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API SkinningSsbo( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkinningSsbo();
		/**
		 *\~english
		 *\brief		Updates the SSBO from given values.
		 *\param[in]	p_skeleton	The skeleton.
		 *\param[in]	p_index		The instance index.
		 *\~french
		 *\brief		Met à jour le SSBO avec les valeurs données.
		 *\param[in]	p_skeleton	Le squelette.
		 *\param[in]	p_index		L'indice de l'instance.
		 */
		C3D_API void Update( AnimatedSkeleton const & p_skeleton
			, uint32_t p_index )const;
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Getters.
		 */
		/**@{*/
		inline ShaderStorageBuffer & GetSsbo()
		{
			return m_ssbo;
		}

		inline ShaderStorageBuffer const & GetSsbo()const
		{
			return m_ssbo;
		}
		/**@}*/

	private:
		//!\~english	The SSBO.
		//!\~french		L'SSBO.
		ShaderStorageBuffer m_ssbo;
		//!\~english	The bones matrices uniform variable.
		//!\~french		Le variable uniforme contenant les matrices des os.
		std::vector< Castor::Matrix4x4f > m_bonesMatrix;
	};
}

#endif
