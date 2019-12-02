/**=======================================================================

	messaging.hpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>


namespace messaging
{
	//!---------------------------------------------------------
	//! @brief Message Base (Abstract)
	//!---------------------------------------------------------
	//! 
	struct message_base
	{
		virtual ~message_base() {}
	};


	//!---------------------------------------------------------
	//! @brief Wrapped Message
	//!---------------------------------------------------------
	//! 
	template<typename Msg>
	struct wrapped_message : message_base
	{
		Msg _contents;

		explicit wrapped_message(Msg const& contents) : _contents(contents) {}
	};


	//!---------------------------------------------------------
	//! @brief Message Queue class
	//!---------------------------------------------------------
	//! 
	class queue
	{
		std::mutex _mutex;
		std::condition_variable _cond;
		std::queue<std::shared_ptr<message_base> > _queue;

	public:
		template<typename T> void push(T const& msg)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.push(std::make_shared<wrapped_message<T> >(msg));
			_cond.notify_all();
		}

		std::shared_ptr<message_base> wait_and_pop()
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_cond.wait(lock, [this] {return !_queue.empty(); });
			auto res = _queue.front();
			_queue.pop();

			return res;
		}
	};


	//!---------------------------------------------------------
	//! @brief Sender Wrapper : send(push) message to queue
	//!---------------------------------------------------------
	//! 
	class sender
	{
		queue* _queue;

	public:
		sender() : _queue(nullptr) {}
		explicit sender(queue *q) : _queue(q) {}

		template<typename Msg>
		void send(Msg const& msg)
		{
			if (_queue)
				_queue->push(msg);
		}
	};


	class close_queue
	{};


	//!---------------------------------------------------------
	//! @brief Dispatcher Wrapper : pop & dispatch message
	//!---------------------------------------------------------
	//! 
	class dispatcher
	{
		queue *_queue;
		bool _chained;

		dispatcher(dispatcher const&) = delete;
		dispatcher& operator=(dispatcher const&) = delete;

		template<typename Dispatcher, typename Msg, typename Func>
		friend class TemplateDispatcher;

		void wait_and_dispatch()
		{
			for (;;)
			{
				auto msg = _queue->wait_and_pop();
				dispatch(msg);
			}
		}

		bool dispatch(std::shared_ptr<message_base> const& msg)
		{
			if (dynamic_cast<wrapped_message<close_queue>*>(msg.get()))
			{
				throw close_queue();
			}
			return false;
		}

	public:
		dispatcher(dispatcher&& other)
			: _queue(other._queue), _chained(other._chained)
		{
			other._chained = true;
		}

		explicit dispatcher(queue *q)
			: _queue(q), _chained(false)
		{}

		template<typename Msg, typename Func>
		TemplateDispatcher<dispatcher, Msg, Func> handle(Func&& f)
		{
			return TemplateDispatcher<dispatcher, Msg, Func>(_queue, this, std::forward<Func>(f));
		}

		~dispatcher() noexcept(false)
		{
			if (!_chained)
			{
				wait_and_dispatch();
			}
		}
	};


	class receiver
	{
		queue _queue;

	public:
		operator sender()
		{
			return sender{ (&_queue) };
		}

		dispatcher wait()
		{
			return dispatcher{ (&_queue) };
		}
	};


	template<typename PreviousDispatcher, typename Msg, typename Func>
	class TemplateDispatcher
	{
		queue *_queue;
		PreviousDispatcher *_prev;
		Func _f;
		bool _chained;

		TemplateDispatcher(TemplateDispatcher const&) = delete;
		TemplateDispatcher& operator()(TemplateDispatcher const&) = delete;

		template<typename Dispatcher, typename OtherMsg, typename OtherFunc>
		friend class TemplateDispatcher;

		void wait_and_dispatch()
		{
			for (;;)
			{
				auto msg = _queue->wait_and_pop();
				if (dispatch(msg))
					break;
			}
		}

		bool dispatch(std::shared_ptr<message_base> const& msg)
		{
			if (wrapped_message<Msg>* wrapper = dynamic_cast<wrapped_message<Msg>*>(msg.get()))
			{
				_f(wrapper->_contents);
				return true;
			}
			else
			{
				return _prev->dispatch(msg);
			}
		}

	public:
		TemplateDispatcher(TemplateDispatcher&& other) :
			_queue(other._queue), _prev(other._prev), _f(std::move(other._f)), _chained(other._chained)
		{
			other._chained = true;
		}
		TemplateDispatcher(queue *q, PreviousDispatcher *prev, Func&& f) :
			_queue(q), _prev(prev), _f(std::forward<Func>(f)), _chained(false)
		{
			prev->_chained = true;
		}

		template<typename OtherMsg, typename OtherFunc>
		TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
			handle(OtherFunc&& of)
		{
			return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
				_queue, this, std::forward<OtherFunc>(of)
				);

		}

		~TemplateDispatcher() noexcept(false)
		{
			if (!_chained)
				wait_and_dispatch();
		}
	};
}