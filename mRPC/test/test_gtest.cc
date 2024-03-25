#include "gtest/gtest.h"
#include <stack>

// 测试实例1
TEST(testStack, simpletest) { /* NOLINT */
	std::stack<int> st;
	st.push(4);
	EXPECT_EQ(4, st.top());
	st.pop();
}

// 测试实例2
TEST(testStack, testAll) { /* NOLINT */
	std::stack<int> st;
	st.push(9);
	st.push(28);

	int val = st.top();
	st.pop();

	EXPECT_EQ(28, val); // 28等于val则测试通过（非致命断言）
	EXPECT_NE(0, val);  // 0不等于val则测试通过（非致命断言）
	EXPECT_GT(29, val); // 29大于val则测试通过（非致命断言）
	EXPECT_GE(29, val); // 29大于等于val则测试通过（非致命断言）
	EXPECT_TRUE(val == 28)
	    << "val is not equal to 28"; // val ==
	                                 // 28结果为false，输出后面日志（非致命断言）
}

GTEST_API_ int main(int argc, char** argv) {
	std::cout << "Running main() from gtest_main.cc\n";

	testing::InitGoogleTest(
	    &argc,
	    argv); // 解析命令行中的GoogleTest参数，它允许用户通过多样的命令行参数来控制测试程序的行为（即定制命令行参数的行为）
	return RUN_ALL_TESTS(); // 将会搜索不同的Test
	                        // Case和不同的源文件中所有已经存在测试案例，然后运行它们，所有Test都成功时返回1，否则返回0。
}
