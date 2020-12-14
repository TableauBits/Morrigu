#include "MachaLayer.h"

namespace MRG
{
	class Macha : public Application
	{
	public:
		Macha() : Application("Macha - The Morrigu graphical editor") { pushLayer(new MachaLayer{}); }
		~Macha() {}
	};

	Application* createApplication() { return new Macha(); }
}  // namespace MRG
