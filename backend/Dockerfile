# Use official Ubuntu image as the base
FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install C++ build tools and OpenCV
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# Copy all backend files (including the Databases folder) into the container
COPY . .

# Configure and build the C++ server
RUN cmake -B build
RUN cmake --build build --target rubiks_server -j4

# Expose port 8080 so Render can route traffic to it
EXPOSE 8080

# Command to start the server
CMD ["./build/rubiks_server"]
