#pragma once 
#include <utility>
#include <array>

/*
 ласс дл€ хранени€ информации о вводе с клавиатуры
*/
class RRKeyboardState
{
public: 
    // Ќомера клавиш на клавиатуре, идентичные SDL_SCANCODE...
    // “аким образом, их можно приводить один тип к другому
    enum class Keys {

        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        N1 = 30,
        N2 = 31,
        N3 = 32,
        N4 = 33,
        N5 = 34,
        N6 = 35,
        N7 = 36,
        N8 = 37,
        N9 = 38,
        N0 = 39,

        ENTER = 40,
        ESCAPE = 41,
        BACKSPACE = 42,
        TAB = 43,
        SPACE = 44,
    };
      
    bool IsKeyDown(Keys key) const {
        return keys[static_cast<int>(key)];
    }
    bool IsKeyUp(Keys key) const {
        return !IsKeyDown(key);
    }

    bool OldIsKeyUp(Keys key) const {
        return !OldIsKeyDown(key);
    }
    bool OldIsKeyDown(Keys key) const {
        return oldKeys[static_cast<int>(key)];
    }

    bool Click(Keys key) const {
        return IsKeyDown(key) && OldIsKeyUp(key);
    }

    void Update(const unsigned char* keys) {
        // каждый раз копируем два массива - дл€ текущего состо€ни€ и дл€ предыдущего 
        for (unsigned char i{}; i < MaxArraySize; i++) { 
            oldKeys[i] = this->keys[i];
            this->keys[i] = keys[i];
        }
    }
private:
    static constexpr int MaxArraySize{ 44 };
    std::array<unsigned char, MaxArraySize> keys, oldKeys; 
};

