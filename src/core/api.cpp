#include "api.hpp"
#include "core/context.hpp"
#include "core/common.hpp"

// Platform information
#include <stdexcept>
#include <memory>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

// system() API
#include <cstdlib>
#include <stdexcept>
#include <cstring>

// sethostname() and gethostname()
#include <unistd.h>

#include <filesystem>
#include <fstream>

namespace ietf::sys {
namespace API {
}
}