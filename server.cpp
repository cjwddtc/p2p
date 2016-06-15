#include "udp.hpp"
#include <iostream>
#include <list>
#include <tuple>
using namespace boost::asio;
using ip::udp;
using boost::function;

bool find_string(std::tuple<std::string,unsigned long,int> &a,std::string &str){
	return std::get<0>(a)==str;
}

bool find_addr(std::tuple<std::string,unsigned long,int> &a,unsigned long addr){
	return std::get<1>(a)==addr;
}
class server
{
	std::list<std::tuple<std::string,unsigned long,int>> onlines;
public:
	bool add(const boost::asio::ip::udp::endpoint &ep,char *ptr,size_t size){
		if(!ep.address().is_v4()) return false;
		ptr[size]=0;
		char *p=strchr(ptr,'\n');
		if(p==0) return false;
		*p++=0;
		unsigned long addr=ep.address().to_v4().to_ulong();
		int port=atoi(p);
		std::string name(ptr);
		if(std::find_if(onlines.begin(),onlines.end(),bind(&find_addr,_1,addr))!=onlines.end()) return false;
		if(std::find_if(onlines.begin(),onlines.end(),bind(&find_string,_1,name))!=onlines.end()) return false;
		onlines.push_back(std::make_tuple(name,addr,port));
		printf("%lx,%d,%s",addr,port,ptr);
		return true;
	}
	void remove(const boost::asio::ip::udp::endpoint &ep)
	{
		unsigned long addr=ep.address().to_v4().to_ulong();
		onlines.erase(std::find_if(onlines.begin(),onlines.end(),bind(&find_addr,_1,addr)));
	}
};

int main(){
	io_service ser;
	upd_listner li(4,12345,1024,ser);
	server serv;
	li.OnConnect.connect(bind(&server::add,&serv,_1,_2,_3));
	li.OnTimeOut.connect(bind(&server::remove,&serv,_1));
	ser.run();
}