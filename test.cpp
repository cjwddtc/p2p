#include "service.hpp"
#include "session.hpp"
#include <boost/asio.hpp>
#include <chrono>
using boost::asio::deadline_timer;
int main(){
	int n=5;
	service_wrap<services_> ser(n);
	
deadline_timer d(ser());
	d.expires_from_now(boost::posix_time::seconds(5));
	d.async_wait([](const boost::system::error_code &ec){
		printf("asd\n");});
	ser.run();
	getchar();
	ser.stop();
	ser.des();
}