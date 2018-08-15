#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <array>
#include "RacksManager.h"
using boost::asio::ip::tcp;
using namespace boost::asio;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
  typedef boost::shared_ptr<tcp_connection> pointer;
  static pointer create(boost::asio::io_context& io_context)
  {
    return pointer(new tcp_connection(io_context));
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void addRacksManager(RacksManager* rack)
  {
    m_racksmanager=rack;
  }
  void read()
  {
      async_read(socket_, boost::asio::buffer(m_message),
        boost::bind(&tcp_connection::handle_read,shared_from_this(), boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  void setMessage(const std::string& retour)
  {
    m_message1=retour;
  }  
  void write()
  {
       async_write(socket_, boost::asio::buffer(m_message1),boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  void start()
  {
    /*async_write(socket_, boost::asio::buffer(message_),boost::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));*/
    write();
    read();
    /*async_read(socket_, boost::asio::buffer(message_client),
        boost::bind(&tcp_connection::handle_read,shared_from_this(), boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));*/
    write();
  }

private:
  tcp_connection(boost::asio::io_context& io_context):socket_(io_context){}

  void handle_write(const boost::system::error_code& error,size_t bytes_transferred){}
  void handle_read(const boost::system::error_code& error,size_t bytes_transferred)
  {
     Value command(m_message.data());
     std::vector<Value> p=command.Tokenize("*");
     if(m_racksmanager==nullptr) std::cout<<"ERROR"<<std::endl;
     if(p[0].String()=="Initialize")
     {
         m_racksmanager->Initialize();
         setMessage("Good");
     }
     else if(p[0].String()=="Connect") m_racksmanager->connect();
     else if(p[0].String()=="Disconnect") m_racksmanager->disconnect();
     else if(p[0].String()=="On") m_racksmanager->on();
     else if(p[0].String()=="Off") m_racksmanager->off();
     else
     {
        std::cout<<"Unknown command"<<std::endl;
     }
}
  tcp::socket socket_;
  std::string m_message=std::string(50000,'\0');
  std::string m_message1=std::string(50000,'\0');
  RacksManager* m_racksmanager{nullptr};
};

class tcp_server
{
public:
  tcp_server(boost::asio::io_context& io_context,RacksManager* manager): acceptor_(io_context, tcp::endpoint(tcp::v4(),5555))
  {
    start_accept();
    m_racksmanager=manager;
  }
  ~tcp_server()
  {
    if(m_racksmanager!=nullptr) 
    {
        delete m_racksmanager;
        m_racksmanager=nullptr;
    }
  }
private:
  void start_accept()
  {
    tcp_connection::pointer new_connection =tcp_connection::create(acceptor_.get_executor().context());
    acceptor_.async_accept(new_connection->socket(),boost::bind(&tcp_server::handle_accept, this, new_connection,boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection,const boost::system::error_code& error)
  {
    if (!error)
    {
      new_connection->addRacksManager(m_racksmanager);
      new_connection->start();
    }
    start_accept();
  }
  tcp::acceptor acceptor_;
  RacksManager* m_racksmanager{nullptr};
};

int main()
{
  try
  {
    RacksManager manager;
    boost::asio::io_context io_context;
    tcp_server server(io_context,&manager);
    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
