#include <gtest/gtest.h>
#include <letsjit.hpp>

TEST(Compilation, IncFun) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  FunctionNode node{
      {"do_inc",
       {{"x", letsjit::ast::MakeTypeHolder<int>()}},
       letsjit::ast::MakeTypeHolder<int>()},
      std::make_shared<ReturnNode>(Plus(MakeFunctionArg("x"), MakeNumber(1)))};
  node.CompileInstruction(*context);
  context->Compile();
  auto do_inc = context->GetFunction("do_inc");
  int incremented_value = do_inc.Call<int, int>(23);
  ASSERT_EQ(incremented_value, 24);
}

TEST(Compilation, FactFun) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();
  FunctionNode node{
      {"fact",
       {{"x", letsjit::ast::MakeTypeHolder<int>()}},
       letsjit::ast::MakeTypeHolder<int>()},
      MakeReturn(MakeTernary(
          LessEq(MakeFunctionArg("x"), MakeNumber(1)), MakeNumber(1),
          Mul(MakeFunctionArg("x"),
              MakeFunctionCall("fact",
                               Sub(MakeFunctionArg("x"), MakeNumber(1))))))};
  node.CompileInstruction(*context);
  std::string program = context->DumpIR();
  context->Compile();
  auto fact = context->GetFunction("fact");
  int fact_value = fact.Call<int, int>(4);
  ASSERT_EQ(fact_value, 24);
}

int last_x = 0;
void test_fun(int x) { last_x = x; }

TEST(Compilation, ExternalFunctionCall) {
  using namespace letsjit::ast::nodes;
  auto context = letsjit::compilation::MakeContext();

  context->RegisterExternalFunction("test_fun", test_fun);
  FunctionNode node{
      {"foo", {}, letsjit::ast::MakeTypeHolder<void>()},
      MakeSequence(MakeFunctionCall("test_fun", MakeNumber(42)), MakeReturn())};
  node.CompileInstruction(*context);
  context->Compile();
  auto foo = context->GetFunction("foo");
  last_x = 0;
  foo.Call<void>();
  ASSERT_EQ(last_x, 42);
}
