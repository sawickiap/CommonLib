SOURCES = Common/Base.cpp \
	Common/Config.cpp \
	Common/DateTime.cpp \
	Common/Dator.cpp \
	Common/Error.cpp \
	Common/Files.cpp \
	Common/FreeList.cpp \
	Common/Logger.cpp \
	Common/Math.cpp \
	Common/Profiler.cpp \
	Common/Stream.cpp \
	Common/Threads.cpp \
	Common/Tokenizer.cpp \
	Common/ZlibUtils.cpp \
	ConsoleTest/ConsoleTest.cpp
	
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

CXXFLAGS = -O2 -Wall

all: DependRule ConsoleTestRule

ConsoleTestRule: $(OBJECTS)
		$(CXX) -o ConsoleTestExe $(OBJECTS) -l z -l pthread

clean:
		rm -rf $(OBJECTS) ConsoleTestExe

DependRule: $(SOURCES)
		$(CXX) -M $(SOURCES) $(CXXFLAGS) > Depend

-include Depend

