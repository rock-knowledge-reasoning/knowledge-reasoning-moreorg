FROM ubuntu:18.04

MAINTAINER 2maz "https://github.com/2maz"

## BEGIN BUILD ARGUMENTS
# Arguments for creation of the Docker imaged,
# passed via --build-arg

# Mandatory arguments
ARG PKG_NAME=
RUN test -n "$PKG_NAME"
ENV PKG_NAME=${PKG_NAME}

# Optional arguments
ARG PKG_BRANCH="master"
ENV PKG_BRANCH=${PKG_BRANCH}

ARG PKG_PULL_REQUEST="false"
ENV PKG_PULL_REQUEST=${PKG_PULL_REQUEST}
## END ARGUMENTS

RUN apt update
RUN apt upgrade -y

ENV DEBIAN_FRONTEND noninteractive
RUN apt install -y ruby ruby-dev wget tzdata locales g++ autotools-dev make cmake sudo git python3-dev

ENV LANGUAGE=de_DE.UTF-8
ENV LANG=de_DE.UTF-8
ENV LC_ALL=de_DE.UTF-8
ENV SHELL /bin/bash
RUN locale-gen de_DE.UTF-8

RUN echo "Europe/Berlin" > /etc/timezone; dpkg-reconfigure -f noninteractive tzdata
RUN dpkg-reconfigure locales

RUN useradd -ms /bin/bash docker
RUN echo "docker ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER docker
WORKDIR /home/docker

RUN git config --global user.email "rock-users@dfki.de"
RUN git config --global user.name "Rock CI"

RUN wget https://raw.githubusercontent.com/rock-core/autoproj/master/bin/autoproj_bootstrap

RUN mkdir -p /home/docker/rock_test
WORKDIR /home/docker/rock_test
# Use the existing seed configuration
COPY --chown=docker .ci/autoproj-config.yml seed-config.yml
ENV AUTOPROJ_BOOTSTRAP_IGNORE_NONEMPTY_DIR 1
ENV AUTOPROJ_NONINTERACTIVE 1
# Take advantage of available debian packages
RUN ruby /home/docker/autoproj_bootstrap git https://github.com/rock-core/buildconf.git branch=osdeps --seed-config=seed-config.yml
RUN sed -i "s#rock\.core#${PKG_NAME}#g" autoproj/manifest
COPY --chown=docker .ci/deb_blacklist.yml autoproj/deb_blacklist.yml
RUN if [ "$PKG_PULL_REQUEST" = "false" ]; then \
        echo "Using branch: ${PKG_BRANCH}"; \
        echo "overrides:\n  - ${PKG_NAME}:\n    branch: ${PKG_BRANCH}" > autoproj/overrides.yml; \
    fi
RUN echo "Autobuild.displayed_error_line_count = 150" >> autoproj/init.rb
# Activate testing
RUN /bin/bash -c "source env.sh; autoproj test enable ${PKG_NAME}"
## Update
RUN /bin/bash -c "source env.sh; autoproj update; autoproj osdeps"
## Check if this a pull request and change to pull request
## accordingly
RUN if [ "$PKG_PULL_REQUEST" != "false" ]; then \
        echo "Using pull request: ${PKG_PULL_REQUEST}"; \
        cd "${PKG_NAME}"; \
        git fetch autobuild pull/${PKG_PULL_REQUEST}/head:docker_test_pr; \
        git checkout docker_test_pr; \
        cd -; \
        /bin/bash -c "source env.sh; autoproj osdeps;"; \
    fi
RUN /bin/bash -c "source env.sh; amake"
