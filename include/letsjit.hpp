#pragma once

#include <llvm/ExecutionEngine/MCJIT.h>

#include "ast/instructions/instruction.hpp"
#include "ast/expressions/expression.hpp"
#include "ast/expressions/number.hpp"
#include "ast/nodes/number.hpp"
#include "ast/nodes/arithmetic_op.hpp"
#include "ast/nodes/return.hpp"
#include "ast/nodes/function.hpp"
#include "ast/nodes/function_arg.hpp"
#include "ast/nodes/ternary_node.hpp"
#include "ast/nodes/sequence.hpp"
#include "ast/nodes/condition.hpp"
#include "ast/nodes/function_call.hpp"
#include "ast/nodes/field_access_node.hpp"
