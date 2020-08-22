#ifndef MRG_MOUSECODES
#define MRG_MOUSECODES

#include <cstdint>

namespace MRG
{
	typedef enum class MouseCode :
	    uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}  // namespace MRG

#define MRG_MOUSE_BUTTON_0 ::MRG::Mouse::Button0
#define MRG_MOUSE_BUTTON_1 ::MRG::Mouse::Button1
#define MRG_MOUSE_BUTTON_2 ::MRG::Mouse::Button2
#define MRG_MOUSE_BUTTON_3 ::MRG::Mouse::Button3
#define MRG_MOUSE_BUTTON_4 ::MRG::Mouse::Button4
#define MRG_MOUSE_BUTTON_5 ::MRG::Mouse::Button5
#define MRG_MOUSE_BUTTON_6 ::MRG::Mouse::Button6
#define MRG_MOUSE_BUTTON_7 ::MRG::Mouse::Button7
#define MRG_MOUSE_BUTTON_LAST ::MRG::Mouse::ButtonLast
#define MRG_MOUSE_BUTTON_LEFT ::MRG::Mouse::ButtonLeft
#define MRG_MOUSE_BUTTON_RIGHT ::MRG::Mouse::ButtonRight
#define MRG_MOUSE_BUTTON_MIDDLE ::MRG::Mouse::ButtonMiddle

#endif