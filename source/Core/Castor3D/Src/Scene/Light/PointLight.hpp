/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POINT_LIGHT_H___
#define ___C3D_POINT_LIGHT_H___

#include "Light.hpp"

#include <Design/ChangeTracked.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents a PointLight
	\remark		A point light is a light which enlights from a point in all directions.
	\~french
	\brief		Classe de représentation d'une PointLight
	\remark		Une point light est une source de lumière à un point donné qui illumine dans toutes les directions
	*/
	class PointLight
		: public LightCategory
	{
	public:
		/*!
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
			C3D_API TextWriter( castor::String const & p_tabs, PointLight const * p_category = nullptr );
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
			C3D_API bool operator()( PointLight const & p_light, castor::TextFile & p_file );
			/**
			 *\copydoc		castor3d::LightCategory::TextWriter::writeInto
			 */
			C3D_API bool writeInto( castor::TextFile & p_file )override;

		private:
			PointLight const * m_category;
		};

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_light	La Light parente.
		 */
		C3D_API PointLight( Light & p_light );

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
		C3D_API static LightCategoryUPtr create( Light & p_light );
		/**
		 *\~english
		 *\return		The vertices needed to draw the mesh materialising the ligh's volume of effect.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage représentant le volume d'effet de la lumière.
		 */
		C3D_API static castor::Point3fArray const & generateVertices();
		/**
		 *\copydoc		castor3d::LightCategory::Update
		 */
		C3D_API void update( castor::Point3r const & p_target
			, Viewport & p_viewport
			, int32_t p_index = -1 )override;
		/**
		 *\copydoc		castor3d::LightCategory::createTextWriter
		 */
		C3D_API std::unique_ptr < LightCategory::TextWriter > createTextWriter( castor::String const & p_tabs )override
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
		C3D_API void setAttenuation( castor::Point3f const & p_ptAttenuation );
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
		C3D_API void updateNode( SceneNode const & p_node )override;
		/**
		 *\copydoc		castor::LightCategory::doBind
		 */
		C3D_API void doBind( castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const override;

	private:
		friend class Scene;
		//!\~english	The attenuation components : constant, linear and quadratic.
		//!\~french		Les composantes d'attenuation : constante, linéaire et quadratique.
		castor::ChangeTracked< castor::Point3f > m_attenuation{ castor::Point3f{ 1.0f, 0.0f, 0.0f } };
		//!\~english	The light source shadow map index.
		//!\~french		L'index de la shadow map de la source lumineuse.
		int32_t m_shadowMapIndex{ -1 };
	};
}

#endif
