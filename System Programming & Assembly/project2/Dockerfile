# Use a base image with gcc installed
FROM gcc:latest

# Set the working directory inside the container
WORKDIR /app

# Copy the source code files into the container
COPY . /app

# Compile the code
RUN gcc -o shell shell.c ./my_libs/client_lib.c ./my_libs/server_lib.c ./my_libs/utils.c

# Command to run the server by default
CMD ["./shell", "-s", "-p", "60000", "-i", "127.0.0.1"]
