#include <boost/asio.hpp>
#include <map>
#include <stdint.h>
#include <boost/signals2.hpp>

class session
{
public:
	class signal
	{
	public:
		boost::signals2::signal<void(session* ses, char* ptr, size_t len)> OnRead;
		boost::signals2::signal<void(session* ses, char* ptr, size_t len, int id)> OnWrite;
		boost::signals2::signal<void(session* ses, const boost::system::error_code& ec, int id)> OnError;
		boost::signals2::signal<void(session* ses)> OnClose;
		boost::signals2::signal<void(session* ses)> OnInit;
	};
private:
	void test();
	void on_read(const boost::system::error_code &ec,size_t len);
	void on_write(const boost::system::error_code &er,size_t len,char *ptr,int id);
	void on_init(const boost::system::error_code &er);
public:
	static void accept(signal *si,int port,boost::function<boost::asio::io_service&()> func);
	void on_accept(const boost::system::error_code& er,
			boost::asio::ip::tcp::acceptor *acc,boost::function<boost::asio::io_service&()> func);
	static void connect(signal *si,boost::asio::ip::tcp::endpoint ep,boost::function<boost::asio::io_service&()> func);
	void write(size_t size,int id);
	session(boost::asio::io_service &ser,signal *se);
	void read();
	void close();
	~session();
	void* ptr;
	size_t size;
	unsigned read_count:1;
	unsigned valid:2;
	unsigned write_count:13;
	signal *ses;
public:
	boost::asio::ip::tcp::socket soc;
};