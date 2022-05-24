#include "pch.h"
#include "PowerController.h"

#include "windows.h"
#include "winuser.h"
#include "powrprof.h"

namespace PowerClock::Common
{
    bool PowerController::Lock()
    {
        return LockWorkStation();
    }

    bool PowerController::Sleep(bool hibernate)
    {
        if (!CheckHibernationAllowed())
        {
            //if (respect)
            //{
            //    // Cannot hibernate the system
            //    return false;
            //}
            //else
            //{
            //    hibernate = false;
            //}
            return false;
        }

        return SetSuspendState(hibernate, force, false);
    }

    bool PowerController::Shutdown()
    {
        if (!GetPrivileges())
        {
            return false;
        }
        else
        {
            DWORD flags = SHUTDOWN_POWEROFF;
            if (force)
            {
                flags |= SHUTDOWN_FORCE_SELF;
            }

            return InitiateShutdown(nullptr, nullptr, 0, flags, SHTDN_REASON_MINOR_MAINTENANCE);
        }
    }

    bool PowerController::Restart()
    {
        return false;
    }

    bool PowerController::CheckHibernationAllowed()
    {
        SYSTEM_POWER_CAPABILITIES capas;
        if (!GetPwrCapabilities(&capas))
        {
            // Failed to get power capabilities, assume hibernation is not supported.
            return false;
        }
        return capas.FastSystemS4 || capas.SystemS4;
    }

    bool PowerController::GetPrivileges()
    {
        HANDLE tokenHandle;
        TOKEN_PRIVILEGES privs{};

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle))
        {
            // Failed to open current process token.
            return false;
        }
        if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &privs.Privileges[0].Luid))
        {
            // Failed to lookup privilege value.
            return false;
        }

        privs.PrivilegeCount = 1;
        privs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (!AdjustTokenPrivileges(tokenHandle, false, &privs, 0, (PTOKEN_PRIVILEGES)NULL, 0))
        {
            // Cannot adjust token privileges - Prob no permissions.
            return false;
        }

        // Check if function succeeded to make privileges changes
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            // Not all privileges assigned - Can do a look to what has been assigned and return specific code
            return false;
        }

        return true;
    }
}