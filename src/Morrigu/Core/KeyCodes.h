#ifndef MRG_KEYCODES
#define MEG_KEYCODES

#include <cstdint>
#include <ostream>

namespace MRG
{
	typedef enum class KeyCode :
	    uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44,      /* , */
		Minus = 45,      /* - */
		Period = 46,     /* . */
		Slash = 47,      /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61,     /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,    /* \ */
		RightBracket = 93, /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}  // namespace MRG

#define MRG_KEY_SPACE ::MRG::Key::Space
#define MRG_KEY_APOSTROPHE ::MRG::Key::Apostrophe /* ' */
#define MRG_KEY_COMMA ::MRG::Key::Comma           /* , */
#define MRG_KEY_MINUS ::MRG::Key::Minus           /* - */
#define MRG_KEY_PERIOD ::MRG::Key::Period         /* . */
#define MRG_KEY_SLASH ::MRG::Key::Slash           /* / */
#define MRG_KEY_0 ::MRG::Key::D0
#define MRG_KEY_1 ::MRG::Key::D1
#define MRG_KEY_2 ::MRG::Key::D2
#define MRG_KEY_3 ::MRG::Key::D3
#define MRG_KEY_4 ::MRG::Key::D4
#define MRG_KEY_5 ::MRG::Key::D5
#define MRG_KEY_6 ::MRG::Key::D6
#define MRG_KEY_7 ::MRG::Key::D7
#define MRG_KEY_8 ::MRG::Key::D8
#define MRG_KEY_9 ::MRG::Key::D9
#define MRG_KEY_SEMICOLON ::MRG::Key::Semicolon /* ; */
#define MRG_KEY_EQUAL ::MRG::Key::Equal         /* = */
#define MRG_KEY_A ::MRG::Key::A
#define MRG_KEY_B ::MRG::Key::B
#define MRG_KEY_C ::MRG::Key::C
#define MRG_KEY_D ::MRG::Key::D
#define MRG_KEY_E ::MRG::Key::E
#define MRG_KEY_F ::MRG::Key::F
#define MRG_KEY_G ::MRG::Key::G
#define MRG_KEY_H ::MRG::Key::H
#define MRG_KEY_I ::MRG::Key::I
#define MRG_KEY_J ::MRG::Key::J
#define MRG_KEY_K ::MRG::Key::K
#define MRG_KEY_L ::MRG::Key::L
#define MRG_KEY_M ::MRG::Key::M
#define MRG_KEY_N ::MRG::Key::N
#define MRG_KEY_O ::MRG::Key::O
#define MRG_KEY_P ::MRG::Key::P
#define MRG_KEY_Q ::MRG::Key::Q
#define MRG_KEY_R ::MRG::Key::R
#define MRG_KEY_S ::MRG::Key::S
#define MRG_KEY_T ::MRG::Key::T
#define MRG_KEY_U ::MRG::Key::U
#define MRG_KEY_V ::MRG::Key::V
#define MRG_KEY_W ::MRG::Key::W
#define MRG_KEY_X ::MRG::Key::X
#define MRG_KEY_Y ::MRG::Key::Y
#define MRG_KEY_Z ::MRG::Key::Z
#define MRG_KEY_LEFT_BRACKET ::MRG::Key::LeftBracket   /* [ */
#define MRG_KEY_BACKSLASH ::MRG::Key::Backslash        /* \ */
#define MRG_KEY_RIGHT_BRACKET ::MRG::Key::RightBracket /* ] */
#define MRG_KEY_GRAVE_ACCENT ::MRG::Key::GraveAccent   /* ` */
#define MRG_KEY_WORLD_1 ::MRG::Key::World1             /* non-US #1 */
#define MRG_KEY_WORLD_2 ::MRG::Key::World2             /* non-US #2 */

