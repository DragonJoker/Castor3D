template <typename T, size_t Count>
Castor3D::IdPoint<T, Count> :: IdPoint( size_t p_uiIndex)
	:	point()
	,	m_uiIndex( p_uiIndex)
{
}
template <typename T, size_t Count>
Castor3D::IdPoint<T, Count> :: IdPoint( const IdPoint<T, Count> & p_ptPoint, bool p_bLinked)
	:	point( p_ptPoint)
	,	m_uiIndex( p_ptPoint.m_uiIndex)
{
	if (p_bLinked)
	{
		link( p_ptPoint.const_ptr());
	}
}
template <typename T, size_t Count>
template <typename _Ty>
Castor3D::IdPoint<T, Count> :: IdPoint( const _Ty * p_pCoords, size_t p_uiIndex, bool p_bLinked)
	:	point( p_pCoords)
	,	m_uiIndex( p_uiIndex)
{
	if (p_bLinked)
	{
		link( p_pCoords);
	}
}
template <typename T, size_t Count>
Castor3D::IdPoint<T, Count> :: IdPoint( const_point_reference p_ptPoint, size_t p_uiIndex, bool p_bLinked)
	:	point( p_ptPoint)
	,	m_uiIndex( p_uiIndex)
{
	if (p_bLinked)
	{
		link( p_ptPoint.const_ptr());
	}
}
template <typename T, size_t Count>
template <typename _Ty>
Castor3D::IdPoint<T, Count> :: IdPoint( const _Ty & p_a, size_t p_uiIndex)
	:	point( p_a)
	,	m_uiIndex( p_uiIndex)
{
}
template <typename T, size_t Count>
template <typename _Ty>
Castor3D::IdPoint<T, Count> :: IdPoint( const _Ty & p_a, const _Ty & p_b, size_t p_uiIndex)
	:	point( p_a, p_b)
	,	m_uiIndex( p_uiIndex)
{
}
template <typename T, size_t Count>
template <typename _Ty>
Castor3D::IdPoint<T, Count> :: IdPoint( const _Ty & p_a, const _Ty & p_b, const _Ty & p_c, size_t p_uiIndex)
	:	point( p_a, p_b, p_c)
	,	m_uiIndex( p_uiIndex)
{
}
template <typename T, size_t Count>
template <typename _Ty>
Castor3D::IdPoint<T, Count> :: IdPoint( const _Ty & p_a, const _Ty & p_b, const _Ty & p_c, const _Ty & p_d, size_t p_uiIndex)
	:	point( p_a, p_b, p_c, p_d)
	,	m_uiIndex( p_uiIndex)
{
}
template <typename T, size_t Count>
template <typename _Ty, size_t _Count>
Castor3D::IdPoint<T, Count> :: IdPoint( const Point<_Ty, _Count> & p_ptPoint, size_t p_uiIndex, bool p_bLinked)
	:	point( p_ptPoint)
	,	m_uiIndex( p_uiIndex)
{
	if (p_bLinked)
	{
		link( p_ptPoint.const_ptr());
	}
}
template <typename T, size_t Count>
Castor3D::IdPoint<T, Count> :: ~IdPoint()
{
}
template <typename T, size_t Count>
inline Castor3D::IdPoint<T, Count> & IdPoint<T, Count> :: operator =( const IdPoint<T, Count> & p_toCopy)
{
	Point<T, Count>::operator =( p_toCopy);
	m_uiIndex = p_toCopy.m_uiIndex;
	return * this;
}
template <typename T, size_t Count>
void Castor3D::IdPoint<T, Count> :: _createSerialiseMap()
{
	for (size_t i = 0 ; i < Count ; i++)
	{
		Serialisable::AddElement( & Point<T, Count>::at( i));
	}
}
