SRC=common.cc main.cc mybuslistener.cc mybusobject.cc servicebusattachment.cc
OBJ=common.o main.o mybuslistener.o mybusobject.o servicebusattachment.o

CXXFLAGS = -Wall -pipe -std=c++11 -fno-rtti -fno-exceptions -Wno-long-long -Wno-deprecated -g -DQCC_OS_LINUX -DQCC_OS_GROUP_POSIX -DQCC_CPU_X86

LIBS = -lalljoyn -lstdc++ -lcrypto -lpthread -lrt 

aj_client: clientebusattachment.o common.o  main.o  mybuslistener.o  mybusobject.o
	$(CXX) -o $@ $^ -L../alljoyn-lib $(LIBS)

clientebusattachment.o: clientbusattachment.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

common.o: common.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

main.o: main.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

mybuslistener.o: mybuslistener.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

mybusobject.o: mybusobject.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

clean:
	-rm -rf *.o
