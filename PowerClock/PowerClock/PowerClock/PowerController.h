#pragma once

namespace PowerClock::Common
{
    class PowerController
    {
    public:
        PowerController() = default;
        PowerController(bool force) : force(force) {};

        bool Lock();
        bool Sleep(bool hibernate);
        bool Shutdown();
        bool Restart();

    private:
        bool force;

        bool CheckHibernationAllowed();
        bool GetPrivileges();
    };
}