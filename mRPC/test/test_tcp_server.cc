#include "log.h"
#include "net_addr.h"
#include "tcp_server.h"
#include <memory>

int main() {

    mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
    mrpc::Config::GetGlobalConfig();

    std::shared_ptr<mrpc::IPNetAddr> addr =
        std::make_shared<mrpc::IPNetAddr>("127.0.0.1", 12345);
    DEBUGLOG("create addr %s", addr->toString().c_str());

    mrpc::TcpServer tcp_server(addr);

    int i = 0;
    std::shared_ptr<mrpc::TimerEvent> timer_event =
        std::make_shared<mrpc::TimerEvent>(1000, true, [&]() {
            INFOLOG("trigger timer event, count=%d", i++);
            DEBUGLOG("*******************************************");
        });

    mrpc::Eventloop::GetThreadLocalEventloop()->addTimerEvent(timer_event);

    tcp_server.start();
}