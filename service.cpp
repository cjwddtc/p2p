#include "service.hpp"

using boost::thread;
using boost::asio::io_service;
using boost::bind;

services_::services_(int num)
    : ss(num)
{
    for(auto& a : ss) {
	ws.push_back(boost::asio::io_service::work(a.first));
    }
}
boost::asio::io_service& services_::operator()()
{
    return ss[rand() % ss.size()].first;
}
void services_::run()
{
    for(auto& a : ss) {
	thread thr(bind(io_service::run, &a.first));
	a.second.swap(thr);
    }
}
void services_::stop()
{
    for(auto& a : ss) 
		a.first.stop();
    for(auto& a : ss) 
		a.second.join();
    for(auto& a : ss) 
		a.first.reset();
}

service::service():wk(ser)
{}

boost::asio::io_service& service::operator ()()
{
	return ser;
}

void service::run()
{
	thr=thread(bind(io_service::run, &ser));
}

void service::stop()
{
	ser.stop();
	thr.join();
	ser.reset();
}
