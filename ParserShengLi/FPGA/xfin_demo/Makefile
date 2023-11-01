all: clean build sz_demo sh_demo sh_sz_demo

SZTESTSRC = src/sz_demo.cpp
SHTESTSRC = src/sh_demo.cpp
SHSZTESTSRC = src/sh_sz_demo.cpp
INCTEST = -Iinclude
LIBSRC = -Llib -lmdreceiver -lzlog -lcommon -lnetworkctrl -lQt5Core \
	-licuuc -licui18n -licudata -lQt5Network -lpthread -lhv

EXTRAITEM = bin/sz_demo bin/sh_demo bin/sh_sz_demo
CC = g++ -std=c++11

LIB_COMMON = libcommon.so.1.0.5
LIB_COMMON_BASE = lib/libcommon.so
LIB_COMMON_MAIN = lib/libcommon.so.1
LIB_COMMON_SUB = lib/libcommon.so.1.0

LIB_MDRECV = libmdreceiver.so.1.0.5
LIB_MDRECV_BASE = lib/libmdreceiver.so
LIB_MDRECV_MAIN = lib/libmdreceiver.so.1
LIB_MDRECV_SUB = lib/libmdreceiver.so.1.0

LIB_NETCTL = libnetworkctrl.so.1.0.5
LIB_NETCTL_BASE = lib/libnetworkctrl.so
LIB_NETCTL_MAIN = lib/libnetworkctrl.so.1
LIB_NETCTL_SUB = lib/libnetworkctrl.so.1.0

LIB_ZLOG = libzlog.so.1.2.0
LIB_ZLOG_BASE = lib/libzlog.so
LIB_ZLOG_MAIN = lib/libzlog.so.1
LIB_ZLOG_SUB = lib/libzlog.so.1.2

LIB_QTCORE = libQt5Core.so.5.12.10
LIB_QTCORE_BASE = lib/libQt5Core.so
LIB_QTCORE_MAIN = lib/libQt5Core.so.5
LIB_QTCORE_SUB = lib/libQt5Core.so.5.12

LIB_QTNETWORK = libQt5Network.so.5.12.10
LIB_QTNETWORK_BASE = lib/libQt5Network.so
LIB_QTNETWORK_MAIN = lib/libQt5Network.so.5
LIB_QTNETWORK_SUB = lib/libQt5Network.so.5.12

LIB_ICUDATA = libicudata.so.56.1
LIB_ICUDATA_BASE = lib/libicudata.so
LIB_ICUDATA_MAIN = lib/libicudata.so.56

LIB_ICUI18N = libicui18n.so.56.1
LIB_ICUI18N_BASE = lib/libicui18n.so
LIB_ICUI18N_MAIN = lib/libicui18n.so.56

LIB_ICUUC = libicuuc.so.56.1
LIB_ICUUC_BASE = lib/libicuuc.so
LIB_ICUUC_MAIN = lib/libicuuc.so.56

sz_demo: src/sz_demo.cpp
	$(CC) $(SZTESTSRC) $(INCTEST) $(LIBSRC) -o bin/$@

sh_demo: src/sh_demo.cpp
	$(CC) $(SHTESTSRC) $(INCTEST) $(LIBSRC) -o bin/$@

sh_sz_demo: src/sh_sz_demo.cpp
	$(CC) $(SHSZTESTSRC) $(INCTEST) $(LIBSRC) -o bin/$@

clean:
	rm -f $(EXTRAITEM)

# libcommon
	@if [ -f $(LIB_COMMON_BASE) ]; then \
		rm -f $(LIB_COMMON_BASE); \
	fi

	@if [ -f $(LIB_COMMON_MAIN) ]; then \
		rm -f $(LIB_COMMON_MAIN); \
	fi

	@if [ -f $(LIB_COMMON_SUB) ]; then \
		rm -f $(LIB_COMMON_SUB); \
	fi

