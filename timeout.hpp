#ifndef TIMEOUT_H
#define TIMEOUT_H
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2.hpp>
#include <chrono>
class timer
{
	std::chrono::seconds sec;
	boost::asio::steady_timer clo;
public:
	timer(boost::asio::io_service &io,int sec);
	void reset();
	void start();
	void on_timeout(const boost::system::error_code &er);
    boost::signals2::signal<void(timer &)> OnTimeOut;
};
#endif