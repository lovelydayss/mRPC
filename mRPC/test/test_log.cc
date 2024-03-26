#include "config.h"
#include "utils.h"
#include <thread>
#include <unistd.h>

int main() {

	mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	std::thread t1([]() {
		for (int i = 0; i < 20; i++) {
			DEBUGFMTLOG("debug this is thread in {}", "fun1");
			INFOFMTLOG("info this is thread in {}", "fun1");
			sleep(1);
		}
	});

	std::thread t2([]() {
		for (int i = 0; i < 20; i++) {
			DEBUGFMTLOG("debug this is thread in {}", "fun2");
			INFOFMTLOG("info this is thread in {}", "fun2");

			sleep(1);
		}
	});

	t1.join();
	t2.join();
	return 0;
}