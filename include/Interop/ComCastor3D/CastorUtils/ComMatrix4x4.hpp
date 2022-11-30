/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SQUARE_MATRIX_H__
#define __COMC3D_COM_SQUARE_MATRIX_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_EX( Matrix4x4, castor, Matrix4x4f );
	/*!
	\~english
	\brief		This class defines a CMatrix object accessible from COM.
	\~french
	\brief		Cette classe définit un CMatrix accessible depuis COM
	*/
	class CMatrix4x4
		: public CComAtlObject< Matrix4x4, castor::Matrix4x4f >
	{
	public:
		STDMETHOD( Transpose )();
		STDMETHOD( Invert )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Matrix4x4 ), CMatrix4x4 );
}

#endif
