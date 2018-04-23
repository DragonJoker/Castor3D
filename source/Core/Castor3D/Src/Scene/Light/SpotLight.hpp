/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SPOT_LIGHT_H___
#define ___C3D_SPOT_LIGHT_H___

#include "Light.hpp"

#include <Design/ChangeTracked.hpp>

namespace castor3d
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
			C3D_API TextWriter( castor::String const & tabs, SpotLight const * category = nullptr );
			/**
			 *\~english
			 *\brief		Writes a light into a text file
			 *\param[in]	file	The file to save the cameras in
			 *\param[in]	light	The light to save
			 *\~french
			 *\brief		Ecrit une lumière dans un fichier texte
			 *\param[in]	file	Le fichier
			 *\param[in]	light	La lumière
			 */
			C3D_API bool operator()( SpotLight const & light, castor::TextFile & file );
			/**
			 *\copydoc		castor3d::LightCategory::TextWriter::writeInto
			 */
			C3D_API bool writeInto( castor::TextFile & file )override;

		private:
			SpotLight const * m_category;
		};

	private:
		friend class Scene;

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	light	La Light parente.
		 */
		C3D_API explicit SpotLight( Light & light );

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
		C3D_API static LightCategoryUPtr create( Light & light );
		/**
		 *\~english
		 *\return		The vertices needed to draw the mesh materialising the ligh's volume of effect.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage représentant le volume d'effet de la lumière.
		 */
		C3D_API static castor::Point3fArray const & generateVertices();
		/**
		 *\copydoc		castor3d::LightCategory::update
		 */
		C3D_API void update()override;
		/**
		 *\copydoc		castor3d::LightCategory::updateShadow
		 */
		C3D_API void updateShadow( castor::Point3r const & target
			, Viewport & viewport
			, int32_t index = -1 )override;
		/**
		 *\copydoc		castor3d::LightCategory::createTextWriter
		 */
		C3D_API std::unique_ptr < LightCategory::TextWriter > createTextWriter( castor::String const & tabs )override
		{
			return std::make_unique< TextWriter >( tabs, this );
		}
		/**
		 *\~english
		 *\brief		Sets attenuation components.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit les composantes d'atténuation.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setAttenuation( castor::Point3f const & value );
		/**
		*\~english
		*\brief			Sets the light exponent.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'exposition de la source lumineuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setExponent( float value );
		/**
		*\~english
		*\brief			Sets the light cutoff.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'angle d'ouverture.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setCutOff( castor::Angle const & value );
		/**
		 *\~english
		 *\return		The light source direction.
		 *\~french
		 *\return		La direction de la source lumineuse.
		 */
		inline castor::Point3f const & getDirection()const
		{
			return m_direction;
		}
		/**
		 *\~english
		 *\return		The light space transformation matrix.
		 *\~french
		 *\return		La matrice de transformation de la lumière.
		 */
		inline castor::Matrix4x4f const & getLightSpaceTransform()const
		{
			return m_lightSpace;
		}
		/**
		 *\~english
		 *\return		The attenuation components.
		 *\~french
		 *\return		Les composantes d'attenuation.
		 */
		inline castor::Point3f const & getAttenuation()const
		{
			return m_attenuation.value();
		}
		/**
		 *\~english
		 *\return		The exponent value.
		 *\~french
		 *\return		La valeur de l'exposant.
		 */
		inline float getExponent()const
		{
			return m_exponent;
		}
		/**
		 *\~english
		 *\return		The cone angle.
		 *\~french
		 *\return		L'angle du cône.
		 */
		inline castor::Angle const & getCutOff()const
		{
			return m_cutOff.value();
		}

	private:
		/**
		 *\copydoc		castor3d::LightCategory::updateNode
		 */
		C3D_API void updateNode( SceneNode const & node )override;
		/**
		 *\copydoc		castor::LightCategory::doBind
		 */
		C3D_API void doBind( castor::Point4f * buffer )const override;

	private:
		//!\~english	The attenuation components : constant, linear and quadratic.
		//\~french		Les composantes d'attenuation : constante, linéaire et quadratique.
		castor::ChangeTracked< castor::Point3f > m_attenuation{ castor::Point3f{ 1, 0, 0 } };
		//!\~english	The light exponent, id est how much the light is focused.
		//\~french		L'exposant de la lumièrs, càd à quel point elle est concentrée.
		float m_exponent{ 1.0f };
		//!\~english	The angle of the cone.
		//\~french		L'angle du cône.
		castor::ChangeTracked< castor::Angle > m_cutOff{ 45.0_degrees };
		//!\~english	The light source space transformation matrix.
		//!\~french		La matrice de transformation vers l'espace de la source lumineuse.
		mutable castor::Matrix4x4f m_lightSpace;
		//!\~english	The light source direction.
		//!\~french		La direction de la source lumineuse.
		castor::Point3f m_direction;
	};
}

#endif
