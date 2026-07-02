//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_SYSTEM_HPP
#define ENGINE_SYSTEM_HPP


class ISystem
{
public:
    ISystem() = default;
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;
};

#endif //ENGINE_SYSTEM_HPP