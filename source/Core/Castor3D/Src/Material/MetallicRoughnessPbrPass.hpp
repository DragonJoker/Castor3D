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
#ifndef ___C3D_MetallicRoughnessPbrPass_H___
#define ___C3D_MetallicRoughnessPbrPass_H___

#include "Material/Pass.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		13/02/2017
	\~english
	\brief		Definition of a PBR Metallic/Roughness pass.
	\remark		A PBR pass is composed of : colour, roughness, reflectance.
	\~french
	\brief		Définition d'une passe PBR Metallic/Roughness.
	\remark		Une passe PBR est composée de : couleur, rugosité, réflectivité.
	*/
	class MetallicRoughnessPbrPass
		: public Pass
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Pass loader.
		\~french
		\brief Loader de Pass.
		*/
		class TextWriter
			: public Castor::TextWriter< MetallicRoughnessPbrPass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief			Writes a LegacyPass into a text file.
			 *\param[in]		p_pass	The LegacyPass to write.
			 *\param[in,out]	p_file	The file where to write the LegacyPass.
			 *\~french
			 *\brief			Ecrit une LegacyPass dans un fichier texte.
			 *\param[in]		p_pass	La LegacyPass à écrire.
			 *\param[in,out]	p_file	Le file où écrire la LegacyPass.
			 */
			C3D_API bool operator()( MetallicRoughnessPbrPass const & p_pass, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	p_parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		A ne pas utiliser autrement que via la classe Material.
		 *\param[in]	p_parent	Le matériau parent.
		 */
		C3D_API MetallicRoughnessPbrPass( Material & p_parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MetallicRoughnessPbrPass();
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		inline uint32_t GetPassSize()const override
		{
			return 3u;
		}
		/**
		 *\~english
		 *\brief		Sets the albedo colour.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la couleur d'albédo.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAlbedo( Castor::Colour const & p_value )
		{
			m_albedo = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the roughness.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la rugosité.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetRoughness( float p_value )
		{
			m_roughness = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the reflectance.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la réflectivité.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetMetallic( float p_value )
		{
			m_metallic = p_value;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline Castor::Colour const & GetAlbedo()const
		{
			return m_albedo;
		}
		/**
		 *\~english
		 *\return		The roughness.
		 *\~french
		 *\return		La rugosité.
		 */
		inline float GetRoughness()const
		{
			return m_roughness;
		}
		/**
		 *\~english
		 *\return		The reflectance.
		 *\~french
		 *\return		La réflectivité.
		 */
		inline float const & GetMetallic()const
		{
			return m_metallic;
		}

	private:
		/**
		 *\copydoc		Castor3D::Pass::DoInitialise
		 */
		void DoInitialise()override;
		/**
		 *\copydoc		Castor3D::Pass::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::Pass::DoUpdateRenderNode
		 */
		void DoUpdate( PassBuffer & p_buffer )const override;
		/**
		 *\copydoc		Castor3D::Pass::DoSetOpacity
		 */
		void DoSetOpacity( float p_value )override;

	private:
		//!\~english	The albedo colour.
		//!\~french		La couleur d'albédo.
		Castor::Colour m_albedo;
		//!\~english	The roughness.
		//!\~french		La rugosité.
		float m_roughness{ 1.0 };
		//!\~english	The reflectance.
		//!\~french		La réflectivité.
		float m_metallic{ 0.0 };
	};
}

#endif
