BINDIR = bin
BUILDDIR = build
SRCDIR = src

# don't include the unit test file
SRC = $(shell find $(SRCDIR) -type f -name '*.cpp' | perl -ne 'print if not /src\/unit\_test\.cpp/g')
OBJ = $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SRC:.cpp=.o))
DEPS = $($(OBJ):%.o=.d)
# all source files except for main
TEST_SRC = $(shell find $(SRCDIR) -type f -name '*.cpp' | perl -ne 'print if not /src\/main\.cpp/g')
TEST_OBJ = $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(TEST_SRC:.cpp=.o))

CFLAGS = -g -Wall -std=c++11
INC = -Iinclude
CPP = g++
LIBS =
NAME = coolcompiler
UTESTS = unit_tests
TARGET = $(BINDIR)/$(NAME)

ifeq ($(CONF), debug)
	CFLAGS += -DDEBUG
endif

#these do not create build dependencies
.PHONY: clean create_bin_build all

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "Linking"
	$(CPP) -o $@ $^ $(LIBS)
	@ln -sf $@ $(NAME)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp create_bin_build
	@echo "Building"
	$(CPP) $(CFLAGS) $(INC) -o $@ -MMD -MP -MF ${@:.o=.d} $< -c

utests: $(TEST_OBJ)
	@echo "Linking tests"
	$(CPP) -o bin/$@ $^ $(LIBS)
	@ln -sf bin/$@ $(UTESTS)

create_bin_build:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BUILDDIR)

clean:
	rm -rf bin build $(NAME) $(UTESTS)

-include $(DEPS)
