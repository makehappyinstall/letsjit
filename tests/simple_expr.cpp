#include <gtest/gtest.h>
#include <letsjit.hpp>

TEST(SimpleExpr, NumbersExpr) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  ASSERT_EQ(Number<uint8_t>{42}.Compile(*context).ToString(), "i8 42");
  ASSERT_EQ(Number<uint16_t>{24}.Compile(*context).ToString(), "i16 24");
  ASSERT_EQ(Number<int8_t>{44}.Compile(*context).ToString(), "i8 44");
  ASSERT_EQ(Number<int64_t>{445}.Compile(*context).ToString(), "i64 445");
  ASSERT_EQ(Number<double>{12.24}.Compile(*context).ToString(),
            "double 1.224000e+01");
}

TEST(SimpleExpr, ArithmeticOp) {
  auto context = letsjit::compilation::MakeContext();
  using namespace letsjit::ast::nodes;
  using letsjit::ast::ArithmeticOperations;
  auto sum = ArithmeticOpNode{std::make_shared<Number<int>>(42),
                              std::make_shared<Number<int>>(24),
                              ArithmeticOperations::kPlus}
                 .Compile(*context);
  ASSERT_EQ(sum.ToString(), "i32 66");
}

TEST(SimpleExpr, SimpleRet) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  ReturnNode ret{std::make_shared<Number<int>>(22)};
  ret.CompileInstruction(*context);
}

TEST(SimpleExpr, SimpleFun) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  FunctionNode node{
      {"foo", {}, letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<ReturnNode>(std::make_shared<Number<int>>(24))};
  node.CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

define i32 @foo() {
entry:
  ret i32 24
}
)");
}

TEST(SimpleExpr, SimpleFunArgAccess) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  FunctionNode node{
      {"do_inc",
       {{"x", letsjit::ast::MakeTypeHolder<int>()}},
       letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<ReturnNode>(std::make_shared<ArithmeticOpNode>(
          std::make_shared<FunctionArgAccessNode>("x"),
          std::make_shared<Number<int>>(1),
          letsjit::ast::ArithmeticOperations::kPlus))};
  node.CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

define i32 @do_inc(i32 %x) {
entry:
  %addtmp = add i32 %x, 1
  ret i32 %addtmp
}
)");
}

TEST(SimpleExpr, SimpleTernary) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  FunctionNode node{
      {"do_ret_ternary", {}, letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<ReturnNode>(std::make_shared<TernaryNode>(
          std::make_shared<Number<bool>>(true),
          std::make_shared<Number<int>>(1), std::make_shared<Number<int>>(2)))};
  node.CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

define i32 @do_ret_ternary() {
entry:
  br i8 1, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi i32 [ 1, %then ], [ 2, %else ]
  ret i32 %iftmp
}
)");
}

TEST(SimpleExpr, SimpleCondition) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  FunctionNode node{
      {"do_ret_ternary", {}, letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<SequenceNode>(
          std::vector<std::shared_ptr<letsjit::ast::AbstractInstruction>>{
              std::make_shared<ConditionNode>(
                  std::make_shared<Number<bool>>(true),
                  std::make_shared<ReturnNode>(
                      std::make_shared<Number<int>>(42))),
              std::make_shared<ReturnNode>(
                  std::make_shared<Number<int>>(21))})};
  node.CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

define i32 @do_ret_ternary() {
entry:
  br i8 1, label %then, label %ifcont

then:                                             ; preds = %entry
  ret i32 42

ifcont:                                           ; preds = %entry
  ret i32 21
}
)");
}

TEST(SimpleExpr, SimpleFunctionCall) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  auto foo_func = std::make_shared<FunctionNode>(
      letsjit::FunctionDeclaration{"foo",
                                   {{"x", letsjit::ast::MakeTypeHolder<int>()}},
                                   letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<ReturnNode>(std::make_shared<FunctionCallNode>(
          "foo", std::vector<std::shared_ptr<letsjit::ast::AbstractExpression>>{
                     std::make_shared<Number<int>>(1)})));
  foo_func->CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

define i32 @foo(i32 %x) {
entry:
  %calltmp = call i32 @foo(i32 1)
  ret i32 %calltmp
}
)");
}

int test_function(int x, double y) { return x + y; }

TEST(SimpleExpr, ExternalFunctionCall) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  context->GetTypesContext().RegisterExternalFunction("test_function",
                                                      test_function);
  auto foo_func = std::make_shared<FunctionNode>(
      letsjit::FunctionDeclaration{"foo",
                                   {{"x", letsjit::ast::MakeTypeHolder<int>()}},
                                   letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<ReturnNode>(std::make_shared<FunctionCallNode>(
          "test_function",
          std::vector<std::shared_ptr<letsjit::ast::AbstractExpression>>{
              std::make_shared<FunctionArgAccessNode>("x"),
              std::make_shared<Number<double>>(0)})));
  foo_func->CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

declare i32 @test_function(i32, double)

define i32 @foo(i32 %x) {
entry:
  %calltmp = call i32 @test_function(i32 %x, double 0.000000e+00)
  ret i32 %calltmp
}
)");
}

