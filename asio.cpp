#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <stdint.h>
#include <map>
#include "asio.h"
using namespace boost;
using namespace asio;
using boost::system::error_code;
using ip::tcp;
using boost::thread;
using boost::bind;
using boost::function;
using namespace boost::placeholders;
template <class F>
using signal_=boost::signals2::signal<F>;
using std::make_pair;
void signals::start(io_service &ser,int port){
	acc=new tcp::acceptor(ser,tcp::endpoint(tcp::v4(),port));
	(new session(ser,this))->accept(acc);
}