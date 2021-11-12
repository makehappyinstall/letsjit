#pragma once

#include <memory>

namespace letsjit::compilation {

struct Context;

std::unique_ptr<Context> MakeContext();

}