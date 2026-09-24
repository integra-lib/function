#include <gtest/gtest.h>

#include <hwlib/utilities/function.hpp>
#include <utility>

namespace
{

int Triple(int value)
{
    return value * 3;
}

TEST(FunctionTest, DefaultConstructedIsEmpty)
{
    const hwlib::utilities::Function<int(int)> fn;
    EXPECT_FALSE(static_cast<bool>(fn));
}

TEST(FunctionTest, NullptrConstructedIsEmpty)
{
    const hwlib::utilities::Function<int(int)> fn{nullptr};
    EXPECT_FALSE(static_cast<bool>(fn));
}

TEST(FunctionTest, CallsALambda)
{
    hwlib::utilities::Function<int(int)> fn = [](int value) { return value + 1; };
    ASSERT_TRUE(static_cast<bool>(fn));
    EXPECT_EQ(fn(41), 42);
}

TEST(FunctionTest, CallsAFreeFunction)
{
    hwlib::utilities::Function<int(int)> fn = &Triple;
    EXPECT_EQ(fn(5), 15);
}

TEST(FunctionTest, CapturesByValue)
{
    const int base                          = 10;
    hwlib::utilities::Function<int(int)> fn = [base](int value) { return base + value; };
    EXPECT_EQ(fn(5), 15);
}

// Regression: the ported operator= did not compile — it passed an object to
// unique_ptr::reset and never returned *this. The method is a template, so the
// defect stayed invisible until something actually assigned into a Function.
TEST(FunctionTest, AssignmentReplacesTheCallable)
{
    hwlib::utilities::Function<int(int)> fn = [](int value) { return value + 1; };
    EXPECT_EQ(fn(1), 2);

    fn = [](int value) { return value * 2; };
    EXPECT_EQ(fn(1), 2);
    EXPECT_EQ(fn(10), 20);
}

TEST(FunctionTest, AssignmentIntoAnEmptyFunction)
{
    hwlib::utilities::Function<int(int)> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    fn = &Triple;
    ASSERT_TRUE(static_cast<bool>(fn));
    EXPECT_EQ(fn(2), 6);
}

TEST(FunctionTest, MoveTransfersTheCallable)
{
    hwlib::utilities::Function<int(int)> source = [](int value) { return value + 1; };
    hwlib::utilities::Function<int(int)> target = std::move(source);

    ASSERT_TRUE(static_cast<bool>(target));
    EXPECT_EQ(target(41), 42);
    EXPECT_FALSE(static_cast<bool>(source)); // NOLINT (bugprone-use-after-move)
}

} // namespace