TEST(SimpleExpr, ExternalFunctionCallInstruction) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  context->GetTypesContext().RegisterExternalFunction("test_function",
                                                      test_function);
  auto foo_func = std::make_shared<FunctionNode>(
      letsjit::FunctionDeclaration{"foo",
                                   {{"x", letsjit::ast::MakeTypeHolder<int>()}},
                                   letsjit::ast::MakeTypeHolder<void>()},
      MakeSequence(MakeFunctionCall("test_function", MakeFunctionArg("x"),
                                    MakeNumber<double>(0)),
                   MakeFunctionCall("test_function", MakeFunctionArg("x"),
                                    MakeNumber<double>(1)),
                   MakeReturn()));
  foo_func->CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

declare i32 @test_function(i32, double)

define void @foo(i32 %x) {
entry:
  %calltmp = call i32 @test_function(i32 %x, double 0.000000e+00)
  %calltmp1 = call i32 @test_function(i32 %x, double 1.000000e+00)
  ret void
}
)");
}

struct FunctionReturnPointerStruct {};
FunctionReturnPointerStruct *function_returns_pointer() { return nullptr; }

TEST(SimpleExpr, FunctionReturnPointer) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  context->GetTypesContext().RegisterStruct(
      {"FunctionReturnPointerStruct", {}, typeid(FunctionReturnPointerStruct)});
  context->GetTypesContext().RegisterExternalFunction(
      "function_returns_pointer", function_returns_pointer);
  auto foo_func = std::make_shared<FunctionNode>(
      letsjit::FunctionDeclaration{
          "foo",
          {},
          letsjit::ast::MakeTypeHolder<FunctionReturnPointerStruct *>(
              context->GetTypesContext())},
      MakeReturn(MakeFunctionCall("function_returns_pointer")));
  foo_func->CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

%FunctionReturnPointerStruct = type opaque

declare %FunctionReturnPointerStruct* @function_returns_pointer()

define %FunctionReturnPointerStruct* @foo() {
entry:
  %calltmp = call %FunctionReturnPointerStruct* @function_returns_pointer()
  ret %FunctionReturnPointerStruct* %calltmp
}
)");
}

struct TestSubStruct {
  int value;
};

struct TestStruct {
  int foo;
  double bar;
  TestSubStruct sub_struct;
};

TEST(SimpleExpr, FieldAccess) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  context->GetTypesContext().RegisterStruct(
      {"TestStruct",
       {{"foo", {&TestStruct::foo, context->GetTypesContext()}},
        {"bar", {&TestStruct::bar, context->GetTypesContext()}}},
       typeid(TestStruct)});
  FunctionNode node{
      {"get_bar",
       {{"test_struct", letsjit::ast::MakeTypeHolder<TestStruct *>()}},
       letsjit::ast::MakeTypeHolder<double>()},
      MakeReturn(MakeFieldAccess(MakeFunctionArg("test_struct"), "bar"))};
  node.CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

%TestStruct = type opaque

define double @get_bar(%TestStruct* %test_struct) {
entry:
  %addptroffset = add %TestStruct* %test_struct, i64 8
  %deref = load double, double* %addptroffset
  ret double %deref
}
)");
}

TEST(SimpleExpr, SubstructFieldAccess) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  context->GetTypesContext().RegisterStruct(
      {"TestSubStruct",
       {{"value", {&TestSubStruct::value, context->GetTypesContext()}}},
       typeid(TestSubStruct)});
  context->GetTypesContext().RegisterStruct(
      {"TestStruct",
       {{"foo", {&TestStruct::foo, context->GetTypesContext()}},
        {"bar", {&TestStruct::bar, context->GetTypesContext()}},
        {"sub_struct", {&TestStruct::sub_struct, context->GetTypesContext()}}},
       typeid(TestStruct)});
  FunctionNode node{
      {"get_sub_struct",
       {{"test_struct", letsjit::ast::MakeTypeHolder<TestStruct *>()}},
       letsjit::ast::MakeTypeHolder<TestSubStruct *>()},
      MakeReturn(
          MakeFieldAccess(MakeFunctionArg("test_struct"), "sub_struct"))};
  node.CompileInstruction(*context);
  ASSERT_EQ(context->DumpIR(), R"(; ModuleID = 'main_module'
source_filename = "main_module"

%TestSubStruct = type opaque
%TestStruct = type opaque

define %TestSubStruct* @get_sub_struct(%TestStruct* %test_struct) {
entry:
  %addptroffset = add %TestStruct* %test_struct, i64 16
  ret %TestSubStruct* %addptroffset
}
)");
}
