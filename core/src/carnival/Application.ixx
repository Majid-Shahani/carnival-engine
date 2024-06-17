module;
#include "macros.h"

export module Application;

namespace Carnival {
	export{
		class CL_API Application {
		public:
			Application();
			virtual ~Application();
			void Run();
		};

		Application* CreateApplication(); // TO BE DEFINED IN THE CLIENT
	}
}