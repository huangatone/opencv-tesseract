#include <boost/bind.hpp>

#include "session.h"


Session::Session(boost::asio::io_service& _ioService)
:socket_(_ioService) {
	memset(msg_, 0, sizeof(msg_));
}


Session::~Session(void)
{
}


void Session::start(void) {
	// 告诉链接成功的客户端，你想要的信息。
	char msg[256] = "001:Connect Succeed! Please tell me with 10 bytes, the total data and the size of each package, example:128 1024";
	boost::asio::async_write(socket_, boost::asio::buffer(msg, strlen(msg)),
		boost::bind(&Session::init_handler, shared_from_this(),
		boost::asio::placeholders::error));
}

boost::asio::ip::tcp::socket& Session::socket(void) {
	return socket_;
}


// 第一个协议包
void Session::init_handler(const boost::system::error_code& _error) {
	if (_error) {
		return;
	}
	// 读取客户端发来的 10 bytes，确定单个包的大小以及数据总大小
	boost::asio::async_read(socket_, boost::asio::buffer(msg_, 10),
		boost::bind(&Session::analyse_handler, shared_from_this(),
		boost::asio::placeholders::error));

}

void Session::analyse_handler(const boost::system::error_code& _error) {
	if (_error) {
		return;
	}
	

	// 格式化保存协议包数据
	std::stringstream io(msg_);
	 maxSize_ = 1000;
	 sumSize_ = 100;

	// 发送接收请求信息
	char msg[REPLY_SIZE];
	sprintf(msg, "001:is ok, data remaining %d.", sumSize_);
	std::cout << msg;
	boost::asio::async_write(socket_, boost::asio::buffer(msg, REPLY_SIZE),
		boost::bind(&Session::write_handler, shared_from_this(),
		boost::asio::placeholders::error));
}


// 完成数据传输
void Session::done_handler(const boost::system::error_code& _error) {
	if (_error) {
		return;
	}
	currentMsg_ += msg_;

	std::cout << "recv succeed: " << currentMsg_ << std::endl;
	// 发送信息到回调
	if (!currentMsg_.empty() && callback_ != nullptr) {
		callback_(socket_.remote_endpoint().address().to_string(), currentMsg_);
		currentMsg_.clear();
	}
	memset(msg_, 0, sizeof(msg_));

	char msg[32] = "send to client, done.";
	boost::asio::async_write(socket_, boost::asio::buffer(msg, REPLY_SIZE),
		boost::bind(&Session::init_handler, shared_from_this(),
		boost::asio::placeholders::error));
}

void Session::read_handler(const boost::system::error_code& _error, size_t _readSize) {
	if (_error) {
		return;
	}
	// 数据处理
	currentMsg_ += msg_;

	std::cout << "Recv: " << msg_ << std::endl;
	
	memset(msg_, 0, sizeof(msg_));

	
	char msg[REPLY_SIZE] = "Your msg is recv\n";
	//sprintf(msg, "001:%d.", sumSize_);
	/*
	boost::asio::async_write(socket_, boost::asio::buffer(msg, REPLY_SIZE),
		boost::bind(&Session::write_handler, shared_from_this(),
		boost::asio::placeholders::error));

	boost::asio::async_read(socket_, boost::asio::buffer(msg_, maxSize_),
		boost::bind(&Session::read_handler, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

*/	
	
	
}
void Session::write_handler(const boost::system::error_code& _error) {
	if (_error) {
		return;
	}
std::cout << "msg sended: " <<  std::endl;
	/*boost::asio::async_read(socket_, boost::asio::buffer(msg_, maxSize_),
		boost::bind(&Session::read_handler, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));*/
}