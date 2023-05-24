# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# `base` image
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FROM debian:10 as base

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# `builder` image
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FROM base as builder-base

RUN apt-get update && apt-get install -y build-essential
COPY . /app
WORKDIR /app
RUN make all

CMD ["/app/bin/echo_server", "0.0.0.0", "8080"]

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# `production` image used for runtime
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FROM base as production

WORKDIR /app
COPY --from=builder-base /app/bin /app

CMD ["/app/echo_server", "0.0.0.0", "8080"]
