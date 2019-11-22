FROM gcc:latest
ARG SSH_KEY
RUN mkdir -p /root/.ssh && \
    chmod 0700 /root/.ssh && \
    ssh-keyscan -H github.com >> /etc/ssh/ssh_known_hosts && \
    echo "${SSH_KEY}" > /root/.ssh/id_rsa && \
    chmod 600 /root/.ssh/id_rsa
WORKDIR /app
RUN apt-get update && apt-get install -y cmake libssl-dev libgl1-mesa-glx
#COPY ./Substance_Automation_Toolkit /opt/Allegorithmic/Substance_Automation_Toolkit/
#COPY ./substance_daemon/ ./
RUN git clone git@github.com:49View/event_horizon.git
RUN cd event_horizon/daemons/substance_daemon/ && cmake -DBUILD_FROM_SOURCE=TRUE .
#ENTRYPOINT ./substance_daemon


#FROM ubuntu
#WORKDIR /app
#RUN apt-get update && apt-get install -y libssl1.0.0 libssl-dev libgl1-mesa-glx
#COPY ./Substance_Automation_Toolkit /opt/Allegorithmic/Substance_Automation_Toolkit/
#COPY ./substance_daemon/ ./
#CMD ./substance_daemon