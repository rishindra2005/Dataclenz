FROM gcc:latest

WORKDIR /app

# Install git
RUN apt-get update && apt-get install -y git

# Clone the repository
RUN git clone https://github.com/rishindra2005/Dataclenz.git .

# Run make
# RUN make

# Set the default command to bash
# CMD ["cd /app && ./Dataclenz"]