# libmdreceiver
	@if [ -f $(LIB_MDRECV_BASE) ]; then \
		rm -f $(LIB_MDRECV_BASE); \
	fi

	@if [ -f $(LIB_MDRECV_MAIN) ]; then \
		rm -f $(LIB_MDRECV_MAIN); \
	fi

	@if [ -f $(LIB_MDRECV_SUB) ]; then \
		rm -f $(LIB_MDRECV_SUB); \
	fi

# libnetworkctrl
	@if [ -f $(LIB_NETCTL_BASE) ]; then \
		rm -f $(LIB_NETCTL_BASE); \
	fi

	@if [ -f $(LIB_NETCTL_MAIN) ]; then \
		rm -f $(LIB_NETCTL_MAIN); \
	fi

	@if [ -f $(LIB_NETCTL_SUB) ]; then \
		rm -f $(LIB_NETCTL_SUB); \
	fi

# libzlog
	@if [ -f $(LIB_ZLOG_BASE) ]; then \
		rm -f $(LIB_ZLOG_BASE); \
	fi

	@if [ -f $(LIB_ZLOG_MAIN) ]; then \
		rm -f $(LIB_ZLOG_MAIN); \
	fi

	@if [ -f $(LIB_ZLOG_SUB) ]; then \
		rm -f $(LIB_ZLOG_SUB); \
	fi

# libQt5Core
	@if [ -f $(LIB_QTCORE_BASE) ]; then \
		rm -f $(LIB_QTCORE_BASE); \
	fi

	@if [ -f $(LIB_QTCORE_MAIN) ]; then \
		rm -f $(LIB_QTCORE_MAIN); \
	fi

	@if [ -f $(LIB_QTCORE_SUB) ]; then \
		rm -f $(LIB_QTCORE_SUB); \
	fi

# libQt5Network
	@if [ -f $(LIB_QTNETWORK_BASE) ]; then \
		rm -f $(LIB_QTNETWORK_BASE); \
	fi

	@if [ -f $(LIB_QTNETWORK_MAIN) ]; then \
		rm -f $(LIB_QTNETWORK_MAIN); \
	fi

	@if [ -f $(LIB_QTNETWORK_SUB) ]; then \
		rm -f $(LIB_QTNETWORK_SUB); \
	fi

# libicudata
	@if [ -f $(LIB_ICUDATA_BASE) ]; then \
		rm -f $(LIB_ICUDATA_BASE); \
	fi

	@if [ -f $(LIB_ICUDATA_MAIN) ]; then \
		rm -f $(LIB_ICUDATA_MAIN); \
	fi

# libicui18n
	@if [ -f $(LIB_ICUI18N_BASE) ]; then \
		rm -f $(LIB_ICUI18N_BASE); \
	fi

	@if [ -f $(LIB_ICUI18N_MAIN) ]; then \
		rm -f $(LIB_ICUI18N_MAIN); \
	fi

# libicuuc
	@if [ -f $(LIB_ICUUC_BASE) ]; then \
		rm -f $(LIB_ICUUC_BASE); \
	fi

	@if [ -f $(LIB_ICUUC_MAIN) ]; then \
		rm -f $(LIB_ICUUC_MAIN); \
	fi

build:
	@if [ ! -e bin ]; then \
		mkdir bin; \
	fi

# libcommon
	@if [ ! -f $(LIB_COMMON_BASE) ]; then \
		ln -s $(LIB_COMMON) 			$(LIB_COMMON_BASE); \
	fi

	@if [ ! -f $(LIB_COMMON_MAIN) ]; then \
		ln -s $(LIB_COMMON) 			$(LIB_COMMON_MAIN); \
	fi

	@if [ ! -f $(LIB_COMMON_SUB) ]; then \
		ln -s $(LIB_COMMON) 			$(LIB_COMMON_SUB); \
	fi

