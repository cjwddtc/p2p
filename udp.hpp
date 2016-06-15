#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <map>
#include <memory>
#include "timeout.hpp"

struct bool_comb 
{
	typedef bool result_type;
	template <class InputIterator>
	bool operator()(InputIterator first, InputIterator last) const 
	{ 
		while(first!=last)
			if(*first++==false)
				return false;
		return true;
	} 
};

class udp_socket{
    boost::asio::ip::udp::socket soc;
    void *ptr;
    size_t size;
    boost::asio::ip::udp::endpoint ep;
    void on_message(const boost::system::error_code& ec,size_t len);
public:
	boost::signals2::signal<void(udp_socket &soc,boost::asio::ip::udp::endpoint,
			void *,size_t size)> OnMessage;
	udp_socket(boost::asio::io_service &io,size_t buf_size);
	void *write(size_t size,const boost::asio::ip::udp::endpoint &ep);
	void *write(size_t size);
	void accept(int port);
};

class upd_listner{
    boost::asio::io_service &ser;
	int sec;
    boost::asio::ip::udp::socket soc;
    boost::asio::ip::udp::endpoint ep;
    void *ptr;
    size_t size;
	char ch;
    std::map<boost::asio::ip::udp::endpoint,std::unique_ptr<timer>> timers;
    void on_message(const boost::system::error_code& ec,size_t len);
	void on_timeout(const boost::asio::ip::udp::endpoint &end);
public:
    boost::signals2::signal<void(const boost::asio::ip::udp::endpoint &)> OnTimeOut;
    boost::signals2::signal<bool(const boost::asio::ip::udp::endpoint &,char *,size_t),bool_comb> OnConnect;
    upd_listner(size_t out_time,int port,size_t buf_size,boost::asio::io_service &io);
};
