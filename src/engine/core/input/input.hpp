//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_INPUT_HPP
#define ENGINE_INPUT_HPP

#include <unordered_set>

enum DeviceType
{
    KEYBOARD,
    MOUSE,
    JOYSTICK
};

class Input
{
    int input_key;
    DeviceType device_type;
public:
    explicit Input(const int input_key, const DeviceType device) : input_key(input_key), device_type(device){}
    ~Input() = default;

    int get_input_key() const
    {
        return this->input_key;
    }
    DeviceType get_input_type() const noexcept
    {
        return this->device_type;
    }

    bool operator==(const Input& o) const
    {
        return this->input_key == o.input_key && this->device_type == o.device_type;
    }
};

#endif //ENGINE_INPUT_HPP