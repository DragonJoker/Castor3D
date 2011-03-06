template <> struct BufferElement::ElementCopier<real, 1>
{
	Point<real, 1> * operator ()( const Point<real, 1> & p_ptPoint)
	{
		return new Point<real, 1>( p_ptPoint);
	}
};

template <> struct BufferElement::ElementCopier<real, 2>
{
	Point<real, 2> * operator ()( const Point<real, 2> & p_ptPoint)
	{
		return new Point<real, 2>( p_ptPoint);
	}
};

template <> struct BufferElement::ElementCopier<real, 3>
{
	Point<real, 3> * operator ()( const Point<real, 3> & p_ptPoint)
	{
		return new Point<real, 3>( p_ptPoint);
	}
};

template <> struct BufferElement::ElementCopier<real, 4>
{
	Point<unsigned char, 4> * operator ()( const Point<unsigned char, 4> & p_ptPoint)
	{
		return new Point<unsigned char, 4>( p_ptPoint);
	}
};