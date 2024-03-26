#include "tcp_acceptor.h"
#include "net_addr.h"
#include "utils.h"
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

MRPC_NAMESPACE_BEGIN

TcpAcceptor::TcpAcceptor(const NetAddr::s_ptr& local_addr)
    : m_local_addr(local_addr) {
	if (!local_addr->checkValid()) {
		ERRORFMTLOG("invalid local addr {}", local_addr->toString().c_str());
		exit(0);
	}

	m_family = m_local_addr->getFamily();
	m_listenfd = socket(m_family, SOCK_STREAM, 0);

	if (m_listenfd < 0) {
		ERRORFMTLOG("invalid listenfd {}", m_listenfd);
		exit(0);
	}

	// 设置重用本地地址及端口
	int val = 1;
	if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) !=
	    0) {
		ERRORFMTLOG("setsockopt REUSEADDR error, errno={}, error={}", errno,
		         strerror(errno));
	}

	socklen_t len = m_local_addr->getSockLen();
	if (bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0) {
		ERRORFMTLOG("bind error, errno={}, error={}", errno, strerror(errno));
		exit(0);
	}

	if (listen(m_listenfd, 1000) != 0) {
		ERRORFMTLOG("listen error, errno={}, error={}", errno, strerror(errno));
		exit(0);
	}
}

std::pair<int, NetAddr::s_ptr> TcpAcceptor::accept() const {

	if (m_family == AF_INET) {
		sockaddr_in client_addr{};
		memset(&client_addr, 0, sizeof(client_addr));
		socklen_t client_addr_len = sizeof(client_addr);

		int client_fd =
		    ::accept(m_listenfd, reinterpret_cast<sockaddr*>(&client_addr),
		             &client_addr_len);
		if (client_fd < 0) {
			ERRORFMTLOG("accept error, errno={}, error={}", errno,
			         strerror(errno));
		}

		IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
		INFOFMTLOG("A client have accpeted succ, peer addr [{}]",
		        peer_addr->toString().c_str());

		return std::make_pair(client_fd, peer_addr);
	} else {
		return std::make_pair(-1, nullptr);
	}
}

MRPC_NAMESPACE_END