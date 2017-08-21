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
#ifndef ___C3D_MorphingUbo_H___
#define ___C3D_MorphingUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Matrices Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour les matrices.
	*/
	class MorphingUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API MorphingUbo( MorphingUbo const & ) = delete;
		C3D_API MorphingUbo & operator=( MorphingUbo const & ) = delete;
		C3D_API MorphingUbo( MorphingUbo && ) = default;
		C3D_API MorphingUbo & operator=( MorphingUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API MorphingUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MorphingUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_time	The current time index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_time	L'indice de temps courant.
		 */
		C3D_API void update( float p_time )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		static uint32_t constexpr BindingPoint = 6u;
		//!\~english	Name of the morphing animation frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données d'animation de morphing.
		C3D_API static castor::String const BufferMorphing;
		//!\~english	Name of the morphing time attribute.
		//!\~french		Nom de l'attribut du temps d'animation par sommet.
		C3D_API static castor::String const Time;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The time uniform variable.
		//!\~french		La variable uniforme contenant le temps.
		Uniform1f & m_time;
	};
}

#define UBO_MORPHING( Writer, Flags )\
	glsl::Ubo morphing{ writer, castor3d::MorphingUbo::BufferMorphing, castor3d::MorphingUbo::BindingPoint };\
	auto c3d_time = morphing.declMember< glsl::Float >( castor3d::MorphingUbo::Time, checkFlag( Flags, castor3d::ProgramFlag::eMorphing ) );\
	morphing.end()

#endif
