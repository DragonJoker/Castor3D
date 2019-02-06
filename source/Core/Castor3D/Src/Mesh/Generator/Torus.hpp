/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TORUS_H___
#define ___C3D_TORUS_H___

#include "Mesh/MeshGenerator.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The torus representation
	\remark		A torus is an ellipse performing an horizontal ellipse.
				<br />The original ellipse will be called internal and has its own radius and number of subsections
	\~french
	\brief		Représentation d'un tore
	\remark		Un tore est une ellipse qui parcourt une ellipse
				<br />L'ellipse originale sera appelée interne et possède son propre rayon et nombre de sections
	*/
	class Torus
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Torus();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Torus();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();

	private:
		/**
		*\copydoc		castor3d::MeshGenerator::doGenerate
		*/
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		real m_internalRadius;
		real m_externalRadius;
		uint32_t m_internalNbFaces;
		uint32_t m_externalNbFaces;
	};
}

#endif
