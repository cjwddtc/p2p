#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "timeout.hpp"
#include <sstream>
#include <iostream>
#include "udp.hpp"
using namespace boost::asio;
using ip::udp;
using boost::function;

class nothing{
public:
    template <class ...Arg>
    void operator()(Arg ...arg){}
};

class udp_timer
{
	udp::socket soc;
    udp::endpoint ep;
    udp::endpoint ept;
	bool flag;
	char buf;
	timer t;
	timer tt;
public:
    boost::signals2::signal<void(udp_timer &)> OnTimeOut;
    boost::signals2::signal<void(udp_timer &)> OnConnect;
	udp_timer(io_service &ser,const ip::address &addr,int port,int timeout):
			soc(ser),ep(addr,port),t(ser,timeout/2),tt(ser,timeout),flag(true){
		t.OnTimeOut.connect([](timer &t){t.reset();});
		tt.OnTimeOut.connect([this](auto &a){printf("off\n");OnTimeOut(*this);});
		soc.open(udp::v4());
	}
	void start(std::string name){
		soc.async_send_to(buffer(name),ep,bind(&udp_timer::on_write,this,_1,_2));
		t.OnTimeOut.connect([this,name](timer &t){
			soc.async_send_to(buffer(name),ep,bind(&udp_timer::on_write,this,_1,_2));
		});
	}
	void on_write(const boost::system::error_code& ec, size_t len){
		soc.async_receive_from(buffer(&buf,1),ept,bind(&udp_timer::on_read,this,_1,_2));
	}
	void on_read(const boost::system::error_code& ec, size_t len){
		tt.reset();
		if(ec==0){
			assert(ept==ep);
		}
		if(ec!=0 || buf==0){
			OnTimeOut(*this);
		}
		else{
			tt.reset();
			if(flag){
				flag=false;
				OnConnect(*this);
			}
		}
	}
};

int main(){
	io_service ser;
    udp_timer ti(ser,ip::address_v4::from_string("127.0.0.1"),12345,4);
	std::stringstream ss;
	ss<<"qwe"<<'\n'<<12346;
	ti.start(ss.str());
	ti.OnConnect.connect([](udp_timer &){printf("connect\n");});
	ti.OnTimeOut.connect([](udp_timer &){printf("timeout\n");});
	ser.run();
}