#ifndef MRG_STRUCT_WINDOWPROPERTIES
#define MRG_STRUCT_WINDOWPROPERTIES

#include "Events/Event.h"

#include <cstdint>
#include <functional>

namespace MRG
{
	using EventCallbackFunction = std::function<void(Event&)>;

	class WindowProperties
	{
	public:
		[[nodiscard]] static Scope<WindowProperties> create(const char* title, uint32_t width, uint32_t height, bool vSync = true);

		const char* title;
		uint32_t width;
		uint32_t height;
		bool VSync;
		EventCallbackFunction callback = [](Event&) {};
		uint16_t keyRepeats = 0;

	protected:
		WindowProperties(const char*& newTitle, uint32_t newWidth, uint32_t newHeight, bool newVSync)
		    : title(newTitle), width(newWidth), height(newHeight), VSync(newVSync)
		{}
	};
}  // namespace MRG

#endif
