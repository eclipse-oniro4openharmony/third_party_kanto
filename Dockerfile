FROM swr.cn-south-1.myhuaweicloud.com/openharmony-docker/docker_oh_standard:3.2

RUN curl -L -s https://go.dev/dl/go1.21.6.linux-amd64.tar.gz | tar -v -C /usr/local -xz

ENV PATH $PATH:/usr/local/go/bin
