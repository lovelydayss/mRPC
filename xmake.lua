--global define begin
add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_ldflags("-lpthread")
--global define end

-- pakage test begin
target("test_jsoncpp")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_files("mRPC/test/test_jsoncpp.cc")

target("test_ffmpeg")
    set_kind("binary")
   
    add_links("avutil", "avcodec", "avformat", "avdevice", "avfilter", "swscale", "swresample", "postproc")
    --add_links("avcodec")
    add_includedirs("/usr/local/include/ffmpeg")
    add_linkdirs("/usr/local/lib")

    add_files("mRPC/test/test_ffmpeg.cc")
target("test_gtest")
    set_kind("binary")
   
    add_links("gtest")
    add_includedirs("mRPC/include/gtest/include")
    add_linkdirs("mRPC/include/gtest/lib")

    add_files("mRPC/test/test_gtest.cc")

-- pakage test end

-- rpc test begin
target("test_log")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_includedirs("mRPC/include/fmtlog/include")
    add_includedirs("mRPC/src/common/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/test/test_log.cc")


target("test_eventloop")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/test/test_eventloop.cc")

target("test_eventloop_timer")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/test/test_eventloop_timer.cc")

target("test_io_thread")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/test/test_io_thread.cc")

target("test_net_addr")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    add_includedirs("mRPC/src/net/tcp/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/net_addr.cc")
    add_files("mRPC/test/test_net_addr.cc")

target("test_tcp_server")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_links("protobuf")
    add_includedirs("mRPC/include/protobuf/include")
    add_linkdirs("mRPC/include/protobuf/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    add_includedirs("mRPC/src/net/tcp/include")
    add_includedirs("mRPC/src/net/codec/include")
    add_includedirs("mRPC/src/net/rpc/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/src/net/tcp/*.cc")
    add_files("mRPC/src/net/codec/*.cc")
    add_files("mRPC/src/net/rpc/*.cc")
    add_files("mRPC/test/test_tcp_server.cc")

target("test_tcp_client")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_links("protobuf")
    add_includedirs("mRPC/include/protobuf/include")
    add_linkdirs("mRPC/include/protobuf/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    add_includedirs("mRPC/src/net/tcp/include")
    add_includedirs("mRPC/src/net/codec/include")
    add_includedirs("mRPC/src/net/rpc/include")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/src/net/tcp/*.cc")
    add_files("mRPC/src/net/codec/*.cc")
    add_files("mRPC/src/net/rpc/*.cc")
    add_files("mRPC/test/test_tcp_client.cc")

target("test_rpc_server")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_links("protobuf")
    add_includedirs("mRPC/include/protobuf/include")
    add_linkdirs("mRPC/include/protobuf/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    add_includedirs("mRPC/src/net/tcp/include")
    add_includedirs("mRPC/src/net/codec/include")
    add_includedirs("mRPC/src/net/rpc/include")
    add_includedirs("mRPC/test")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/src/net/tcp/*.cc")
    add_files("mRPC/src/net/codec/*.cc")
    add_files("mRPC/src/net/rpc/*.cc")
    add_files("mRPC/test/order.pb.cc")
    add_files("mRPC/test/test_rpc_server.cc")


target("test_rpc_client")
    set_kind("binary")

    add_links("jsoncpp")
    add_includedirs("mRPC/include/jsoncpp/include")
    add_linkdirs("mRPC/include/jsoncpp/lib")

    add_links("protobuf")
    add_includedirs("mRPC/include/protobuf/include")
    add_linkdirs("mRPC/include/protobuf/lib")

    add_includedirs("mRPC/include/fmtlog/include")

    add_includedirs("mRPC/src/common/include")
    add_includedirs("mRPC/src/net/include")
    add_includedirs("mRPC/src/net/tcp/include")
    add_includedirs("mRPC/src/net/codec/include")
    add_includedirs("mRPC/src/net/rpc/include")
    add_includedirs("mRPC/test")
    
    add_files("mRPC/src/common/*.cc")
    add_files("mRPC/src/net/*.cc")
    add_files("mRPC/src/net/tcp/*.cc")
    add_files("mRPC/src/net/codec/*.cc")
    add_files("mRPC/src/net/rpc/*.cc")
    add_files("mRPC/test/order.pb.cc")
    add_files("mRPC/test/test_rpc_client.cc")

-- rpc test end

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro defination
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

