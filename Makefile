# Colors:
NC      =   \033[0m
LRED    =   \033[0;31m
LGREEN  =   \033[1;92m
TITLE   =   \033[1;36m
# Compile variables
CC              =   c++
FLAGS           =   -Wall -Wextra -Werror -std=c++98 #-g3 -fsanitize=address
COMPILE         =   $(CC) $(FLAGS)
# ************ CODE ************
NAME            =   webserv

DATA_STRUCTURES = 	seLst.cpp \
					multimapFuncs.cpp
PARSER   		=   parse.cpp \
					binaryInsert.cpp \
					cmpLocations.cpp
SOCKETS   		=   socketConnection.cpp \
					eventLoop.cpp
REQUEST    		=  	httpRequest.cpp \
					readChunkedRequest.cpp
RESPONSE    	=   httpMethods.cpp \
					getMethod.cpp \
					cgi.cpp \
					postMultiPartForm.cpp \
					postText.cpp \
					postUrlEncoded.cpp \
					errors.cpp
SEND 			=	httpResponse.cpp 

SRCS			=	main.cpp \
					${DATA_STRUCTURES:%=DATA_STRUCTURES/%} \
					${PARSER:%=PARSER/%} \
					${SOCKETS:%=SOCKETS/%} \
					${REQUEST:%=REQUEST/%} \
					${RESPONSE:%=RESPONSE/%} \
					${SEND:%=SEND/%} \

OBJS            =   ${SRCS:%.cpp=BIN/%.o} 
# Makefile logic
all: $(NAME)
re: fclean all
$(NAME): $(OBJS) 
	@$(COMPILE) $(OBJS) -o $(NAME)
	@echo "${LGREEN}compiled!\n [$(NAME) OK]\n ${NC}"
BIN/%.o:    SRCS/%.cpp
	@mkdir -p $(dir $@)
	@$(COMPILE) -c $< -o $@ 
clean:
	@rm -rf bin
	@echo "\n${LRED}clean!${NC}\n"
fclean: clean
	@rm -f $(NAME)
	@echo "${LRED}deleted!\n${NC}"
.PHONY: all re fclean clean
