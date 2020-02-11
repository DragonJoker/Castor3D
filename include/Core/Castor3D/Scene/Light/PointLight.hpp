/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POINT_LIGHT_H___
#define ___C3D_POINT_LIGHT_H___

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>

namespace castor3d
{
	class PointLight
		: public LightCategory
	{
	public:
		/**
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		PointLight loader
		\~french
		\brief		Loader de PointLight
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
			C3D_API TextWriter( castor::String const & tabs, PointLight const * category = nullptr );
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
			C3D_API bool operator()( PointLight const & light, castor::TextFile & file );
			/**
			 *\copydoc		castor3d::LightCategory::TextWriter::writeInto
			 */
			C3D_API bool writeInto( castor::TextFile & file )override;

		private:
			PointLight const * m_category;
		};

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	light	La Light parente.
		 */
		C3D_API explicit PointLight( Light & light );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PointLight();
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
		 *\~english
		 *\brief			Updates the shadow informations.
		 *\param[in,out]	viewport	The viewport that receives the far plane value.
		 *\param[in]		index		The shadow map index.
		 *\~french
		 *\brief			Met à jour les information d'ombre.
		 *\param[in,out]	viewport	Le viewport qui reçoit la valeur du far plane.
		 *\param[in]		index		L'indice de la shadow map.
		 */
		C3D_API void updateShadow( Viewport & viewport
			, int32_t index = -1 );
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
		 *\return		The attenuation components.
		 *\~french
		 *\return		Les composantes d'attenuation.
		 */
		inline castor::Point3f const & getAttenuation()const
		{
			return m_attenuation.value();
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
		friend class Scene;
		//!\~english	The attenuation components : constant, linear and quadratic.
		//!\~french		Les composantes d'attenuation : constante, linéaire et quadratique.
		castor::ChangeTracked< castor::Point3f > m_attenuation{ castor::Point3f{ 1.0f, 0.0f, 0.0f } };
	};
}

#endif
