#ifndef MRG_CLASS_LAYER
#define MRG_CLASS_LAYER

#include "Core/Timestep.h"
#include "Events/Event.h"

#include <string>

namespace MRG
{
	class Layer
	{
	public:
		Layer(const std::string& name) : m_name(name) {}
		virtual ~Layer() = default;

		virtual void onAttach() = 0;
		virtual void onDetach() = 0;
		virtual void onUpdate(Timestep ts) = 0;
		virtual void onEvent(Event& event) = 0;
		virtual void onImGuiRender() {}

		[[nodiscard]] const std::string getName() const { return m_name; }

	protected:
		std::string m_name;
	};
}  // namespace MRG

#endif
