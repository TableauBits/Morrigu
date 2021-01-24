#ifndef MRG_CLASS_LAYER
#define MRG_CLASS_LAYER

#include "Core/Timestep.h"
#include "Events/Event.h"

#include <string>
#include <utility>

namespace MRG
{
	class Layer
	{
	public:
		explicit Layer(std::string name) : m_name(std::move(name)) {}
		Layer(const Layer&) = delete;
		Layer(Layer&&) = delete;
		virtual ~Layer() = default;

		Layer& operator=(const Layer&) = delete;
		Layer& operator=(Layer&&) = delete;

		virtual void onAttach() = 0;
		virtual void onDetach() = 0;
		virtual void onUpdate(Timestep ts) = 0;
		virtual void onEvent(Event& event) = 0;
		virtual void onImGuiRender() {}

		[[nodiscard]] std::string getName() const { return m_name; }

	protected:
		std::string m_name;
	};
}  // namespace MRG

#endif
