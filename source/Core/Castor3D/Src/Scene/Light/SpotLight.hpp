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
#ifndef ___C3D_SPOT_LIGHT_H___
#define ___C3D_SPOT_LIGHT_H___

#include "Light.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents a SpotLight
	\remark		A spot light is a light which enlights from a point in a given direction with a conic shape
	\~french
	\brief		Classe de représentation d'une PointLight
	\remark		Une spot light est une source de lumière à un point donné qui illumine dans une direction donnée en suivant une forme cônique
	*/
	class SpotLight
		: public LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		SpotLight loader
		\~french
		\brief		Loader de SpotLight
		*/
		class TextWriter
			: public LightCategory::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( Castor::String const & p_tabs, SpotLight const * p_category = nullptr );
			/**
			 *\~english
			 *\brief		Writes a light into a text file
			 *\param[in]	p_file	The file to save the cameras in
			 *\param[in]	p_light	The light to save
			 *\~french
			 *\brief		Ecrit une lumière dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_light	La lumière
			 */
			C3D_API bool operator()( SpotLight const & p_light, Castor::TextFile & p_file );
			/**
			 *\copydoc		Castor3D::LightCategory::TextWriter::WriteInto
			 */
			C3D_API bool WriteInto( Castor::TextFile & p_file )override;

		private:
			SpotLight const * m_category;
		};

	private:
		friend class Scene;

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_light	La Light parente.
		 */
		C3D_API SpotLight( Light & p_light );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SpotLight();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory.
		 *\return		A light source.
		 *\~french
		 *\brief		Fonction de création utilisée par Factory.
		 *\return		Une source lumineuse.
		 */
		C3D_API static LightCategoryUPtr Create( Light & p_light );
		/**
		 *\copydoc		Castor3D::LightCategory::Update
		 */
		C3D_API void Update( Castor::Point3r const & p_target
			, Viewport & p_viewport
			, int32_t p_index = -1 )override;
		/**
		 *\copydoc		Castor3D::LightCategory::CreateTextWriter
		 */
		C3D_API std::unique_ptr < LightCategory::TextWriter > CreateTextWriter( Castor::String const & p_tabs )override
		{
			return std::make_unique< TextWriter >( p_tabs, this );
		}
		/**
		 *\~english
		 *\brief		Sets attenuation components
		 *\param[in]	p_ptAttenuation	The attenuation components
		 *\~french
		 *\brief		Définit les composantes d'atténuation
		 *\param[in]	p_ptAttenuation	Les composantes d'attenuation
		 */
		C3D_API void SetAttenuation( Castor::Point3f const & p_ptAttenuation );
		/**
		*\~english
		*\brief			Sets the light exponent
		 *\param[in]	p_exponent	The new exponent value
		 */
		C3D_API void SetExponent( float p_exponent );
		/**
		*\~english
		*\brief			Sets the light cutoff
		 *\param[in]	p_cutOff	The new cutoff value
		 */
		C3D_API void SetCutOff( Castor::Angle const & p_cutOff );
		/**
		 *\~english
		 *\return		The light source direction.
		 *\~french
		 *\return		La direction de la source lumineuse.
		 */
		inline Castor::Point3f const & GetDirection()const
		{
			return m_direction;
		}
		/**
		 *\~english
		 *\return		The light space transformation matrix.
		 *\~french
		 *\return		La matrice de transformation de la lumière.
		 */
		inline Castor::Matrix4x4f const & GetLightSpaceTransform()const
		{
			return m_lightSpace;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f const & GetAttenuation()const
		{
			return m_attenuation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f & GetAttenuation()
		{
			return m_attenuation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the exponent value
		 *\return		The exponent value
		 *\~french
		 *\brief		Récupère la valeur de l'exposant
		 *\return		La valeur de l'exposant
		 */
		inline float GetExponent()const
		{
			return m_exponent;
		}
		/**
		 *\~english
		 *\brief		Retrieves the cone angle
		 *\return		The cone angle
		 *\~french
		 *\brief		Récupère l'angle du cône
		 *\return		L'angle du cône
		 */
		inline Castor::Angle const & GetCutOff()const
		{
			return m_cutOff;
		}

	private:
		/**
		 *\copydoc		Castor3D::LightCategory::UpdateNode
		 */
		C3D_API void UpdateNode( SceneNode const & p_node )override;
		/**
		 *\copydoc		Castor::LightCategory::DoBind
		 */
		C3D_API void DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const override;

	private:
		//!\~english	The attenuation components : constant, linear and quadratic.
		//\~french		Les composantes d'attenuation : constante, linéaire et quadratique.
		Castor::Point3f m_attenuation{ 1, 0, 0 };
		//!\~english	The light exponent, id est how much the light is focused.
		//\~french		L'exposant de la lumièrs, càd à quel point elle est concentrée.
		float m_exponent{ 1.0f };
		//!\~english	The angle of the cone.
		//\~french		L'angle du cône.
		Castor::Angle m_cutOff{ 45.0_degrees };
		//!\~english	The light source space transformation matrix.
		//!\~french		La matrice de transformation vers l'espace de la source lumineuse.
		mutable Castor::Matrix4x4f m_lightSpace;
		//!\~english	The light source direction.
		//!\~french		La direction de la source lumineuse.
		Castor::Point3f m_direction;
		//!\~english	The light source shadow map index.
		//!\~french		L'index de la shadow map de la source lumineuse.
		int32_t m_shadowMapIndex{ -1 };
	};
}

#endif
