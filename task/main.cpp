#include "Future.h"
#include "Promise.h"
#include <thread>
#include <iostream>
#include <chrono>
#include "Async.h"

#include <experimental/coroutine>
#include "FutureAwait.h"

void prnTh(const char* text)
{
	std::cout << text << ", tid: " << std::this_thread::get_id() << std::endl;
}

namespace Acoross
{
	namespace FutureImpl
	{
		Future<int> testAwait()
		{
			prnTh("testAwait");
			co_return 1;
		}

		Future<void> testAwait2()
		{
			prnTh("testAwait2");
			co_return;
		}

		Future<void> g()
		{
			prnTh("g1");
			printf("g: %d\n", co_await testAwait());
			prnTh("g2");
		}
	}
}

int main()
{
	prnTh("before");
	Acoross::FutureImpl::g().Wait();
	prnTh("after");

	using namespace std::chrono_literals;

	std::unique_ptr<int> ui;
	std::future<int> f;

	auto prom = Acoross::Make<Acoross::FutureImpl::Promise<int>>();
	auto fut = prom->GetFuture();

	auto th = std::thread([prom]() 
	{
		std::this_thread::sleep_for(2s);
		prom->SetValue(10);
	});

	auto val = fut.Get();
	std::cout << val << std::endl;

	th.join();

	Acoross::FutureImpl::Async([]()
	{
		return 1;
	});

	auto async_fut = std::async(std::launch::async, []()->int
	{
		return 10;
	});

	auto def_fut = std::async(std::launch::deferred, []()
	{

	});
}