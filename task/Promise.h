#pragma once

#include "TaskBase.h"
#include "Future.h"

namespace Acoross
{
	namespace FutureImpl
	{
		template <class T>
		class Promise : public NoCopy
		{
		public:
			Promise()
				: _assocState(Make<AssociatedState<T>>())
				, _futureRetrieved{ false }
			{

			}

			~Promise()
			{

			}

			Future<T> GetFuture()
			{
				return Future<T>(_assocState);
			}

			void SetValue(T val)
			{
				_assocState.SetValue(val);
			}

			void SetException(std::exception_ptr exc)
			{
				_assocState.SetException(exc);
			}

		private:
			Sp<AssociatedState<T>> _assocState;
			bool _futureRetrieved{ false };
		};
	}
}