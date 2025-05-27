#include <boost/asio.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <iostream>

int main()
{
  boost::asio::io_context io;
  boost::asio::ip::tcp::acceptor acceptor(io,{boost::asio::ip::tcp::v4(), 8080});

  while(true)
  {
    boost::asio::ip::tcp::socket socket(io);
    acceptor.accept(socket);

    boost::asio::streambuf buffer;
    boost::asio::read_until(socket, buffer, "\r\n\r\n");

    std::string request(boost::asio::buffers_begin(buffer.data()), boost::asio::buffers_end(buffer.data()));

    if(true)
    {
      std::cout <<"BLOCKED" <<std::endl;
      socket.write_some(boost::asio::buffer("HTTP/1.1 403 Forbidden\r\n\r\n"));
    }
    else 
    {
      boost::asio::ip::tcp::socket backend(io);
      backend.connect({boost::asio::ip::make_address("127.0.0.1"), 8000});
      backend.write_some(buffer.data());
    }
  }
  return 0;
}