# libmdreceiver
	@if [ ! -f $(LIB_MDRECV_BASE) ]; then \
		ln -s $(LIB_MDRECV) 			$(LIB_MDRECV_BASE); \
	fi

	@if [ ! -f $(LIB_MDRECV_MAIN) ]; then \
		ln -s $(LIB_MDRECV) 			$(LIB_MDRECV_MAIN); \
	fi

	@if [ ! -f $(LIB_MDRECV_SUB) ]; then \
		ln -s $(LIB_MDRECV) 			$(LIB_MDRECV_SUB); \
	fi

# libnetworkctrl
	@if [ ! -f $(LIB_NETCTL_BASE) ]; then \
		ln -s $(LIB_NETCTL) 			$(LIB_NETCTL_BASE); \
	fi

	@if [ ! -f $(LIB_NETCTL_MAIN) ]; then \
		ln -s $(LIB_NETCTL) 			$(LIB_NETCTL_MAIN); \
	fi

	@if [ ! -f $(LIB_NETCTL_SUB) ]; then \
		ln -s $(LIB_NETCTL) 			$(LIB_NETCTL_SUB); \
	fi

# libzlog
	@if [ ! -f $(LIB_ZLOG_BASE) ]; then \
		ln -s $(LIB_ZLOG) 			$(LIB_ZLOG_BASE); \
	fi

	@if [ ! -f $(LIB_ZLOG_MAIN) ]; then \
		ln -s $(LIB_ZLOG) 			$(LIB_ZLOG_MAIN); \
	fi

	@if [ ! -f $(LIB_ZLOG_SUB) ]; then \
		ln -s $(LIB_ZLOG) 			$(LIB_ZLOG_SUB); \
	fi

# libQt5Core
	@if [ ! -f $(LIB_QTCORE_BASE) ]; then \
		ln -s $(LIB_QTCORE) 			$(LIB_QTCORE_BASE); \
	fi

	@if [ ! -f $(LIB_QTCORE_MAIN) ]; then \
		ln -s $(LIB_QTCORE) 			$(LIB_QTCORE_MAIN); \
	fi

	@if [ ! -f $(LIB_QTCORE_SUB) ]; then \
		ln -s $(LIB_QTCORE) 			$(LIB_QTCORE_SUB); \
	fi

# libQt5Network
	@if [ ! -f $(LIB_QTNETWORK_BASE) ]; then \
		ln -s $(LIB_QTNETWORK) 			$(LIB_QTNETWORK_BASE); \
	fi

	@if [ ! -f $(LIB_QTNETWORK_MAIN) ]; then \
		ln -s $(LIB_QTNETWORK) 			$(LIB_QTNETWORK_MAIN); \
	fi

	@if [ ! -f $(LIB_QTNETWORK_SUB) ]; then \
		ln -s $(LIB_QTNETWORK) 			$(LIB_QTNETWORK_SUB); \
	fi

# libicudata
	@if [ ! -f $(LIB_ICUDATA_BASE) ]; then \
		ln -s $(LIB_ICUDATA) 			$(LIB_ICUDATA_BASE); \
	fi

	@if [ ! -f $(LIB_ICUDATA_MAIN) ]; then \
		ln -s $(LIB_ICUDATA) 			$(LIB_ICUDATA_MAIN); \
	fi

# libicui18n
	@if [ ! -f $(LIB_ICUI18N_BASE) ]; then \
		ln -s $(LIB_ICUI18N) 			$(LIB_ICUI18N_BASE); \
	fi

	@if [ ! -f $(LIB_ICUI18N_MAIN) ]; then \
		ln -s $(LIB_ICUI18N) 			$(LIB_ICUI18N_MAIN); \
	fi

# libicuuc
	@if [ ! -f $(LIB_ICUUC_BASE) ]; then \
		ln -s $(LIB_ICUUC) 			$(LIB_ICUUC_BASE); \
	fi

	@if [ ! -f $(LIB_ICUUC_MAIN) ]; then \
		ln -s $(LIB_ICUUC) 			$(LIB_ICUUC_MAIN); \
	fi
