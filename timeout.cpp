#include "timeout.hpp"
#include <boost/functional.hpp>

using boost::bind;

timer::timer(boost::asio::io_service& io, int sec_):sec(sec_),clo(io,sec){
	start();
}

void timer::reset()
{
	clo.cancel();
	clo.expires_from_now(sec);
	start();
}

void timer::on_timeout(const boost::system::error_code& er)
{
	if(er==0)
		OnTimeOut(*this);
}

void timer::start()
{
	clo.async_wait(bind(timer::on_timeout,this,_1));
}
