#include "config.h"
#include "eventloop.h"
#include "fd_event.h"
#include "timer_event.h"
#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

	mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	// std::unique_ptr<mrpc::EventLoop> EventLoop =
	// std::unique_ptr<mrpc::EventLoop>(new mrpc::EventLoop());

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		ERRORFMTLOG("listenfd = -1{}","");
		exit(0);
	}

	sockaddr_in addr{};
	memset(&addr, 0, sizeof(addr));

	addr.sin_port = htons(12310);
	addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &addr.sin_addr);

	int ret = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
	if (ret != 0) {
		ERRORFMTLOG("bind error");
		exit(1);
	}

	ret = listen(listenfd, 100);
	if (ret != 0) {
		ERRORFMTLOG("listen error");
		exit(1);
	}

	mrpc::FdEvent::s_ptr event = std::make_shared<mrpc::FdEvent>(listenfd);
	event->listen(mrpc::FdEvent::IN_EVENT, [listenfd]() {
		sockaddr_in peer_addr{};
		socklen_t addr_len = sizeof(peer_addr);
		memset(&peer_addr, 0, sizeof(peer_addr));

		int clientfd = accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr),
		                      &addr_len);
		DEBUGFMTLOG("success get client fd[{}], peer addr: [{}:{}]", clientfd,
		         inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
	});

	mrpc::EventLoop::GetThreadLocalEventLoop()->addEpollEvent(event);

	int i = 0;
	mrpc::TimerEvent::s_ptr timer_event = std::make_shared<mrpc::TimerEvent>(
	    MILLISECONDS(1000), true, [&i]() { INFOFMTLOG("trigger timer event, count={}", i++); });

	mrpc::EventLoop::GetThreadLocalEventLoop()->addTimerEvent(timer_event);
	mrpc::EventLoop::GetThreadLocalEventLoop()->loop();

	return 0;
}