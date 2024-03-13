#include "config.h"
#include "eventloop.h"
#include "fd_event.h"
#include "log.h"
#include "timer_event.h"

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

    mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
    mrpc::Config::GetGlobalConfig();

    // std::unique_ptr<mrpc::Eventloop> eventloop =
    // std::unique_ptr<mrpc::Eventloop>(new mrpc::Eventloop());

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        ERRORLOG("listenfd = -1");
        exit(0);
    }

    sockaddr_in addr{};
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(12310);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(listenfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (rt != 0) {
        ERRORLOG("bind error");
        exit(1);
    }

    rt = listen(listenfd, 100);
    if (rt != 0) {
        ERRORLOG("listen error");
        exit(1);
    }

    std::shared_ptr<mrpc::FdEvent> event = std::make_shared<mrpc::FdEvent>(listenfd);
    event->listen(mrpc::FdEvent::IN_EVENT, [listenfd](){
        sockaddr_in peer_addr{};
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, sizeof(peer_addr));

        int clientfd = accept(
            listenfd, reinterpret_cast<sockaddr *>(&peer_addr), &addr_len);
        DEBUGLOG("success get client fd[%d], peer addr: [%s:%d]", clientfd,
                 inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
    });

    mrpc::Eventloop::GetThreadLocalEventloop()->addEpollEvent(event);

    int i = 0;
    std::shared_ptr<mrpc::TimerEvent> timer_event =
        std::make_shared<mrpc::TimerEvent>(1000, true, [&i]() {
            INFOLOG("trigger timer event, count=%d", i++);
        });

    mrpc::Eventloop::GetThreadLocalEventloop()->addTimerEvent(timer_event);
    mrpc::Eventloop::GetThreadLocalEventloop()->loop();

    return 0;
}