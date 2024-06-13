NAME		=	codingame_sync_file
BINARY		=	compiled

CXX			=	g++

CPPFLAGS	=	-MMD -g3
CXXFLAGS	=	-std=c++17 -g3 -Wall -Wextra #-Werror

#ADD YOUR SOURCES BELOW WITHOUT THE SOURCE PREFIX MAIN.C GIVEN AS EXAMPLE
SRCS		=	main.cpp \

#IF YOU HAVE HEADER FILES YOU ABSOLUTELY NEED TO ADD THEM IN THE INC_FILES VAR
INC_FILES	=	\
				pragma.hpp\
				Key.hpp\
				MiniGame.hpp\

INC_PATH	=	includes/
INC			=	-I $(INC_PATH)


SRCS_PATH	=	./sources/
OBJS_PATH	=	./objects/

OBJS		=	$(addprefix $(OBJS_PATH), $(SRCS:.cpp=.o))
DEPS		=	$(OBJS:.o=.d)
COMMANDS	=	$(patsubst %.cpp,$(OBJS_PATH)%.cc,$(SRCS))


all: $(BINARY) $(NAME) compile_commands.json

$(NAME):	$(addprefix $(INC_PATH), $(INC_FILES)) $(addprefix $(SRCS_PATH), $(SRCS))
			@cat $^ | grep -v '#\s*include\s*"' > $(NAME)
			make $(BINARY)
			@echo "Created merged file"

$(BINARY): $(OBJS)
			$(CXX) $(CXXFLAGS) $^ -o $(BINARY) $(INC)


$(OBJS_PATH):
				mkdir -p $(OBJS_PATH)

COMP_COMMAND = $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(INC)
CONCAT = awk 'FNR==1 && NR!=1 {print ","}{print}'

$(OBJS_PATH)%.o $(OBJS_PATH)%.cc:	$(SRCS_PATH)%.cpp
	@mkdir -p $(dir $@)
	$(COMP_COMMAND)
	@printf '{\n\t"directory" : "$(shell pwd)",\n\t"command" : "$(COMP_COMMAND)",\n\t"file" : "$<"\n}' > $(OBJS_PATH)$*.cc
				

compile_commands.json : $(COMMANDS) Makefile
	@echo "Making compile_commands.json"
	@echo "[" > compile_commands.json
	@$(CONCAT) $(COMMANDS) >> compile_commands.json
	@echo "]" >> compile_commands.json

clean:
		rm -rf $(OBJS_PATH)

fclean:		clean
		rm -rf $(NAME) compile_commands.json

re:	fclean
	make all


.PHONY: all clean fclean re test vtest
.SUFFIXES:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules
-include $(DEPS)
