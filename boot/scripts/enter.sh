docker run --rm -it --name cdt \
  --volume $(pwd)/:/project \
  -w /project \
  dicoop/blockchain:v5.1.0-dev /bin/bash
