#ifndef ___MAP_ITERATOR_H___
#define ___MAP_ITERATOR_H___

namespace CastorShape
{
	template <class T>
    class EMMapIterator
    {
    private:
        typename T::iterator mCurrent;
        typename T::iterator mEnd;
        /// Private constructor since only the parameterised constructor should be used
    public:
        EMMapIterator() {};
        typedef typename T::mapped_type MappedType;
        typedef typename T::key_type KeyType;

        /** Constructor.
        @remarks
            Provide a start and end iterator to initialise.
        */
        EMMapIterator(typename T::iterator start, typename T::iterator end)
            : mCurrent(start), mEnd(end)
        {
        }

        /** Constructor.
        @remarks
            Provide a container to initialise.
        */
        explicit EMMapIterator(T& c)
            : mCurrent(c.begin()), mEnd(c.end())
        {
        }

        /** Returns true if there are more items in the collection. */
        bool hasMoreElements(void) const
        {
            return mCurrent != mEnd;
        }

        /** Returns the next value element in the collection, and advances to the next. */
        typename T::mapped_type getNext(void)
        {
            return (mCurrent++)->second;
        }
        /** Returns the next value element in the collection, without advancing to the next. */
        typename T::mapped_type peekNextValue(void)
        {
            return mCurrent->second;
        }
        /** Returns the next key element in the collection, without advancing to the next. */
        typename T::key_type peekNextKey(void)
        {
            return mCurrent->first;
        }
        /** Required to overcome intermittent bug */
 	    EMMapIterator<T> & operator=( EMMapIterator<T> &rhs )
 	    {
 		    mCurrent = rhs.mCurrent;
 		    mEnd = rhs.mEnd;
 		    return *this;
 	    }
        /** Returns a pointer to the next value element in the collection, without 
            advancing to the next afterwards. */
        typename T::mapped_type* peekNextValuePtr(void)
        {
            return &(mCurrent->second);
        }
        /** Moves the iterator on one element. */
        void moveNext(void)
        {
            ++mCurrent;
        }



    };
}

#endif
