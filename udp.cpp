#include "udp.hpp"
#include <utility>
#include <iostream>
using namespace boost::asio;
using ip::udp;
using boost::bind;
using std::make_pair;
using std::make_unique;
class nothing{
public:
    template <class ...Arg>
    void operator()(Arg ...arg){}
};

upd_listner::upd_listner(size_t out_time, int port, size_t buf_size,boost::asio::io_service &io):
        ser(io),sec(out_time),soc(ser,udp::endpoint(udp::v4(), port)),ptr(malloc(size)),size(buf_size)
{
    soc.async_receive_from(buffer(ptr,size),ep,bind(&upd_listner::on_message,this,_1,_2));
}

void upd_listner::on_message(const boost::system::error_code& ec, size_t len)
{
    auto it=timers.find(ep);
    if(it==timers.end()){
        auto it_i=timers.insert(make_pair(ep,make_unique<timer>(ser,sec)));
		auto it=it_i.first;
		it->second->OnTimeOut.connect(bind(upd_listner::on_timeout,this,it->first));
		ch=OnConnect(it->first,(char *)ptr,len);
		std::cout << it->first << std::endl;
		soc.async_send_to(buffer(&ch,1),it->first,nothing());
	}
    else{
		it->second->reset();
		soc.async_send_to(buffer(&ch,1),it->first,nothing());
	}
    soc.async_receive_from(buffer(ptr,size),ep,bind(&upd_listner::on_message,this,_1,_2));
}

void upd_listner::on_timeout(const boost::asio::ip::udp::endpoint& end)
{
	OnTimeOut(end);
	ser.post([this,&end](){timers.erase(end);});
}

void udp_socket::on_message(const boost::system::error_code& ec, size_t len)
{
	if(ec!=0)
		return;
	OnMessage(*this,ep,ptr,len);
	soc.async_receive_from(buffer(ptr,size),ep,bind(&udp_socket::on_message,this,_1,_2));
}

void* udp_socket::write(size_t size, const boost::asio::ip::udp::endpoint& ep)
{
	void *ptr=malloc(size);
	soc.get_io_service().post([ptr,ep,this,size](){
			soc.async_send_to(buffer(ptr,size),ep,
					[ptr](const boost::system::error_code& ec, size_t len){
						free(ptr);
					});
	});
}

udp_socket::udp_socket(boost::asio::io_service& io, size_t buf_size):soc(io),size(buf_size)
{
	soc.open(udp::v4());
}

void udp_socket::accept(int port)
{
	soc.bind(boost::asio::ip::udp::endpoint(ip::address_v4(),port));
	soc.async_receive_from(buffer(ptr,size),ep,bind(&udp_socket::on_message,this,_1,_2));
}

void* udp_socket::write(size_t size)
{
	return write(size,ep);
}