/* Function keys */
#define MRG_KEY_ESCAPE ::MRG::Key::Escape
#define MRG_KEY_ENTER ::MRG::Key::Enter
#define MRG_KEY_TAB ::MRG::Key::Tab
#define MRG_KEY_BACKSPACE ::MRG::Key::Backspace
#define MRG_KEY_INSERT ::MRG::Key::Insert
#define MRG_KEY_DELETE ::MRG::Key::Delete
#define MRG_KEY_RIGHT ::MRG::Key::Right
#define MRG_KEY_LEFT ::MRG::Key::Left
#define MRG_KEY_DOWN ::MRG::Key::Down
#define MRG_KEY_UP ::MRG::Key::Up
#define MRG_KEY_PAGE_UP ::MRG::Key::PageUp
#define MRG_KEY_PAGE_DOWN ::MRG::Key::PageDown
#define MRG_KEY_HOME ::MRG::Key::Home
#define MRG_KEY_END ::MRG::Key::End
#define MRG_KEY_CAPS_LOCK ::MRG::Key::CapsLock
#define MRG_KEY_SCROLL_LOCK ::MRG::Key::ScrollLock
#define MRG_KEY_NUM_LOCK ::MRG::Key::NumLock
#define MRG_KEY_PRINT_SCREEN ::MRG::Key::PrintScreen
#define MRG_KEY_PAUSE ::MRG::Key::Pause
#define MRG_KEY_F1 ::MRG::Key::F1
#define MRG_KEY_F2 ::MRG::Key::F2
#define MRG_KEY_F3 ::MRG::Key::F3
#define MRG_KEY_F4 ::MRG::Key::F4
#define MRG_KEY_F5 ::MRG::Key::F5
#define MRG_KEY_F6 ::MRG::Key::F6
#define MRG_KEY_F7 ::MRG::Key::F7
#define MRG_KEY_F8 ::MRG::Key::F8
#define MRG_KEY_F9 ::MRG::Key::F9
#define MRG_KEY_F10 ::MRG::Key::F10
#define MRG_KEY_F11 ::MRG::Key::F11
#define MRG_KEY_F12 ::MRG::Key::F12
#define MRG_KEY_F13 ::MRG::Key::F13
#define MRG_KEY_F14 ::MRG::Key::F14
#define MRG_KEY_F15 ::MRG::Key::F15
#define MRG_KEY_F16 ::MRG::Key::F16
#define MRG_KEY_F17 ::MRG::Key::F17
#define MRG_KEY_F18 ::MRG::Key::F18
#define MRG_KEY_F19 ::MRG::Key::F19
#define MRG_KEY_F20 ::MRG::Key::F20
#define MRG_KEY_F21 ::MRG::Key::F21
#define MRG_KEY_F22 ::MRG::Key::F22
#define MRG_KEY_F23 ::MRG::Key::F23
#define MRG_KEY_F24 ::MRG::Key::F24
#define MRG_KEY_F25 ::MRG::Key::F25

/* Keypad */
#define MRG_KEY_KP_0 ::MRG::Key::KP0
#define MRG_KEY_KP_1 ::MRG::Key::KP1
#define MRG_KEY_KP_2 ::MRG::Key::KP2
#define MRG_KEY_KP_3 ::MRG::Key::KP3
#define MRG_KEY_KP_4 ::MRG::Key::KP4
#define MRG_KEY_KP_5 ::MRG::Key::KP5
#define MRG_KEY_KP_6 ::MRG::Key::KP6
#define MRG_KEY_KP_7 ::MRG::Key::KP7
#define MRG_KEY_KP_8 ::MRG::Key::KP8
#define MRG_KEY_KP_9 ::MRG::Key::KP9
#define MRG_KEY_KP_DECIMAL ::MRG::Key::KPDecimal
#define MRG_KEY_KP_DIVIDE ::MRG::Key::KPDivide
#define MRG_KEY_KP_MULTIPLY ::MRG::Key::KPMultiply
#define MRG_KEY_KP_SUBTRACT ::MRG::Key::KPSubtract
#define MRG_KEY_KP_ADD ::MRG::Key::KPAdd
#define MRG_KEY_KP_ENTER ::MRG::Key::KPEnter
#define MRG_KEY_KP_EQUAL ::MRG::Key::KPEqual

#define MRG_KEY_LEFT_SHIFT ::MRG::Key::LeftShift
#define MRG_KEY_LEFT_CONTROL ::MRG::Key::LeftControl
#define MRG_KEY_LEFT_ALT ::MRG::Key::LeftAlt
#define MRG_KEY_LEFT_SUPER ::MRG::Key::LeftSuper
#define MRG_KEY_RIGHT_SHIFT ::MRG::Key::RightShift
#define MRG_KEY_RIGHT_CONTROL ::MRG::Key::RightControl
#define MRG_KEY_RIGHT_ALT ::MRG::Key::RightAlt
#define MRG_KEY_RIGHT_SUPER ::MRG::Key::RightSuper
#define MRG_KEY_MENU ::MRG::Key::Menu

#endif