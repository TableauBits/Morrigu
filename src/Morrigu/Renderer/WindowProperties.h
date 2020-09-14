#ifndef MRG_STRUCT_WINDOWPROPERTIES
#define MRG_STRUCT_WINDOWPROPERTIES

#include "Events/Event.h"

#include <functional>

namespace MRG
{
	using EventCallbackFunction = std::function<void(Event&)>;

	class WindowProperties
	{
	public:
		[[nodiscard]] static Scope<WindowProperties>
		create(const std::string& title, unsigned int width, unsigned int height, bool vSync = true);

		std::string title;
		unsigned int width;
		unsigned int height;
		bool VSync;
		EventCallbackFunction callback = [](Event&) {};
		unsigned int keyRepeats = 0;
		
	protected:
		WindowProperties(const std::string& newTitle, unsigned int newWidth, unsigned int newHeight, bool newVSync)
		    : title(newTitle), width(newWidth), height(newHeight), VSync(newVSync)
		{}
	};
}  // namespace MRG

#endif