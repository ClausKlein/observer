CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O1
CPPFLAGS = -I include -I/usr/local/include
LDFLAGS  =

EXAMPLEDIR = examples
TESTDIR = test
BENCHMARKDIR = benchmark
OBJDIR = obj
OUTDIR = out

TESTSOURCES = $(shell find $(TESTDIR) -type f -name '*.cpp')
TESTOBJECTS = $(patsubst $(TESTDIR)/%.cpp, $(OBJDIR)/%.o, $(TESTSOURCES))
TESTS = $(patsubst $(OBJDIR)/%.o, $(OUTDIR)/%, $(TESTOBJECTS))

EXAMPLESOURCES = $(shell find $(EXAMPLEDIR) -type f -name '*.cpp')
EXAMPLEOBJECTS = $(patsubst $(EXAMPLEDIR)/%.cpp, $(OBJDIR)/%.o, $(EXAMPLESOURCES))
EXAMPLES = $(patsubst $(OBJDIR)/%.o, $(OUTDIR)/%, $(EXAMPLEOBJECTS))

BENCHMARKSOURCES = $(shell find $(BENCHMARKDIR) -type f -name '*.cpp')
BENCHMARKOBJECTS = $(patsubst $(BENCHMARKDIR)/%.cpp, $(OBJDIR)/%.o, $(BENCHMARKSOURCES))
BENCHMARKS = $(patsubst $(OBJDIR)/%.o, $(OUTDIR)/%, $(BENCHMARKOBJECTS))

.PHONY: all clean tests examples benchmarks
all: tests examples benchmarks

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

tests: $(OBJDIR) $(OUTDIR) $(TESTS)

$(TESTS): $(TESTOBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(patsubst $(OUTDIR)%, $(OBJDIR)%.o, $@)
	$(@)

$(TESTOBJECTS): $(TESTSOURCES)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(patsubst $(OBJDIR)%.o, $(TESTDIR)%.cpp, $@) -o $@

examples: $(OBJDIR) $(OUTDIR) $(EXAMPLES)

$(EXAMPLES): LDFLAGS += -fsanitize=thread
$(EXAMPLES): CXXFLAGS += -fsanitize=thread
$(EXAMPLES): $(EXAMPLEOBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(patsubst $(OUTDIR)%, $(OBJDIR)%.o, $@)
	$(@)

$(EXAMPLEOBJECTS): $(EXAMPLESOURCES)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(patsubst $(OBJDIR)%.o, $(EXAMPLEDIR)%.cpp, $@) -o $@

benchmarks: $(OBJDIR) $(OUTDIR) $(BENCHMARKS)

$(BENCHMARKS): $(BENCHMARKOBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(patsubst $(OUTDIR)%, $(OBJDIR)%.o, $@)
	$(@)

$(BENCHMARKOBJECTS): $(BENCHMARKSOURCES)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(patsubst $(OBJDIR)%.o, $(BENCHMARKDIR)%.cpp, $@) -o $@

clean:
	rm -rf $(OBJDIR)
	rm -rf $(OUTDIR)
