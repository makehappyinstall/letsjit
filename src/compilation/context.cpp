#include <ast/instructions/function_definition.hpp>
#include <compilation/context.hpp>
#include <llvm/IR/Constant.h>
#include <llvm/Support/raw_os_ostream.h>
#include <sstream>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>

namespace letsjit::compilation {

Context::Context() {}

::llvm::LLVMContext &Context::GetLLVMContext() const { return *ctx; }

::llvm::IRBuilder<> &Context::GetIRBuilder() const { return *ir_builder; }

::llvm::Module &Context::GetModule() const { return *module; }

TypesContext &Context::GetTypesContext() const { return *types_context; }

std::unique_ptr<Context> MakeContext() { return std::make_unique<Context>(); }

void Context::EnterFunction(const std::string &name) const {
  call_stack.push(GetTypesContext().GetFunctionPtr(name));
}

void Context::ExitFunction() const { call_stack.pop(); }

std::string Context::DumpIR() const {
  std::ostringstream oss;
  if (execution_engine_ != nullptr) {
    throw std::runtime_error(
        "Cannot dump IR sources because module already compiled");
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

void Context::Compile() {
  std::string errStr;
  std::unique_ptr<::llvm::RTDyldMemoryManager> MM =
      std::make_unique<::llvm::SectionMemoryManager>();
  ::llvm::InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  GetTypesContext().InitGlobalFunctions();

  execution_engine_ = ::llvm::EngineBuilder(std::move(module))
                          .setEngineKind(::llvm::EngineKind::JIT)
                          .setMCJITMemoryManager(std::move(MM))
                          .setErrorStr(&errStr)
                          .setOptLevel(::llvm::CodeGenOpt::None)
                          .setCodeModel(::llvm::CodeModel::Large)
                          .setRelocationModel(::llvm::Reloc::Static)
                          .setMCPU(::llvm::sys::getHostCPUName())
                          .create();

  GetTypesContext().InitLocalFunctions(execution_engine_);

  if (!errStr.empty()) {
    throw std::runtime_error(errStr);
  }
}

} // namespace letsjit::compilation
