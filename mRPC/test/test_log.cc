#include "config.h"
#include "log.h"

#include <thread>
#include <unistd.h>

int main() {

    mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
    mrpc::Config::GetGlobalConfig();

    std::thread t1([]() {
        for (int i = 0; i < 20; i++) {
            DEBUGLOG("debug this is thread in %s", "fun1");
            INFOLOG("info this is thread in %s", "fun1");

            sleep(1);
        }
    });

    std::thread t2([]() {
        for (int i = 0; i < 20; i++) {
            DEBUGLOG("debug this is thread in %s", "fun2");
            INFOLOG("info this is thread in %s", "fun2");

            sleep(1);
        }
    });

    t1.join();
    t2.join();
    return 0;
}