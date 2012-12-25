/*!	Singleton class definition
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

namespace scenegraph {

class Singleton {

	public:
		static Singleton* getInstance(void);

	private:
		static Singleton * instance;
		Singleton();
		virtual ~Singleton();
};

} //! namespace scenegraph
#endif //! SINGLETON_H_
