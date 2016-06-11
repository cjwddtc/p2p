#include "session.hpp"
#include <assert.h>
#include <memory>
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using ip::tcp;

void session::test()
{
    if(write_count==0){
        if(valid==1){
            soc.close();
            valid=0;
        }
        if(read_count==0 && valid==0){
            delete this;
        }
    }
}

void session::on_read(const error_code& ec, size_t len)
{
    if(len)
        ses->OnRead(this,(char *)ptr,len);
    if(ec!=0){
        ses->OnError(this,std::ref(ec),0);
        close();
    }else
        read();
    assert(read_count==1);
    read_count=0;
    test();
}

void session::on_write(const error_code& er, size_t len, char* ptr, int id)
{
    ses->OnWrite(this,(char *)ptr,len,id);
    if(er!=0)
        ses->OnError(this,std::ref(er),id);
    free(ptr);
    write_count--;
    test();
}

void session::write(size_t size, int id)
{
    if(valid){
        char *ptr=(char *)malloc(size);
        write_count++;
        assert(id>0);
        soc.get_io_service().post([this,ptr,size,id]{
			soc.async_send(buffer(ptr,size),bind(&session::on_write,this,_1,_2,ptr,id));
		});
    }
}

session::session(io_service& ser, signal* se):soc(ser),ses(se),ptr(malloc(1024)),size(1024),read_count(0),write_count(0),valid(2){}

void session::read()
{
    read_count++;
    soc.async_receive(buffer(ptr,size),bind(&session::on_read,this,_1,_2));
}

void session::close()
{
    valid=valid==0?0:1;
}

session::~session()
{
    ses->OnClose(this);
    free(ptr);
}

void session::accept(signal *ses,int port,boost::function<io_service&()> func)
{
	auto acc=new tcp::acceptor(func());
	session *s=new session(func(),ses);
	acc->async_accept(s->soc,bind(&session::on_accept,s,_1,acc,func));
}

void session::on_accept(const boost::system::error_code& er,tcp::acceptor *acc,boost::function<io_service&()> func)
{
	on_init(std::ref(er));
	if(er!=0){
		session *s=new session(func(),ses);
		acc->async_accept(s->soc,bind(&session::on_accept,s,_1,acc,func));
	}else{
        delete acc;
    }
}

void session::on_init(const boost::system::error_code& er)
{
	ses->OnInit(this);
	if(er!=0){
		ses->OnError(this,std::ref(er),-1);
		delete this;
	}
}

void session::connect(signal *ses,boost::asio::ip::tcp::endpoint ep,boost::function<boost::asio::io_service&()> func)
{
	session *s=new session(func(),ses);
	s->soc.async_connect(ep,bind(&session::on_init,s,_1));
}