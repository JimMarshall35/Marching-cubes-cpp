#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CubeMarcherTests
{
	TEST_CLASS(CubeMarcherTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			bool t = true;
			Assert::IsTrue(t);
		}
	};
}
