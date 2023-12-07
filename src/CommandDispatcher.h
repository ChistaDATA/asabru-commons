#pragma once

#include <cstdlib>
#include <string>
#include "TypeFactory.h"
#include "ComputationContext.h"
#include "BaseComputationCommand.h"


class CommandDispatcher
{
private:
    inline static TypeFactory *typeFactory = new TypeFactory();
public:
    template<class T>
    static bool RegisterCommand(std::string commandName) {
        try {
            typeFactory->loadPlugin<T>(commandName);
            return true;
        } catch (std::exception &e) {
            return false;
        }
    }
    template<class T>
    static T *GetCommand(std::string commandName) {
        return typeFactory->GetType<T>(commandName);
    }
    static bool Dispatch(const std::string &archetype, ComputationContext *ctx)
    {
        auto *command = typeFactory->GetType<BaseComputationCommand>(archetype);
        if (command == nullptr)
            return false;
        if (command->PreExecute(ctx))
        {
            bool rs = command->Execute(ctx);
            command->PostExecute(ctx);
            return rs;
        }
        return false;
    }
};

