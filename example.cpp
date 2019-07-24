#include <vector>
#include <thread>
#include <memory>
#include <string.h>
#include "sentry.h"
#include <atomic>
#include <excpt.h>
#include <iostream>

#ifdef _WIN32
const char *handler_path = "../Release/crashpad_handler.exe";
#else
const char *handler_path = "bin/Release/crashpad_handler";
#endif

static std::vector<std::thread> s_threads;

int hanlder(struct _EXCEPTION_POINTERS *ep)
{
    std::cout << "crashing the app ourselves." << std::endl;
    sentry_capture_event_and_crash(ep);
    return EXCEPTION_EXECUTE_HANDLER;
}

void fn()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    throw std::runtime_error("some error");
}

void wrapper()
{
       try {
            fn();
        }
        catch (std::exception& e)
        {
            std::cout << "caught exception: " << e.what() << std::endl;
            throw std::runtime_error("another one");
        }
}

void catcher()
{
    __try {
        wrapper();
    }
    __except (hanlder(GetExceptionInformation())) {}
}

int main(void) {

    sentry_options_t *options = sentry_options_new();

    sentry_options_set_dsn(options, "https://6c3abc28bbb847f0b19467897c102826@sentry.io/1501735");
    sentry_options_set_handler_path(options, handler_path);
    sentry_options_set_environment(options, "Production");
    sentry_options_set_release(options, "testing");
    sentry_options_set_database_path(options, "sentrypad-db");
    sentry_options_set_debug(options, 1);
    //sentry_options_add_attachment(options, "example", "../example.c");
    sentry_init(options);
    sentry_set_reporting_enabled(1);

    catcher();
}
