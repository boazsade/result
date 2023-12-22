#!/usr/bin/env bash
IMAGE=http_monitoring
THIS_PATH=$PWD
export GID=$(id -g)
docker run --rm -t -d --user $UID:$GID \
    --workdir="${THIS_PATH}" --volume="/etc/group:/etc/group:ro" \
    --name ${IMAGE} -v ${THIS_PATH}:${THIS_PATH} \
    --volume="/etc/passwd:/etc/passwd:ro" \
    --volume="/etc/shadow:/etc/shadow:ro" \
    ${IMAGE}

