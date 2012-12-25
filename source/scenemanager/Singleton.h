/*!	Singleton class definition
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

namespace scenegraph {

class Singleton {

	private:
		static Singleton * instance;
		Singleton();
		virtual ~Singleton();

	public:
		static Singleton* GetInstance(void);
};

} //! namespace scenegraph
#endif //! SINGLETON_H_
