#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <utility>

class CountdownTimer {
	public:
		using Id = std::string;
		using Duration = std::chrono::milliseconds;
		using EpochNanoseconds = std::chrono::nanoseconds;
		using Handler = std::function<void(const Id&)>;

		explicit CountdownTimer(Duration starting_time = Duration::zero(), Handler handler = {})
				: CountdownTimer(Id{}, starting_time, std::move(handler)) {}

		explicit CountdownTimer(EpochNanoseconds expiration_time, Handler handler = {})
				: CountdownTimer(Id{}, expiration_time, std::move(handler)) {}

		CountdownTimer(Id id, Duration starting_time, Handler handler = {})
				: id_(std::move(id)),
					starting_time_(std::max(starting_time, Duration::zero())),
					remaining_time_(starting_time_),
					handler_(std::move(handler)) {}

		CountdownTimer(Id id, EpochNanoseconds expiration_time, Handler handler = {})
				: id_(std::move(id)), handler_(std::move(handler))
		{
			set_expiration_time_ns(expiration_time.count());
		}

		void start()
		{
			if (expiration_mode_) {
				remaining_time_ = remaining_time();
			} else {
				remaining_time_ = starting_time_;
			}
			running_ = remaining_time_ > Duration::zero();
			last_update_ = std::chrono::steady_clock::now();
		}

		void pause()
		{
			update();
			if (expiration_mode_ && running_) {
				remaining_time_ = remaining_time();
			}
			running_ = false;
		}

		void resume()
		{
			if (!running_ && remaining_time_ > Duration::zero()) {
				if (expiration_mode_) {
					expiration_time_ = std::chrono::system_clock::now() + remaining_time_;
				}
				last_update_ = std::chrono::steady_clock::now();
				running_ = true;
			}
		}

		void set_starting_time(Duration starting_time)
		{
			expiration_mode_ = false;
			starting_time_ = std::max(starting_time, Duration::zero());
			remaining_time_ = starting_time_;
			running_ = false;
			last_update_ = std::chrono::steady_clock::now();
		}

		void set_expiration_time_ns(std::int64_t expiration_time_ns)
		{
			expiration_mode_ = true;
			expiration_time_ = std::chrono::system_clock::time_point(EpochNanoseconds(expiration_time_ns));
			running_ = true;
			remaining_time_ = remaining_time();
			running_ = remaining_time_ > Duration::zero();
			last_update_ = std::chrono::steady_clock::now();
		}

		void set_id(Id id) { id_ = std::move(id); }
		[[nodiscard]] const Id& id() const { return id_; }
		[[nodiscard]] Duration starting_time() const { return starting_time_; }
		[[nodiscard]] Duration remaining_time() const
		{
			if (expiration_mode_ && running_) {
				const auto now = std::chrono::system_clock::now();
				if (now >= expiration_time_) {
					return Duration::zero();
				}
				return std::chrono::duration_cast<Duration>(expiration_time_ - now);
			}
			return remaining_time_;
		}
		[[nodiscard]] std::int64_t remaining_time_ms() const
		{
			return remaining_time().count();
		}
		[[nodiscard]] std::int64_t remaining_time_sec() const
		{
			return std::chrono::duration_cast<std::chrono::seconds>(remaining_time()).count();
		}
		[[nodiscard]] bool is_running() const { return running_; }
		[[nodiscard]] bool is_paused() const { return !running_ && remaining_time_ > Duration::zero(); }

		void set_handler(Handler handler) { handler_ = std::move(handler); }

		bool update()
		{
			if (!running_) {
				return false;
			}

			if (expiration_mode_) {
				remaining_time_ = remaining_time();
			} else {
				const auto now = std::chrono::steady_clock::now();
				const auto elapsed = std::chrono::duration_cast<Duration>(now - last_update_);
				last_update_ = now;

				if (elapsed < remaining_time_) {
					remaining_time_ -= elapsed;
					return false;
				}
			}

			if (remaining_time_ > Duration::zero()) {
				return false;
			}

			remaining_time_ = Duration::zero();
			running_ = false;
			if (handler_) {
				handler_(id_);
			}
			return true;
		}

	private:
		Id id_;
		Duration starting_time_;
		Duration remaining_time_;
		std::chrono::system_clock::time_point expiration_time_;
		std::chrono::steady_clock::time_point last_update_ = std::chrono::steady_clock::now();
		Handler handler_;
		bool running_ = false;
		bool expiration_mode_ = false;
};

class CountdownTimerManager {
	public:
		using Id = CountdownTimer::Id;
		using Duration = CountdownTimer::Duration;
		using EpochNanoseconds = CountdownTimer::EpochNanoseconds;
		using Handler = CountdownTimer::Handler;

		explicit CountdownTimerManager(Handler handler = {})
				: handler_(std::move(handler)) {}

		bool create(Id id, Duration starting_time)
		{
			const Id timer_id = id;
			// @TODO if we havae a handler, attach it to here. 
			return timers_.emplace(
				std::move(id), CountdownTimer(timer_id, starting_time, handler_)).second;
		}

		bool create_at(Id id, std::int64_t expiration_time_ns)
		{
			const Id timer_id = id;
			return timers_.emplace(
					std::move(id), CountdownTimer(timer_id, EpochNanoseconds(expiration_time_ns), handler_)).second;
		}

        void set_starting_time(Id id, Duration starting_time){
            const Id timer_id = id;
            timers_[timer_id].set_starting_time(starting_time);
        }

		void set_expiration_time_ns(const Id& id, std::int64_t expiration_time_ns)
		{
			timers_[id].set_expiration_time_ns(expiration_time_ns);
		}

        Duration get_starting_time(Id id){
            const Id timer_id = id;
            return timers_[timer_id].starting_time();
        }

		int get_remaining_time_sec(const Id& id) const {
			const auto timer = timers_.find(id);
			if (timer == timers_.end()) {
				return 0;
			}
			return static_cast<int>(timer->second.remaining_time_sec());
        }

		std::int64_t get_remaining_time_ms(const Id& id) const
		{
			const auto timer = timers_.find(id);
			if (timer == timers_.end()) {
				return 0;
			}
			return timer->second.remaining_time_ms();
		}

        bool is_running(Id id){
            const Id timer_id = id;
            return timers_[timer_id].is_running();
        }

		bool start(const Id& id)
		{
			const auto timer = timers_.find(id);
			if (timer == timers_.end()) {
				return false;
			}
			timer->second.start();
			return true;
		}

		void start_all()
		{
			for (auto& [id, timer] : timers_) {
				timer.start();
			}
		}

		bool pause(const Id& id)
		{
			const auto timer = timers_.find(id);
			if (timer == timers_.end()) {
				return false;
			}
			timer->second.pause();
			return true;
		}

		bool resume(const Id& id)
		{
			const auto timer = timers_.find(id);
			if (timer == timers_.end()) {
				return false;
			}
			timer->second.resume();
			return true;
		}

		bool update()
		{
			bool expired = false;
			for (auto& [id, timer] : timers_) {
				expired = timer.update() || expired;
			}
			return expired;
		}

		void set_handler(Handler handler)
		{
			handler_ = std::move(handler);
			for (auto& [id, timer] : timers_) {
				timer.set_handler(handler_);
			}
		}

		[[nodiscard]] std::size_t size() const { return timers_.size(); }

	private:
		Handler handler_;
		std::map<Id, CountdownTimer> timers_;
};
