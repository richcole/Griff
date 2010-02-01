CCC    = g++-3.3
CC     = gcc-3.3
AR     = ar
RANLIB = ranlib
MOC    = /usr/bin/moc

SARL_MAJOR_VERSION=0
SARL_MINOR_VERSION=1
SARL_RELEASE_VERSION=0

PREFIX=/usr/local
INSTALL_LIB_DIR=$(PREFIX)/lib
INSTALL_INC_DIR=$(PREFIX)/include/sarl

LDFLAGS = -lgc -lqt-mt

# CPPFLAGS = -Wall -DNDEBUG -O2
# CPPFLAGS = -Wall -ggdb

CPPFLAGS = -Wall -ggdb -fPIC
CPPFLAGS += -I/usr/include/qt3
