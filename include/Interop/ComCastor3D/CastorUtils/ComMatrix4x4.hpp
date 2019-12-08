/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SQUARE_MATRIX_H__
#define __COMC3D_COM_SQUARE_MATRIX_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMatrix object accessible from COM.
	\~french
	\brief		Cette classe définit un CMatrix accessible depuis COM
	*/
	class ATL_NO_VTABLE CMatrix4x4
		: COM_ATL_OBJECT( Matrix4x4 )
		, public castor::Matrix4x4f
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CMatrix4x4();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CMatrix4x4();

		STDMETHOD( Transpose )();
		STDMETHOD( Invert )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Matrix4x4 ), CMatrix4x4 );

	DECLARE_VARIABLE_REF_GETTER( Matrix4x4, castor, Matrix4x4f );
	DECLARE_VARIABLE_REF_PUTTER( Matrix4x4, castor, Matrix4x4f );
	DECLARE_VARIABLE_REF_PARAM_GETTER( Matrix4x4, castor, Matrix4x4f );
}

#endif
