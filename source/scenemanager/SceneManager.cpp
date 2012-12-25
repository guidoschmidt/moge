/*!	Scene manager class implementation
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#include "SceneManager.h"

namespace scenegraph {

SceneManager::SceneManager() {
	//! TODO Auto-generated constructor stub

}

SceneManager::~SceneManager() {
	//! TODO Auto-generated destructor stub
}

//! Get scenemanager singleton instance
/*!
 *
 * @return
 */
static Singleton* SceneManager::GetInstance(){
	if(instance == 0){
		instance = new SceneManager();
	}
	return instance;
}

} //! namespace scenegraph
