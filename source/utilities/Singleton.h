//! Singleton.h
/*!
 * Template class for the singleton design pattern.
 *
 * @date	04.01.2013
 * @author	Guido Schmidt
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <assert.h>

template<class T> class Singleton
{
	private:
		Singleton(Singleton const&);
		Singleton& operator=(Singleton const&);
		static T* instance_pt;

	protected:
		Singleton();
		~Singleton();

	public:
		static T* Instance()
		{
			if(!instance_pt)
				instance_pt = new T;
			assert(instance_pt != 0);
			return instance_pt;
		}
};

template <class T> T* Singleton<T>::instance_pt=0;


#endif //! SINGLETON_H_
