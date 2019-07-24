#ifndef SENTRY_BACKEND_HPP_INCLUDED
#define SENTRY_BACKEND_HPP_INCLUDED

#include "internal.hpp"

namespace sentry {
void init_backend();
void enable_backend(bool);
void dump_without_crash();
void dump_and_crash(void*);
}

#endif
