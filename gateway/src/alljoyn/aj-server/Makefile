CXXFLAGS = -Wall -pipe -std=c++11 -fno-rtti -fno-exceptions -Wno-long-long -Wno-deprecated -DQCC_OS_GROUP_POSIX

LIBS = -lstdc++ -lalljoyn -lcrypto -lpthread -lrt -lm

aj_server: common.o  main.o  mybuslistener.o  mybusobject.o  servicebusattachment.o
	$(CXX) -o $@ $^ -L../alljoyn-lib $(LIBS)

common.o: common.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ common.cc

main.o: main.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

mybuslistener.o: mybuslistener.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

mybusobject.o: mybusobject.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

servicebusattachment.o: servicebusattachment.cc
	$(CXX) -c $(CXXFLAGS) -I../alljoyn-inc -o $@ $<

clean:
	rm -rf *.o

