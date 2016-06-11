#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <utility>

template <class T>
class service_wrap
{
	T *s;
public:
	template <class ...Arg>
    service_wrap(Arg ...arg):s(new T(arg...)){}
	boost::asio::io_service& operator()(){
		(*s)();
	}
    void run(){
		s->run();
	}
    void stop(){
		s->stop();
	}
	void des(){
		delete s;
	}
};


class services_
{
    std::vector<std::pair<boost::asio::io_service, boost::thread> > ss;
    std::vector<boost::asio::io_service::work> ws;
public:
    services_(int num);
	boost::asio::io_service& operator()();
    void run();
    void stop();
};

class service
{
	boost::asio::io_service ser;
	boost::thread thr;
    boost::asio::io_service::work wk;
public:
    service();
	boost::asio::io_service& operator()();
    void run();
    void stop();
};