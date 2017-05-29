#pragma once

#include "TaskBase.h"
#include "Future.h"
#include "Promise.h"
#include <thread>

namespace Acoross
{
	namespace FutureImpl
	{
		template <class Fty>
		Future<std::result_of_t<Fty()>> Async(Fty&& func)
		{
			typedef std::result_of_t<Fty()> Rty;

			auto ptr = Make<TaskAsyncState<Rty>>(func);
			std::thread([ptr]()
			{
				ptr->CallImmediate();
			}).detach();

			return Future<Rty>(ptr);
		}
	}
}