#pragma once

/**
 * 输入键盘/按钮映射
 * 参考glfw
 */

namespace Hazel
{

enum class KeyCode : uint16_t {
    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44,      /* , */
    Minus = 45,      /* - */
    Period = 46,     /* . */
    Slash = 47,      /* / */
    D0 = 48,
    D1 = 49,
    D2 = 50,
    D3 = 51,
    D4 = 52,
    D5 = 53,
    D6 = 54,
    D7 = 55,
    D8 = 56,
    D9 = 57,
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
    World1 = 161,      /* non-US #1 */
    World2 = 162,      /* non-US #2 */
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
    KpDecimal = 330,
    KpDivide = 331,
    KpMultiply = 332,
    KpSubtract = 333,
    KpAdd = 334,
    KpEnter = 335,
    KpEqual = 336,
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,
    MouseButton1 = 0,
    MouseButton2 = 1,
    MouseButton3 = 2,
    MouseButton4 = 3,
    MouseButton5 = 4,
    MouseButton6 = 5,
    MouseButton7 = 6,
    MouseButton8 = 7,
    MouseButtonLast = MouseButton8,
    MouseButtonLeft = MouseButton1,
    MouseButtonRight = MouseButton2,
    MouseButtonMiddle = MouseButton3,
};

/* Printable keys */
#define HZ_KEY_SPACE KeyCode::Space
#define HZ_KEY_APOSTROPHE KeyCode::Apostrophe
#define HZ_KEY_COMMA KeyCode::Comma
#define HZ_KEY_MINUS KeyCode::Minus
#define HZ_KEY_PERIOD KeyCode::Period
#define HZ_KEY_SLASH KeyCode::Slash
#define HZ_KEY_0 KeyCode::D0
#define HZ_KEY_1 KeyCode::D1
#define HZ_KEY_2 KeyCode::D2
#define HZ_KEY_3 KeyCode::D3
#define HZ_KEY_4 KeyCode::D4
#define HZ_KEY_5 KeyCode::D5
#define HZ_KEY_6 KeyCode::D6
#define HZ_KEY_7 KeyCode::D7
#define HZ_KEY_8 KeyCode::D8
#define HZ_KEY_9 KeyCode::D9
#define HZ_KEY_SEMICOLON KeyCode::Semicolon
#define HZ_KEY_EQUAL KeyCode::Equal
#define HZ_KEY_A KeyCode::A
#define HZ_KEY_B KeyCode::B
#define HZ_KEY_C KeyCode::C
#define HZ_KEY_D KeyCode::D
#define HZ_KEY_E KeyCode::E
#define HZ_KEY_F KeyCode::F
#define HZ_KEY_G KeyCode::G
#define HZ_KEY_H KeyCode::H
#define HZ_KEY_I KeyCode::I
#define HZ_KEY_J KeyCode::J
#define HZ_KEY_K KeyCode::K
#define HZ_KEY_L KeyCode::L
#define HZ_KEY_M KeyCode::M
#define HZ_KEY_N KeyCode::N
#define HZ_KEY_O KeyCode::O
#define HZ_KEY_P KeyCode::P
#define HZ_KEY_Q KeyCode::Q
#define HZ_KEY_R KeyCode::R
#define HZ_KEY_S KeyCode::S
#define HZ_KEY_T KeyCode::T
#define HZ_KEY_U KeyCode::U
#define HZ_KEY_V KeyCode::V
#define HZ_KEY_W KeyCode::W
#define HZ_KEY_X KeyCode::X
#define HZ_KEY_Y KeyCode::Y
#define HZ_KEY_Z KeyCode::Z
#define HZ_KEY_LEFT_BRACKET KeyCode::LeftBracket
#define HZ_KEY_BACKSLASH KeyCode::Backslash
#define HZ_KEY_RIGHT_BRACKET KeyCode::RightBracket
#define HZ_KEY_GRAVE_ACCENT KeyCode::GraveAccent
#define HZ_KEY_WORLD_1 KeyCode::World1
#define HZ_KEY_WORLD_2 KeyCode::World2
#define HZ_KEY_ESCAPE KeyCode::Escape
#define HZ_KEY_ENTER KeyCode::Enter
#define HZ_KEY_TAB KeyCode::Tab
#define HZ_KEY_BACKSPACE KeyCode::Backspace
#define HZ_KEY_INSERT KeyCode::Insert
#define HZ_KEY_DELETE KeyCode::Delete
#define HZ_KEY_RIGHT KeyCode::Right
#define HZ_KEY_LEFT KeyCode::Left
#define HZ_KEY_DOWN KeyCode::Down
#define HZ_KEY_UP KeyCode::Up
#define HZ_KEY_PAGE_UP KeyCode::PageUp
#define HZ_KEY_PAGE_DOWN KeyCode::PageDown
#define HZ_KEY_HOME KeyCode::Home
#define HZ_KEY_END KeyCode::End
#define HZ_KEY_CAPS_LOCK KeyCode::CapsLock
#define HZ_KEY_SCROLL_LOCK KeyCode::ScrollLock
#define HZ_KEY_NUM_LOCK KeyCode::NumLock
#define HZ_KEY_PRINT_SCREEN KeyCode::PrintScreen
#define HZ_KEY_PAUSE KeyCode::Pause
#define HZ_KEY_F1 KeyCode::F1
#define HZ_KEY_F2 KeyCode::F2
#define HZ_KEY_F3 KeyCode::F3
#define HZ_KEY_F4 KeyCode::F4
#define HZ_KEY_F5 KeyCode::F5
#define HZ_KEY_F6 KeyCode::F6
#define HZ_KEY_F7 KeyCode::F7
#define HZ_KEY_F8 KeyCode::F8
#define HZ_KEY_F9 KeyCode::F9
#define HZ_KEY_F10 KeyCode::F10
#define HZ_KEY_F11 KeyCode::F11
#define HZ_KEY_F12 KeyCode::F12
#define HZ_KEY_F13 KeyCode::F13
#define HZ_KEY_F14 KeyCode::F14
#define HZ_KEY_F15 KeyCode::F15
#define HZ_KEY_F16 KeyCode::F16
#define HZ_KEY_F17 KeyCode::F17
#define HZ_KEY_F18 KeyCode::F18
#define HZ_KEY_F19 KeyCode::F19
#define HZ_KEY_F20 KeyCode::F20
#define HZ_KEY_F21 KeyCode::F21
#define HZ_KEY_F22 KeyCode::F22
#define HZ_KEY_F23 KeyCode::F23
#define HZ_KEY_F24 KeyCode::F24
#define HZ_KEY_F25 KeyCode::F25
#define HZ_KEY_KP_0 KeyCode::KP0
#define HZ_KEY_KP_1 KeyCode::KP1
#define HZ_KEY_KP_2 KeyCode::KP2
#define HZ_KEY_KP_3 KeyCode::KP3
#define HZ_KEY_KP_4 KeyCode::KP4
#define HZ_KEY_KP_5 KeyCode::KP5
#define HZ_KEY_KP_6 KeyCode::KP6
#define HZ_KEY_KP_7 KeyCode::KP7
#define HZ_KEY_KP_8 KeyCode::KP8
#define HZ_KEY_KP_9 KeyCode::KP9
#define HZ_KEY_KP_DECIMAL KeyCode::KpDecimal
#define HZ_KEY_KP_DIVIDE KeyCode::KpDivide
#define HZ_KEY_KP_MULTIPLY KeyCode::KpMultiply
#define HZ_KEY_KP_SUBTRACT KeyCode::KpSubtract
#define HZ_KEY_KP_ADD KeyCode::KpAdd
#define HZ_KEY_KP_ENTER KeyCode::KpEnter
#define HZ_KEY_KP_EQUAL KeyCode::KpEqual
#define HZ_KEY_LEFT_SHIFT KeyCode::LeftShift
#define HZ_KEY_LEFT_CONTROL KeyCode::LeftControl
#define HZ_KEY_LEFT_ALT KeyCode::LeftAlt
#define HZ_KEY_LEFT_SUPER KeyCode::LeftSuper
#define HZ_KEY_RIGHT_SHIFT KeyCode::RightShift
#define HZ_KEY_RIGHT_CONTROL KeyCode::RightControl
#define HZ_KEY_RIGHT_ALT KeyCode::RightAlt
#define HZ_KEY_RIGHT_SUPER KeyCode::RightSuper
#define HZ_KEY_MENU KeyCode::Menu
#define HZ_MOUSE_BUTTON_1 KeyCode::MouseButton1
#define HZ_MOUSE_BUTTON_2 KeyCode::MouseButton2
#define HZ_MOUSE_BUTTON_3 KeyCode::MouseButton3
#define HZ_MOUSE_BUTTON_4 KeyCode::MouseButton4
#define HZ_MOUSE_BUTTON_5 KeyCode::MouseButton5
#define HZ_MOUSE_BUTTON_6 KeyCode::MouseButton6
#define HZ_MOUSE_BUTTON_7 KeyCode::MouseButton7
#define HZ_MOUSE_BUTTON_8 KeyCode::MouseButton8
#define HZ_MOUSE_BUTTON_LAST KeyCode::MouseButtonLast
#define HZ_MOUSE_BUTTON_LEFT KeyCode::MouseButtonLeft
#define HZ_MOUSE_BUTTON_RIGHT KeyCode::MouseButtonRight
#define HZ_MOUSE_BUTTON_MIDDLE KeyCode::MouseButtonMiddle

}  // namespace Hazel