#include <sstream>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/Constant.h>
#include <ast/instructions/function_definition.hpp>
#include <compilation/context.hpp>

#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/DynamicLibrary.h>

namespace letsjit {

std::vector<llvm::Type *> FunctionDeclaration::FetchArgTypes(const compilation::Context& ctx) const {
  std::vector<llvm::Type *> result{};
  result.reserve(args.size());
  std::transform(args.begin(), args.end(), std::back_inserter(result), [&ctx](const auto& arg) {
    return arg.second.ToTypeinfo(ctx.GetLLVMContext()).RawType();
  });
  return result;
}

}

namespace letsjit::compilation {

Context::Context() { }

::llvm::LLVMContext& Context::GetLLVMContext() const {
  return *ctx;
}

::llvm::IRBuilder<>& Context::GetIRBuilder() const {
  return *ir_builder;
}

::llvm::Module& Context::GetModule() const {
  return *module;
}

std::unique_ptr<Context> MakeContext() {
  return std::make_unique<Context>();
}

void Context::RegisterFunction(const FunctionInfo& function_info) const {
  functions->emplace(function_info.declaration.name, std::make_unique<FunctionInfo>(function_info));
}

void Context::RegisterExternalFunctionImpl(const FunctionDeclaration &fdecl, void* fun_ptr) const {
  std::vector<llvm::Type*> args = fdecl.FetchArgTypes(*this);
  auto fun_type = llvm::FunctionType::get(fdecl.return_type.ToTypeinfo(GetLLVMContext()).RawType(), args, false);
  FunctionInfo finfo{fdecl, ::llvm::Function::Create(fun_type, ::llvm::Function::ExternalLinkage, fdecl.name, &GetModule()), {}, fun_ptr};
  size_t id = 0;
  for (auto& arg: finfo.function->args()) {
    arg.setName(fdecl.args[id].first);
    finfo.arg_map[arg.getName()] = {&arg};
    ++id;
  }
  RegisterFunction(finfo);
}

void Context::EnterFunction(const std::string& name) const {
  if (auto it = functions->find(name); it != functions->end()) {
    call_stack.push(it->second.get());
    return;
  }
  throw std::runtime_error("Invalid function name " + name);
}

void Context::ExitFunction() const {
  call_stack.pop();
}

std::string Context::DumpIR() const {
  std::ostringstream oss;
  if (execution_engine_ != nullptr) {
    throw std::runtime_error("Cannot dump IR sources because module already compiled");
  }
  llvm::raw_os_ostream ros{oss};
  module->print(ros, nullptr);
  return oss.str();
}

FunctionInfo Context::CurrentFunction() const {
  if (call_stack.empty())
    throw std::runtime_error("Function-related call in non-function context");
  return *call_stack.top();
}

FunctionInfo Context::GetFunction(const std::string &name) const {
  if (const auto it = functions->find(name) ; it != functions->end()) {
    return *it->second;
  }
  throw std::runtime_error("Unknown function " + name + " requested");
}

void Context::Compile() {
  std::string errStr;
  std::unique_ptr<::llvm::RTDyldMemoryManager> MM = std::make_unique<::llvm::SectionMemoryManager>();
  ::llvm::InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  for (const auto& fun : *functions) {
    if (fun.second->fun_ptr) {
//      NOTE: addGlobalMapping() doesn't works with MCJIT
//      execution_engine_->addGlobalMapping(fun.first, reinterpret_cast<uint64_t>(fun.second->fun_ptr));
      ::llvm::sys::DynamicLibrary::AddSymbol(fun.first, fun.second->fun_ptr);
    }
  }

  execution_engine_ =
      ::llvm::EngineBuilder(std::move(module))
          .setEngineKind(::llvm::EngineKind::JIT)
          .setMCJITMemoryManager(std::move(MM))
          .setErrorStr(&errStr)
          .setOptLevel(::llvm::CodeGenOpt::None)
          .setCodeModel(::llvm::CodeModel::Large)
          .setRelocationModel(::llvm::Reloc::Static)
          .setMCPU(::llvm::sys::getHostCPUName())
          .create();

  for (const auto& fun : *functions) {
    if (!fun.second->fun_ptr)
      fun.second->fun_ptr = reinterpret_cast<void*>(execution_engine_->getFunctionAddress(fun.first));
  }

  if (!errStr.empty()) {
    throw std::runtime_error(errStr);
  }
}

}
