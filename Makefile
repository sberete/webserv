CXX      = c++
CXXFLAGS = -Werror -Wextra -Wall -std=c++98 -g3
DEPFLAGS = -MMD -MP
INCLUDES = -Iincludes
NAME     = server
OBJ_DIR  = objs

SRCS    = $(shell find srcs -name '*.cpp')
OBJS    = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPENDS = $(OBJS:.o=.d)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

-include $(DEPENDS)
